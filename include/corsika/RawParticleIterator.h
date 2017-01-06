#pragma once
#include <corsika/CorsikaBlock.h>
#include <corsika/CorsikaParticle.h>
#include <corsika/RawStream.h>
#include <boost/optional.hpp>

namespace corsika
{
    typedef boost::shared_ptr<struct VRawParticleIterator> RawParticleIteratorPtr;
    struct VRawParticleIterator
    {
        virtual boost::optional<CorsikaParticle> GetCorsikaParticle() = 0;
        virtual void Rewind() = 0;
        virtual bool IsValid() const = 0;
        virtual ~VRawParticleIterator(){}
        
        // Returns next particle or null if there are no more particles.
        virtual CorsikaParticle* Next(){ return 0; };
        
        static RawParticleIteratorPtr Create(RawStreamPtr stream, size_t start=0);
    };
    template <class Thinning> struct RawParticleIterator: VRawParticleIterator
    {
        RawParticleIterator(RawStreamPtr stream, size_t start);
        boost::optional<CorsikaParticle> GetCorsikaParticle();
        const ParticleData<Thinning>* GetOneParticle();
        void Rewind();
        bool IsValid() const { return valid; }
        
        CorsikaParticle* Next();

    private:
        CorsikaParticle particle;
        RawStreamPtr stream;
        size_t start;
        size_t current_particle;
        Block<Thinning> block;
        bool valid;
    };
}
