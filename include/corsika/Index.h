/**
 \file
 Index
 \author Javier Gonzalez
 \version $Id$
 \date Feb 24 2015
 */

#pragma once
#include <boost/shared_ptr.hpp>
#include <corsika/Units.h>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <iostream>
#include <memory>


namespace corsika
{
    namespace detail
    {
        size_t digitize(const std::vector<double>& arr, double x);
        bool less(double i, double j);
        bool less_equal(double i, double j);
        bool greater(double i, double j);
        bool greater_equal(double i, double j);
    }
    
    /**
     \class SquareCollision
     
     \brief Collision policy for a PositionIndex that uses squares around pointee objects to assign index bins.
     
     The squares are aligned with the axes of the PositionIndex. Pointee objects must provide x, y, dx, and dy member functions.
     
     \author Javier Gonzalez
     */
    
    template<class Pointee>
    class SquareCollision
    {
    public:
        std::vector<std::pair<unsigned int,unsigned int> > operator()(const std::vector<double>& x, const std::vector<double>& y, const Pointee& p) const
        {
            const unsigned int i_min = detail::digitize(x, p.x() - p.dx()/2);
            const unsigned int i_max = detail::digitize(x, p.x() + p.dx()/2);
            const unsigned int j_min = detail::digitize(y, p.y() - p.dy()/2);
            const unsigned int j_max = detail::digitize(y, p.y() + p.dy()/2);
            std::vector<std::pair<unsigned int,unsigned int> > bins;
            for (unsigned int i = i_min; i <= i_max; ++i) {
                for (unsigned int j = j_min; j <= j_max; ++j) {
                    bins.push_back(std::pair<int,int>(i,j));
                }
            }
            return bins;
        }
    private:
        int GetIndex(const std::vector<double>& arr, double x) const
        {
            if (x<arr[0])
                return 0;
            return  arr.size() - (std::upper_bound(arr.rbegin(), arr.rend(), x, detail::greater_equal) - arr.rbegin());
        }
    };
    
    /**
     \class PositionIndex Index.h "Index.h"
     
     \brief Class to index objects based on position.
     
     The objects are sorted in a 2d grid centered at (0,0). The size and
     spacing of the grid (the same for both axes) can be passed in the constructor.
     The rational for this class is that very often the CORSIKA files are large,
     so we want to iterate over particles only once. For each particle, we usually
     need to know what detector element or sampling area could detect this particle.
     This class reduces the iteration over the later.
     
     Each pointee object can be assigned to multiple bins in the grid (including over-/under-flow bins).
     The assignment is done by a separate class, the CollisionPolicy template parameter,
     that is called when pointee objects are added to the index. The requirements on the
     pointee class are given by the collision policy.
     
     The collision policy needs to provide a call operator:
     operator()(const std::vector<double>& x, const std::vector<double>& y, const Pointee& p)
     that takes the bin edges for both axes and a const reference to a pointee object.
     
     \author Javier Gonzalez
     */
    template<class Pointee, class CollisionPolicy=SquareCollision<Pointee> >
    class PositionIndex
    {
    public:
        
        typedef Pointee value_type;
        typedef boost::shared_ptr<Pointee> pointer_type;
        typedef std::vector<boost::shared_ptr<Pointee> > pointer_list_type;
        
        /**
         Constructor takes grid size and bin size as arguments. The
         result is a square grid centered at zero. The bin size is
         modified to guarantee an integer number of bins.
         */
        PositionIndex(double x=50*km, double dx = 100*m):
        fSize(2, x),
        fSpacing(2),
        fBinEdges(2)
        {
            const unsigned int s(std::floor(x/dx)+2); // includes over/under-flow
            fGrid.resize(s);
            for (unsigned int i=0; i != s; ++i) {
                fGrid[i].resize(s);
            }
            for (unsigned int i = 0; i != fSize.size(); ++i) {
                fSpacing[i] = fSize[i]/(s - 2);
                fBinEdges[i].resize(s-1);
                for (unsigned int j=0; j!=s-1; ++j) {
                    fBinEdges[i][j] = j*fSpacing[i] - fSize[i]/2;
                }
            }
        }
        
        /**
         Constructor takes (min, max, n_bins) for each axis as arguments.
         */
        PositionIndex(double xmin, double xmax, unsigned int xbins, double ymin, double ymax, unsigned int ybins):
        fSize(2),
        fSpacing(2),
        fBinEdges(2)
        {
            fSize[0] = xmax-xmin;
            fSize[1] = ymax-ymin;
            
            fGrid.resize(xbins+2); // include over/under-flow
            for (unsigned int i=0; i != fGrid.size(); ++i) {
                fGrid[i].resize(ybins+2);
            }
            std::vector<double> min = {xmin, ymin};
            std::vector<unsigned int> shape = {xbins, ybins};
            for (unsigned int i = 0; i != shape.size(); ++i) {
                fSpacing[i] = fSize[i]/shape[i];
                fBinEdges[i].resize(shape[i]+1);
                for (unsigned int j=0; j!=fBinEdges[i].size(); ++j) {
                    fBinEdges[i][j] = min[i] + j*fSpacing[i];
                }
            }
        }
        
        ~PositionIndex() {}
        
        double GetSize(unsigned int i) const
        { return fSize[i]; }
        double GetSpacing(unsigned int i) const
        { return fSpacing[i]; }
        int GetBins(unsigned int i) const
        { return fBinEdges[i].size() - 1; }
        
        void Add(boost::shared_ptr<Pointee> p)
        {
            if (std::find(fAll.begin(), fAll.end(), p) != fAll.end())
                return;
            std::vector<std::pair<unsigned int,unsigned int> > indices = fCollisionPolicy(fBinEdges[0], fBinEdges[1], *p);
            fAll.push_back(p);
            for (std::vector<std::pair<unsigned int,unsigned int> >::iterator it = indices.begin(); it != indices.end(); ++it) {
                std::vector<boost::shared_ptr<Pointee> >& v = fGrid[it->first][it->second];
                if (std::find(v.begin(), v.end(), p) == v.end()) {
                    v.push_back(p);
                }
            }
        }
        
        void Add(const Pointee& v)
        {
            Add(boost::shared_ptr<Pointee>(new Pointee(v)));
        }
        void Add(std::vector<boost::shared_ptr<Pointee> > v)
        {
            for (typename std::vector<boost::shared_ptr<Pointee> >::const_iterator it = v.begin(); it != v.end(); ++it) {
                Add(*it);
            }
        }
        void Add(std::vector<Pointee> v)
        {
            for (typename std::vector<boost::shared_ptr<Pointee> >::const_iterator it = v.begin(); it != v.end(); ++it) {
                Add(*it);
            }
        }
        
        const std::vector<boost::shared_ptr<Pointee> >& Get(double x, double y) const
        { return fGrid[GetIndex(0, x)][GetIndex(1, y)]; }
        
        std::vector<boost::shared_ptr<Pointee> >& Get(double x, double y)
        { return fGrid[GetIndex(0, x)][GetIndex(1, y)]; }
        
        const std::vector<boost::shared_ptr<Pointee> >& GetAll() const
        { return fAll; }
        
        void Clear()
        {
            for (unsigned int i=0; i!=fGrid.size(); ++i) {
                for (unsigned int j=0; j!=fGrid[i].size(); ++j) {
                    fGrid[i][j].clear();
                }
            }
        }
        
        bool IsEmpty(double x, double y) const
        { return fGrid[GetIndex(0, x)][GetIndex(1, y)].empty(); }
        
    private:
        unsigned int GetIndex(unsigned int i, double x) const
        {
            return detail::digitize(fBinEdges[i], x);
        }
        
        CollisionPolicy fCollisionPolicy;
        std::vector<double> fSize;
        std::vector<double> fSpacing;
        
        std::vector<std::vector<double> > fBinEdges;
        std::vector<std::vector<std::vector<boost::shared_ptr<Pointee> > > > fGrid;
        std::vector<boost::shared_ptr<Pointee> > fAll;
    };
    
}
