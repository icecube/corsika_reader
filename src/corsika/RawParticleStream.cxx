/**
 \file
 Implementation file for RawParticleIterator class
 \author Troy Porter
 \version $Id$
 \date 22 May 2003
 */

#include <corsika/RawParticleStream.h>
#include <corsika/IOException.h>

namespace corsika
{
    template <class Thinning> RawParticleStream<Thinning>::RawParticleStream(RawStreamPtr stream, size_t start):
    stream(stream), start(start)
    {
        // if there is something we KNOW, it is that particles are not in block zero.
        if (this->start == 0) this->start = stream->GetNextPosition();
        Rewind();
    }
    template <class Thinning> boost::optional<CorsikaParticle> RawParticleStream<Thinning>::NextParticle()
    {
        if (const ParticleData<Thinning>* d = GetOneParticle())
            return boost::optional<CorsikaParticle>(CorsikaParticle(*d));
        return boost::optional<CorsikaParticle>();
    }
    template <class Thinning> void RawParticleStream<Thinning>::Rewind()
    {
        current_particle = kParticlesInBlock;
        valid = true;
        stream->SeekTo(start);
    }
    template <class Thinning> const ParticleData<Thinning>* RawParticleStream<Thinning>::GetOneParticle()
    {
        if (current_particle == kParticlesInBlock)
        {
            if (!valid) throw IOException("RawParticleIterator not valid.");

            if (!stream->GetNextBlock(block))
                throw IOException("Error reading block in CORSIKA file.");
            
            if (block.IsControl() || block.IsLongitudinal()) // end of particle records
            {
                valid = false;
                return 0;
            }
            current_particle = 0;
        }
        return block.AsParticleBlock.fParticle + current_particle++;
    }
    
    RawParticleStreamPtr VRawParticleStream::Create(RawStreamPtr stream, size_t start)
    {
        if (stream->IsThinned()) return RawParticleStreamPtr(new RawParticleStream<Thinned>(stream, start));
        return RawParticleStreamPtr(new RawParticleStream<NotThinned>(stream, start));
    }
    template struct RawParticleStream<Thinned>;
    template struct RawParticleStream<NotThinned>;
}
