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
    
    template <class Thinning>
    RawParticleIterator<Thinning>::
    RawParticleIterator() :
    fStartPosition(0),
    fCurrentBlockIndex(0),
    fParticleInBlock(0),
    fCurrentBlock(),
    fIteratorValid(false),
    fBlockBufferValid(false)
    { }
    
    template <class Thinning>
    RawParticleIterator<Thinning>::
    RawParticleIterator(RawStreamPtr rawStream, size_t startPosition) :
    fRawStream(rawStream),
    fStartPosition(startPosition),
    fCurrentBlockIndex(0),
    fParticleInBlock(0),
    fCurrentBlock(),
    fIteratorValid(false),
    fBlockBufferValid(false)
    {
        // if there is something we KNOW, it is that particles are not in block zero.
        if (fStartPosition == 0) fStartPosition = rawStream->GetNextPosition();
        Rewind();
    }
    
    template <class Thinning> bool RawParticleIterator<Thinning>::operator==(const RawParticleIterator& other) const
    {
        if (!fRawStream || !other.fRawStream || !fIteratorValid || !other.fIteratorValid) {
            return fIteratorValid == other.fIteratorValid;
        }
        
        bool ret =
        fStartPosition    == other.fStartPosition &&
        fCurrentBlockIndex  == other.fCurrentBlockIndex &&
        fParticleInBlock  == other.fParticleInBlock &&
        fBlockBufferValid == other.fBlockBufferValid;
        
        return ret;
    }
    
    template <class Thinning> void RawParticleIterator<Thinning>::Rewind()
    {
        fCurrentBlockIndex = fStartPosition;
        fParticleInBlock = 0;
        fBlockBufferValid = false;
        fIteratorValid = true;
        if (!fRawStream->IsSeekable())
            throw CorsikaIOException("Can not rewind RawParticleIterator because stream is not seekable");
        fRawStream->SeekTo(fCurrentBlockIndex);
    }
    
    
    template <class Thinning> const typename Block<Thinning>::ParticleData* RawParticleIterator<Thinning>::GetOneParticle()
    {
        if (!fIteratorValid)
            throw CorsikaIOException("RawParticleIterator not valid.");
        
        if (!fBlockBufferValid)
        {
            if (!fRawStream->GetNextBlock(fCurrentBlock))
            {
                std::ostringstream msg;
                msg << "Error reading block " << fCurrentBlockIndex << " in CORSIKA file.";
                throw CorsikaIOException(msg.str());
            }
            if (fCurrentBlock.IsControl() || fCurrentBlock.IsLongitudinal()) { // end of particle records
                fIteratorValid = false;
                return 0;
            }
            fBlockBufferValid = true;
        }
        
        const typename Block<Thinning>::ParticleData * currentRecord =
        fCurrentBlock.AsParticleBlock().fParticle + fParticleInBlock;
        ++fParticleInBlock;
        
        if (fParticleInBlock >= Block<Thinning>::kParticlesInBlock)
        {
            ++fCurrentBlockIndex;
            fParticleInBlock = 0;
            fBlockBufferValid = false;
        }
        return currentRecord;
    }
    
    template class RawParticleIterator<Thinned>;
    template class RawParticleIterator<NotThinned>;
    
    boost::shared_ptr<VRawParticleIterator> VRawParticleIterator::Create(RawStreamPtr stream, size_t start)
    {
        if (stream->IsThinned()) return boost::shared_ptr<VRawParticleIterator>(new RawParticleIterator<Thinned>(stream, start));
        return boost::shared_ptr<VRawParticleIterator>(new RawParticleIterator<NotThinned>(stream, start));
    }
}
