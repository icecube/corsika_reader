/**
 \file
 Implementation of the VShowerFileParticleIterator for an Corsika generated
 shower file
 \author Troy Porter
 \author Lukas Nellen
 \version $Id$
 \date 22 May 2003
 */

#pragma once

#include <corsika/CorsikaBlock.h>
#include <corsika/CorsikaParticle.h>
#include <boost/optional.hpp>

namespace corsika
{
    class VRawStream;
    class CorsikaParticle;
    
    struct VRawParticleIterator
    {
        virtual boost::optional<CorsikaParticle> GetCorsikaParticle() const = 0;
        virtual void Rewind() = 0;
        virtual bool IsValid() const = 0;
    };
    
    /**
     \template RawParticleIterator
     \brief Iterator for particles in a corsika generated shower file
     \author Troy Porter
     \author Lukas Nellen
     \author Javier Gonzalez
     \version $Id$
     \date 22 May 2003
     */
    
    template <class Thinning> struct RawParticleIterator: VRawParticleIterator
    {
        RawParticleIterator();
        RawParticleIterator(boost::shared_ptr<VRawStream> rawStream, size_t startPosition);
        
        ~RawParticleIterator() { }
        
        virtual boost::optional<CorsikaParticle> GetCorsikaParticle() const
        {
            const typename Block<Thinning>::ParticleData* d = GetOneParticle();
            if(d) {
                CorsikaParticle p(d);
                return boost::optional<CorsikaParticle>(p);
            }
            return boost::optional<CorsikaParticle>();
        }
        
        const typename Block<Thinning>::ParticleData* GetOneParticle();
        
        const typename Block<Thinning>::ParticleData* GetOneParticle() const
        { return const_cast<RawParticleIterator<Thinning>*>(this)->GetOneParticle(); }
        
        void Rewind();
        
        bool operator==(const RawParticleIterator& other) const;
        
        bool IsValid() const
        { return fIteratorValid; }
        
    private:
        
        boost::shared_ptr<VRawStream> fRawStream;
        
        size_t fStartPosition;
        size_t fCurrentBlockIndex;
        int fParticleInBlock;
        
        Block<Thinning> fCurrentBlock;
        
        bool fIteratorValid;
        bool fBlockBufferValid;
    };
}
