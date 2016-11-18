/**
   \file
   Implementation file for RawParticleIterator class
   \author Troy Porter
   \version $Id$
   \date 22 May 2003
*/

#include <corsika/RawParticleIterator.h>

#include <corsika/RawCorsikaFile.h>
#include <corsika/CorsikaBlock.h>
#include <corsika/CorsikaIOException.h>
#include <corsika/CorsikaParticle.h>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>

#include <iostream>
#include <sstream>
#include <cmath>

using namespace corsika;

#define ERROR(mess) std::cerr << mess << std::endl;
#define INFO(mess) std::cout << mess << std::endl;

using namespace std;

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
RawParticleIterator(const VRawStream& rawStream, size_t startPosition) :
  fRawStream(rawStream.Clone()),
  fStartPosition(startPosition),
  fCurrentBlockIndex(0),
  fParticleInBlock(0),
  fCurrentBlock(),
  fIteratorValid(false),
  fBlockBufferValid(false)
{
  Rewind();
}

template <class Thinning>
bool
RawParticleIterator<Thinning>::operator==(const RawParticleIterator& other) const
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

template <class Thinning>
void
RawParticleIterator<Thinning>::Rewind()
{
  fCurrentBlockIndex = fStartPosition;
  fParticleInBlock = 0;
  fBlockBufferValid = false;
  fIteratorValid = true;
  if (!fRawStream->IsSeekable())
    throw CorsikaIOException("Can not rewind RawParticleIterator because stream is not seekable");
  fRawStream->SeekTo(fCurrentBlockIndex);
}


template <class Thinning>
const typename Block<Thinning>::ParticleData*
RawParticleIterator<Thinning>::GetOneParticle()
{
  using std::ostringstream;

  if (!fIteratorValid)
    throw CorsikaIOException("RawParticleIterator not valid.");

  if (!fBlockBufferValid) {
    if (!dynamic_cast<RawStream<Thinning>& >(*fRawStream).GetNextBlock(fCurrentBlock)) {
      ostringstream msg;
      msg << "Error reading block " << fCurrentBlockIndex << " in CORSIKA file.";
      ERROR(msg.str());
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

  if (fParticleInBlock >= Block<Thinning>::kParticlesInBlock) {
    ++fCurrentBlockIndex;
    fParticleInBlock = 0;
    fBlockBufferValid = false;
  }
  if (!currentRecord) cout << "no record" << endl;
  return currentRecord;
}

namespace corsika
{
    template class RawParticleIterator<Thinned>;
    template class RawParticleIterator<NotThinned>;
}



// Configure (x)emacs for this file ...
// Local Variables:
// mode: c++
// compile-command: "make -C .. -k"
// End:
