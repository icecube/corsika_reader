#pragma once
#include <corsika/RawParticleIterator.h>
#include <boost/optional.hpp>

namespace corsika
{
    //
    // High-Level particle iterator that records the particles' history in there parent and grandparent fields.
    //
    struct CorsikaShowerFileParticleIterator
    {
        CorsikaShowerFileParticleIterator(){}
        CorsikaShowerFileParticleIterator(RawStreamPtr stream, size_t start, double timeOffset, int observationLevel, bool keepMuProd);
        virtual void Rewind() { iterator_->Rewind(); }
        boost::optional<CorsikaParticle> NextParticle();
        
    private:
        boost::optional<CorsikaParticle> value_;
        RawParticleIteratorPtr iterator_;
        double fTimeOffset;
        int fObservationLevel;
        bool fKeepMuProd;
    };
}
