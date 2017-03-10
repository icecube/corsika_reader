#pragma once
#include <corsika/Block.h>
#include <corsika/Particle.h>
#include <corsika/RawStream.h>
#include <boost/optional.hpp>

namespace corsika
{
    typedef boost::shared_ptr<struct VRawParticleStream> RawParticleStreamPtr;
    struct VRawParticleStream
    {
        virtual boost::optional<Particle> NextParticle() = 0;
        virtual void Rewind() = 0;
        virtual bool IsValid() const = 0;
        virtual ~VRawParticleStream(){}
        
        // Returns next particle or null if there are no more particles.
        virtual Particle* Next(){ return 0; };
        
        static RawParticleStreamPtr Create(RawStreamPtr stream, size_t start=0);
    };
    template <class Thinning> struct RawParticleStream: VRawParticleStream
    {
        RawParticleStream(RawStreamPtr stream, size_t start);
        boost::optional<Particle> NextParticle();
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
