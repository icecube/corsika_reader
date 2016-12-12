/**
 \file
 Implementation file for RawParticleIterator class
 \author Troy Porter
 \version $Id$
 \date 22 May 2003
 */

#include <corsika/RawParticleIterator.h>
#include <corsika/RawStream.h>
#include <corsika/CorsikaIOException.h>
#include <sstream>

namespace corsika
{
    template <class Thinning> RawParticleIterator<Thinning>::RawParticleIterator(RawStreamPtr stream, size_t start):
    stream(stream), start(start)
    {
        // if there is something we KNOW, it is that particles are not in block zero.
        if (this->start == 0) this->start = stream->GetNextPosition();
        Rewind();
    }
    template <class Thinning> boost::optional<CorsikaParticle> RawParticleIterator<Thinning>::GetCorsikaParticle()
    {
        if (const ParticleData<Thinning>* d = GetOneParticle())
            return boost::optional<CorsikaParticle>(CorsikaParticle(*d));
        return boost::optional<CorsikaParticle>();
    }
    template <class Thinning> void RawParticleIterator<Thinning>::Rewind()
    {
        current_particle = kParticlesInBlock;
        valid = true;
        stream->SeekTo(start);
    }
    template <class Thinning> const ParticleData<Thinning>* RawParticleIterator<Thinning>::GetOneParticle()
    {
        if (current_particle == kParticlesInBlock)
        {
            if (!valid) throw CorsikaIOException("RawParticleIterator not valid.");

            if (!stream->GetNextBlock(block))
                throw CorsikaIOException("Error reading block in CORSIKA file.");
            
            if (block.IsControl() || block.IsLongitudinal()) // end of particle records
            {
                valid = false;
                return 0;
            }
            current_particle = 0;
        }
        return block.AsParticleBlock.fParticle + current_particle++;
    }
    
    boost::shared_ptr<VRawParticleIterator> VRawParticleIterator::Create(RawStreamPtr stream, size_t start)
    {
        if (stream->IsThinned()) return boost::shared_ptr<VRawParticleIterator>(new RawParticleIterator<Thinned>(stream, start));
        return boost::shared_ptr<VRawParticleIterator>(new RawParticleIterator<NotThinned>(stream, start));
    }
    template struct RawParticleIterator<Thinned>;
    template struct RawParticleIterator<NotThinned>;
}
