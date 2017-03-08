#pragma once
#include <corsika/RawParticleStream.h>
#include <boost/optional.hpp>

namespace corsika
{
    //
    // High-Level particle iterator that records the particles' history in there parent and grandparent fields.
    //
    struct ShowerParticleStream
    {
        ShowerParticleStream(){}
        ShowerParticleStream(RawStreamPtr stream, size_t start, double timeOffset, int observationLevel, bool keepMuProd);
        virtual void Rewind() { stream->Rewind(); }
        boost::optional<CorsikaParticle> NextParticle();
        
    private:
        boost::optional<CorsikaParticle> value_;
        RawParticleStreamPtr stream;
        double fTimeOffset;
        int fObservationLevel;
        bool fKeepMuProd;
    };
}
