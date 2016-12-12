#pragma once
#include <corsika/CorsikaBlock.h>
#include <corsika/CorsikaParticle.h>
#include <corsika/RawStream.h>
#include <boost/optional.hpp>

namespace corsika
{
    struct VRawParticleIterator
    {
        virtual boost::optional<CorsikaParticle> GetCorsikaParticle() = 0;
        virtual void Rewind() = 0;
        virtual bool IsValid() const = 0;
        static std::shared_ptr<VRawParticleIterator> Create(RawStreamPtr stream, size_t start=0);
    };
    template <class Thinning> struct RawParticleIterator: VRawParticleIterator
    {
        RawParticleIterator(RawStreamPtr stream, size_t start);
        boost::optional<CorsikaParticle> GetCorsikaParticle();
        const ParticleData<Thinning>* GetOneParticle();
        void Rewind();
        bool IsValid() const { return valid; }

    private:
        RawStreamPtr stream;
        size_t start;
        size_t current_particle;
        Block<Thinning> block;
        bool valid;
    };
}
