#include "tests.h"

namespace
{
    using corsika::km;
    using corsika::m;
    using corsika::cm;
    
    struct pointee
    {
        double x_,y_,dx_,dy_;
        pointee(double x, double y, double dx, double dy): x_(x),y_(y),dx_(dx),dy_(dy) {}
        double x() const { return x_; }
        double dx() const { return dx_; }
        double y() const { return y_; }
        double dy() const { return dy_; }
        bool operator==(const pointee& other) const { return x_==other.x_ && dx_==other.dx_ && y_==other.y_ && dy_==other.dy_; }
    };
    
    void test_details()
    {
        double a[11] = {-10., -8., -6., -4., -2., 0., 2., 4., 6., 8., 10.};
        std::vector<double> edges;
        edges.assign(a, a + 11);
        ENSURE_EQUAL(edges.size(), (unsigned)11);
        ENSURE_EQUAL(corsika::detail::digitize(edges, -11.), (unsigned)0);
        ENSURE_EQUAL(corsika::detail::digitize(edges, 11.), (unsigned)11);
        ENSURE_EQUAL(corsika::detail::digitize(edges, -10.), (unsigned)1);
        ENSURE_EQUAL(corsika::detail::digitize(edges, -9.5), (unsigned)1);
        ENSURE_EQUAL(corsika::detail::digitize(edges, -7.5), (unsigned)2);
        ENSURE_EQUAL(corsika::detail::digitize(edges, 2.), (unsigned)7);
        
        // these fail with std::upper_bound(arr.begin(), arr.end(), x, less);
        ENSURE_EQUAL(corsika::detail::digitize(edges, -6.5), (unsigned)2);
        ENSURE_EQUAL(corsika::detail::digitize(edges, -4.5), (unsigned)3);
        ENSURE_EQUAL(corsika::detail::digitize(edges, -2.5), (unsigned)4);
        ENSURE_EQUAL(corsika::detail::digitize(edges, -0.5), (unsigned)5);
        
        
        double a2[21] = {-10., -9., -8., -7., -6., -5., -4., -3., -2., -1., 0.,
            1., 2., 3., 4., 5., 6., 7., 8., 9., 10.};
        std::vector<double> edges2;
        edges2.assign(a2, a2 + 21);
        ENSURE_EQUAL(edges2.size(), (unsigned)21);
        
        ENSURE_EQUAL(*std::upper_bound(edges2.begin(), edges2.end(), 0.5, corsika::detail::less), 1.);
        ENSURE_EQUAL(*std::upper_bound(edges2.begin(), edges2.end(), -0.5, corsika::detail::less), 0.);
        
        ENSURE_EQUAL(corsika::detail::digitize(edges2, 0.5), (unsigned)11);
        ENSURE_EQUAL(corsika::detail::digitize(edges2, -0.5), (unsigned)10);
        ENSURE_EQUAL(corsika::detail::digitize(edges2, -9.5), (unsigned)1);
        ENSURE_EQUAL(corsika::detail::digitize(edges2, 10.1), (unsigned)21);
        
        double a3[11] = {-10., -8., -8., -8., -2., 0., 0., 4., 6., 8., 10.};
        std::vector<double> edges3;
        edges3.assign(a3, a3 + 11);
        ENSURE_EQUAL(edges3.size(), (unsigned)11);
        ENSURE_EQUAL(corsika::detail::digitize(edges3, 0.5), (unsigned)7);
        ENSURE_EQUAL(corsika::detail::digitize(edges3, -7.5), (unsigned)4);
    }
    
    void test_square_policy()
    {
        double a[21] = {-10., -9., -8., -7., -6., -5., -4., -3., -2., -1., 0.,
            1., 2., 3., 4., 5., 6., 7., 8., 9., 10.};
        std::vector<double> edges(21);
        edges.assign(a, a + 21);
        
        corsika::SquareCollision<pointee> policy;
        
        typedef std::pair<unsigned int,unsigned int> IndexPair;
        pointee p(5.5, -9.5, 0.01, 0.01);
        std::vector<std::pair<unsigned int,unsigned int> > bins = policy(edges, edges, p);
        ENSURE_EQUAL(bins.size(), (unsigned)1, "Bins. Tiny area centered in a bin");
        ENSURE_EQUAL(bins[0].first, (unsigned)16, "bins[0].first. Tiny area centered in a bin");
        ENSURE_EQUAL(bins[0].second, (unsigned)1, "bins[0].second. Tiny area centered in a bin");
        
        std::cout << std::endl;
        pointee p2(5., -2., 0.01, 0.01);
        std::vector<std::pair<unsigned int,unsigned int> > bins2 = policy(edges, edges, p2);
        for (unsigned int i = 0; i != bins2.size(); ++i) {
            std::cout << "test_square_policy: (" << bins2[i].first << ", " << bins2[i].second << ")" << std::endl;
        }
        ENSURE_EQUAL(bins2.size(), (unsigned)4, "Square collision, tiny area right at the edge of a bin");
        
        pointee p3(5.5, -2.5, 3.5, 1.5);
        std::vector<std::pair<unsigned int,unsigned int> > bins3 = policy(edges, edges, p3);
        ENSURE_EQUAL(bins3.size(), (unsigned)15, "Square collision, bin-centered, size 3.5 and 1.5 bin size");
        
        pointee p4(5.5, -2.5, 3., 3.);
        std::vector<std::pair<unsigned int,unsigned int> > bins4 = policy(edges, edges, p4);
        ENSURE_EQUAL(bins4.size(), (unsigned)16, "Square collision, bin-centered, size 3. bin size (clipping the upper bin)");
        
    }
    
    void test_square_index()
    {
        corsika::PositionIndex<pointee> index(4*corsika::km, 50*corsika::m);
        pointee p(1*km, -1.5*km, 500*m, 100*m);
        index.Add(p);
        
        ENSURE_EQUAL(index.GetSize(0), 4*corsika::km, "PositionIndex grid size mismatch");
        ENSURE_EQUAL(index.GetSpacing(0), 50*corsika::m, "PositionIndex grid spacing mismatch");
        ENSURE_EQUAL(index.GetSize(1), 4*corsika::km, "PositionIndex grid size mismatch");
        ENSURE_EQUAL(index.GetSpacing(1), 50*corsika::m, "PositionIndex grid spacing mismatch");
        
        corsika::PositionIndex<pointee>::pointer_list_type v = index.Get(0,0);
        ENSURE_EQUAL(v.size(), (unsigned)0, "I know there is no pointee there!");
        
        v = index.Get(p.x(), p.y());
        ENSURE_EQUAL(v.size(), (unsigned)1, "There should be only one pointee.");
        
        // these work because dx/2 and dy/2 are multiples of the grid separation
        v = index.Get(p.x() - p.dx()/2, p.y());
        ENSURE_EQUAL(v.size(), (unsigned)1, "x-axis lower edge: There should be exactly one pointee.");
        v = index.Get(p.x() + p.dx()/2, p.y());
        ENSURE_EQUAL(v.size(), (unsigned)1, "x-axis upper edge: There should be exactly one pointee.");
        v = index.Get(p.x() - p.dx()/2 - 0.0001*cm, p.y());
        ENSURE_EQUAL(v.size(), (unsigned)0, "x-axis lower edge-: I know there is no pointee there!");
        v = index.Get(p.x() - p.dx()/2 + 0.0001*cm, p.y());
        ENSURE_EQUAL(v.size(), (unsigned)1, "x-axis lower edge+: There should be exactly one pointee.");
        // binning policy interval is closed below, the upper edge clips the bin right above
        v = index.Get(p.x() + p.dx()/2 + index.GetSpacing(0) - 0.0001*cm, p.y());
        ENSURE_EQUAL(v.size(), (unsigned)1, "x-axis upper edge-: There should be exactly one pointee.");
        v = index.Get(p.x() + p.dx()/2 + index.GetSpacing(0) + 0.0001*cm, p.y());
        ENSURE_EQUAL(v.size(), (unsigned)0, "x-axis upper edge+: I know there is no pointee there!");
        
        v = index.Get(p.x(), p.y() - p.dy()/2);
        ENSURE_EQUAL(v.size(), (unsigned)1, "y-axis lower edge: There should be exactly one pointee.");
        v = index.Get(p.x(), p.y() + p.dy()/2);
        ENSURE_EQUAL(v.size(), (unsigned)1, "y-axis upper edge: There should be exactly one pointee.");
        v = index.Get(p.x(), p.y() - p.dy()/2 - 0.0001*cm);
        ENSURE_EQUAL(v.size(), (unsigned)0, "y-axis lower edge-: I know there is no pointee there!");
        v = index.Get(p.x(), p.y() - p.dy()/2 + 0.0001*cm);
        ENSURE_EQUAL(v.size(), (unsigned)1, "y-axis lower edge+: There should be exactly one pointee.");
        // binning policy interval is closed below, the upper edge clips the bin right above
        v = index.Get(p.x(), p.y() + p.dy()/2 + index.GetSpacing(0) - 0.0001*cm);
        ENSURE_EQUAL(v.size(), (unsigned)1, "y-axis upper edge-: There should be exactly one pointee.");
        v = index.Get(p.x(), p.y() + p.dy()/2 + index.GetSpacing(0) + 0.0001*cm);
        ENSURE_EQUAL(v.size(), (unsigned)0, "y-axis upper edge+: I know there is no pointee there!");
        
        index.Add(pointee(1*km, -3*km, 500*m, 100*m));
        ENSURE_EQUAL(index.Get(1*km, -10*km).size(), (unsigned)1, "y-axis underflow bin");
        index.Add(pointee(3*km, -3*km, 500*m, 100*m));
        ENSURE_EQUAL(index.Get(10*km, -10*km).size(), (unsigned)1, "(x/y)-axis (over/under)flow bin");
        
        corsika::PositionIndex<pointee> index2(40*corsika::km, 45*corsika::m);
        ENSURE_EQUAL(index2.GetBins(0), 888, "PositionIndex number of bins round mismatch.");
        ENSURE_EQUAL(index2.GetSpacing(0), index2.GetSize(0)/index2.GetBins(0), "PositionIndex grid spacing mismatch");
        assert(index2.GetSpacing(0) != 45*m); // see? it is not 45 m
        ENSURE_EQUAL(index2.GetBins(1), 888, "PositionIndex number of bins round mismatch.");
        ENSURE_EQUAL(index2.GetSpacing(1), index2.GetSize(1)/index2.GetBins(1), "PositionIndex grid spacing mismatch");
        assert(index2.GetSpacing(1) != 45*m); // see? it is not 45 m
        
    }
    
    void test_rectangular_index()
    {
        corsika::PositionIndex<pointee> index(0., 4*corsika::km, 80, 1.*corsika::km, 3.*corsika::km, 20);
        pointee p(1*km, 1.5*km, 500*m, 100*m);
        index.Add(p);
        
        ENSURE_EQUAL(index.GetSize(0), 4*corsika::km, "PositionIndex grid size mismatch");
        ENSURE_EQUAL(index.GetSpacing(0), 50*corsika::m, "PositionIndex grid spacing mismatch");
        ENSURE_EQUAL(index.GetSize(1), 2*corsika::km, "PositionIndex grid size mismatch");
        ENSURE_EQUAL(index.GetSpacing(1), 100*corsika::m, "PositionIndex grid spacing mismatch");
        ENSURE_EQUAL(index.GetBins(0), 80);
        ENSURE_EQUAL(index.GetBins(1), 20);
        
        corsika::PositionIndex<pointee>::pointer_list_type v = index.Get(0,0);
        ENSURE_EQUAL(v.size(), (unsigned)0, "I know there is no pointee there!");
        
        v = index.Get(p.x(), p.y());
        ENSURE_EQUAL(v.size(), (unsigned)1, "There should be only one pointee.");
    }
}
void test_index()
{
    test_details();
    test_square_policy();
    test_square_index();
    test_rectangular_index();
    printf("TestIndex Successfull!\n");
}
