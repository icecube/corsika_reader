/**
   \file
   Implement raw CORSIKA file

   \author Lukas Nellen
   \version $Id$
   \date 08 Dec 2003
*/


#include <string>
#include <sstream>
#include <iostream>

#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/operations.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>

#include <corsika/RawStream.h>
#include <corsika/CorsikaBlock.h>


using namespace std;
using std::string;
using namespace corsika::Corsika;

#define ERROR(mess) cerr << mess << endl;


template <class Thinning, int Padding>
RawStream<Thinning, Padding>::RawStream(std::istream& in, bool randomAccess):
  fDiskStream(&in),
  fCurrentBlockNumber(0),
  fDiskBlockBuffer(),
  fIndexInDiskBlock(0),
  fBlockBufferValid(false),
  fRandomAccess(randomAccess)
{
}


template <class Thinning, int Padding>
RawStream<Thinning, Padding>::RawStream(std::istream& in, boost::shared_ptr<std::ifstream> file, bool randomAccess):
  fDiskStream(&in),
  fFile(file),
  fCurrentBlockNumber(0),
  fDiskBlockBuffer(),
  fIndexInDiskBlock(0),
  fBlockBufferValid(false),
  fRandomAccess(randomAccess)
{
}


template <class Thinning, int Padding>
void
RawStream<Thinning, Padding>::Close()
{
  if (!fDiskStream)
    return;

  boost::iostreams::filtering_istream* filter = dynamic_cast<boost::iostreams::filtering_istream*>(fDiskStream);
  std::ifstream* file = dynamic_cast<std::ifstream*>(fDiskStream);
  if (filter) {
    //filter->reset(); // for some reason this fails with bzip2 in cobalt.
    while (!filter->empty()) {
      filter->pop();
    }
  }
  // else if (file) {
  //   file->close();
  // }
}

template <class Thinning, int Padding>
RawStream<Thinning, Padding>::RawStream(const RawStream& other):
  fDiskStream(other.fDiskStream),
  fFile(other.fFile),
  fCurrentBlockNumber(other.fCurrentBlockNumber),
  fDiskBlockBuffer(other.fDiskBlockBuffer),
  fIndexInDiskBlock(other.fIndexInDiskBlock),
  fBlockBufferValid(other.fBlockBufferValid),
  fRandomAccess(other.fRandomAccess)
{
  //cout << "Copy-constructed. Stream at " << fDiskStream->tellg() << ", " << fCurrentBlockNumber << ", " << fIndexInDiskBlock << endl;
}

template <class Thinning, int Padding>
bool
RawStream<Thinning, Padding>::GetNextBlockImpl(Block<Thinning>& theBlock)
{
  //cout << "NextBlockImpl" << endl;
  if (!fBlockBufferValid) {
    if (!ReadDiskBlock()) {
      //cout << "breaking" << endl;
      return false;
    }
  }

  theBlock = fDiskBlockBuffer.fBlock[fIndexInDiskBlock];
  //cout << "next block: " << fCurrentBlockNumber << ", " << fIndexInDiskBlock << ", " << theBlock.ID() << endl;
  if (++fIndexInDiskBlock >= kBlocksInDiskBlock) {
    ++fCurrentBlockNumber;
    fIndexInDiskBlock = 0;
    fBlockBufferValid = false;
  }

  return true;
}


template <class Thinning, int Padding>
bool
RawStream<Thinning, Padding>::ReadDiskBlock()
{
  //cout << "new disk block (currently at " << fDiskStream->tellg() << ": '" << fDiskBlockBuffer.fBlock[0].ID() << "')" << endl;
  const unsigned int size = sizeof(DiskBlock);
  const int rc = boost::iostreams::read(*fDiskStream, reinterpret_cast<char*>(&fDiskBlockBuffer), size);
  if (rc <= 0) {
    return false;
  }
  //cout << "  --> " << fDiskBlockBuffer.fBlock[0].ID() << " at " << fDiskStream->tellg() << endl;
  for (int i = 0; i != Padding; ++i) {
    if (fDiskBlockBuffer.fPaddingBeginning[i] != fDiskBlockBuffer.fPaddingEnd[i]) {
      ostringstream msg;
      msg << "Error reading block. Sizes do not match: " << i <<  ", "<< fDiskBlockBuffer.fPaddingBeginning[i] << " =? " << fDiskBlockBuffer.fPaddingEnd[i] << endl;
      throw CorsikaIOException(msg.str());
    }
  }
  fBlockBufferValid = true;
  return true;
}


template <class Thinning, int Padding>
void
RawStream<Thinning, Padding>::SeekTo(const PositionType thePosition)
{
  //cout << "SeekTo" << endl;
  PositionType newBlockNumber = thePosition / kBlocksInDiskBlock;
  unsigned int newIndexInBlock = thePosition % kBlocksInDiskBlock;
  //if (newBlockNumber == fCurrentBlockNumber && newIndexInBlock == fIndexInDiskBlock)
  //  return;

  //cout << "will SeekTo " << newBlockNumber << ", " << newIndexInBlock << ", " << thePosition << endl;

  if (fRandomAccess) {
    if (fCurrentBlockNumber != newBlockNumber) {
      fCurrentBlockNumber = newBlockNumber;
    }
    fBlockBufferValid   = false;
    fIndexInDiskBlock   = newIndexInBlock;

    const unsigned int size = sizeof(DiskBlock);
    if (fDiskStream->tellg() < 0){
      fDiskStream->clear();
    }
    fDiskStream->seekg(fCurrentBlockNumber * size);
    //cout << "seeked to " << fDiskStream->tellg() << endl;
  }
  else if (fFile) {
    unsigned int current = GetNextPosition();
    if (current > thePosition) {
      Close();
      fFile->close();
      boost::shared_ptr<RawStream<Thinning, Padding> > other = boost::dynamic_pointer_cast<RawStream<Thinning, Padding> >(RawStreamFactory::Create(fName));
      if (!other)
	throw CorsikaIOException("Failed in dumb seek");
      Move(*other);
      current = GetNextPosition();
    }
    Block<Thinning> block;
    while (thePosition > 0 && thePosition > current) {
      GetNextBlock(block);
      current = GetNextPosition();
    }
    current = GetNextPosition();
  }
  else {
    throw CorsikaIOException("This stream is not seekable");
  }
}


template <class Thinning, int Padding>
bool
RawStream<Thinning, Padding>::IsValid()
{
  //cout << "IsValid" << endl;
  if (!fRandomAccess) {
    return true;
  }

  const PositionType currentBlockNumber = GetNextPosition();
  const bool blockBufferValid = fBlockBufferValid;

  bool fail = false;
  Block<Thinning> block;
  SeekTo(0);
  ostringstream msg;
  if (!GetNextBlock(block)) {
    msg << "Failed getting next block" << endl;
    fail = true;
  }
  if (!block.IsRunHeader()) {
    msg << "First block is not run header" << endl;
    fail = true;
  }

  if (fDiskBlockBuffer.fPaddingBeginning[0] !=
      Thinning::kSubBlocksPerBlock*Thinning::kParticlesInBlock*sizeof(typename Block<Thinning>::ParticleData)) {
    msg << "Unexpected block size. "
        << fDiskBlockBuffer.fPaddingBeginning[0] << " != "
        << Thinning::kSubBlocksPerBlock*Thinning::kParticlesInBlock*sizeof(typename Block<Thinning>::ParticleData) << endl;
    fail = true;
  }
  else {
    msg << "So far so good. "
        << fDiskBlockBuffer.fPaddingBeginning[0] << " != "
        << Thinning::kSubBlocksPerBlock*Thinning::kParticlesInBlock*sizeof(typename Block<Thinning>::ParticleData) << endl;
  }
  if (fDiskBlockBuffer.fPaddingBeginning[0] != fDiskBlockBuffer.fPaddingEnd[0]) {
    msg << "Block begin and end do not match. "
        << fDiskBlockBuffer.fPaddingBeginning[0] << " != " << fDiskBlockBuffer.fPaddingEnd[0] << endl;
    fail = true;
  }
  else {
    msg << "So far so good. "
        << fDiskBlockBuffer.fPaddingBeginning[0] << " != " << fDiskBlockBuffer.fPaddingEnd[0] << endl;
  }

  // leave things as they were
  fBlockBufferValid = blockBufferValid;
  SeekTo(currentBlockNumber);

  return !fail;
}


FormatSpec::FormatSpec(std::istream& in):
  thinned(false),
  size_32(false),
  invalid(false)
{
  char buf[10];
  in.read(buf, 8);
  const long long len64 = *reinterpret_cast<long long*>(buf);
  const int len32 = *reinterpret_cast<int*>(buf);
  for (int i = 1; i <= 8; ++i) {
    in.putback(buf[8-i]);
  }

  invalid = false;
  if ( len32 == corsika::Corsika::Thinned::kBytesPerBlock || len64 == corsika::Corsika::Thinned::kBytesPerBlock)
    thinned = true;
  else if ( len32 == corsika::Corsika::NotThinned::kBytesPerBlock || len64 == corsika::Corsika::NotThinned::kBytesPerBlock)
    thinned = false;
  else {
    invalid = true;
  }
  if (len64 != corsika::Corsika::Thinned::kBytesPerBlock && len64 != corsika::Corsika::NotThinned::kBytesPerBlock)
    size_32 = true;
}


boost::shared_ptr<VRawStream>
RawStreamFactory::Create(const std::string& theName)
{
  boost::shared_ptr<std::ifstream> file(new std::ifstream(theName.c_str()));
  if (!(*file)) {
    string msg = "Error opening Corsika file '" + theName + "'.\n";
    ERROR(msg);
    throw std::exception();
  }

  Corsika::Compression c = Corsika::eNone;
  if (boost::algorithm::ends_with(theName, ".bz2")) {
    c = Corsika::eBZip2;
  }
  else if (boost::algorithm::ends_with(theName, ".gz")) {
    c = Corsika::eGZip;
  }
  boost::shared_ptr<VRawStream> corsikaStream(RawStreamFactory::Create(*file, true, c));
  if (!corsikaStream) {
    ostringstream msg;
    msg << "failed to initialize CORSIKA stream from " << theName;
    throw CorsikaIOException(msg.str());
  }
  corsikaStream->SetFile(theName, file);
  return corsikaStream;
}


boost::shared_ptr<VRawStream>
RawStreamFactory::Create(std::istream& in, bool randomAccess, Compression c)
{
  std::istream* input = &in;
  randomAccess *= c==Corsika::eNone;

  FormatSpec spec(in);
  boost::shared_ptr<std::istream> f = GetFilter(in, c);
  if (f) {
    spec = FormatSpec(*f);
    input = f.get();
    c = eNone;
  }
  //cout << spec.String() << endl;

  boost::shared_ptr<VRawStream> ret;
  if (spec.thinned) {
    if (spec.size_32) {
      ret.reset( new RawStream<Corsika::Thinned, 1>(*input, randomAccess));
    }
    else {
      ret.reset( new RawStream<Corsika::Thinned, 2>(*input, randomAccess));
    }
  }
  else {
    if (spec.size_32) {
      ret.reset( new RawStream<Corsika::NotThinned, 1>(*input, randomAccess));
    }
    else {
      ret.reset( new RawStream<Corsika::NotThinned, 2>(*input, randomAccess));
    }
  }
  if (f)
    ret->Hold(f);
  return ret;
}


namespace corsika {
  namespace Corsika{

    boost::shared_ptr<std::istream> GetFilter(std::istream& in, Compression c)
    {
      boost::shared_ptr<std::istream> f;
      if (c != corsika::Corsika::eNone) {
        boost::iostreams::filtering_istream* filter = new boost::iostreams::filtering_istream();
        if (c == corsika::Corsika::eBZip2) {
          //cout << "using bzip2 decompression 2" << endl;
          filter->set_auto_close(false);
          filter->push(boost::iostreams::bzip2_decompressor());
        }
        else if (c == corsika::Corsika::eGZip) {
          //cout << "using gzip decompression 2" << endl;
          filter->push(boost::iostreams::gzip_decompressor());
        }
        filter->push(in);
        f.reset(filter);
      }
      return f;
    }


    template <class Thinning, int Padding>
    FileIndex Scan(corsika::Corsika::RawStream<Thinning, Padding>& stream, bool force)
    {
      FileIndex index;

      if (!force && !stream.IsSeekable())
        return index;

      typename corsika::Corsika::RawStream<Thinning, Padding>::PositionType startingBlockNumber = stream.GetNextPosition();
      if (stream.IsSeekable())
        stream.SeekTo(0);

      int eventsSoFar = 0;

      unsigned int blockIndex = 0;
      bool foundEventHeader = false;
      bool foundRunHeader = false;
      bool foundLongBlock = false;

      Corsika::Block<Thinning> blockUnth;
      while (stream.GetNextBlock(blockUnth) &&
             !blockUnth.IsRunTrailer()) {
        ++blockIndex;
        if (blockUnth.IsEventHeader()) {
          foundEventHeader = true;
          foundLongBlock = false;
          typename Corsika::RawStream<Thinning, Padding>::PositionType rawPosition = stream.GetNextPosition();
          index.eventHeaders.push_back(rawPosition - 1);
          index.IDToPosition[int(blockUnth.AsEventHeader().fEventNumber)] = eventsSoFar;
          ++eventsSoFar;
        }
        else if (blockUnth.IsEventTrailer())
          index.eventTrailers.push_back(stream.GetNextPosition() - 1);
        else if (blockUnth.IsRunHeader()){
          foundRunHeader = true;
          index.runNumber = int(blockUnth.AsRunHeader().fRunNumber);
        }
        else if (!foundLongBlock && blockUnth.IsLongitudinal()) {
          foundLongBlock = true;
          index.longBlocks.push_back(stream.GetNextPosition() - 1);
        }
        if ( blockIndex >400 && !foundRunHeader){
          string msg = "Error scanning Corsika ground file: "
            "could not find run header";
          ERROR(msg);
          throw corsika::CorsikaIOException(msg);
        }
        if ( blockIndex >400 && !foundEventHeader){
          break;
        }
      }

      if (!blockUnth.IsRunTrailer()) {
        ostringstream msg;
        msg << "Error scanning Corsika ground file: could not find run end.";
        if (foundEventHeader) {
          msg << endl << "Found " << eventsSoFar << " event headers.";
        }
        ERROR(msg.str());
        throw corsika::CorsikaIOException(msg.str());
      }

      if (index.eventHeaders.size() != index.eventTrailers.size()) {
        const string err = "Found different number of event-headers and -trailers";
        ERROR(err);
        throw corsika::CorsikaIOException(err);
      }
      if (index.longBlocks.size() > 0 && index.eventHeaders.size() != index.longBlocks.size()) {
        ostringstream msg;
        msg << "Found different number of event-headers and longitudinal blocks ("
            << index.eventHeaders.size() << " != " << index.longBlocks.size() << ")";
        ERROR(msg.str());
        throw corsika::CorsikaIOException(msg.str());
      }

      if (stream.IsSeekable())
        stream.SeekTo(startingBlockNumber);
      return index;
    }


    template FileIndex Scan<Corsika::Thinned, 1>(corsika::Corsika::RawStream<Corsika::Thinned, 1>& stream, bool force);
    template FileIndex Scan<Corsika::NotThinned, 1>(corsika::Corsika::RawStream<Corsika::NotThinned, 1>& stream, bool force);
    template FileIndex Scan<Corsika::Thinned, 2>(corsika::Corsika::RawStream<Corsika::Thinned, 2>& stream, bool force);
    template FileIndex Scan<Corsika::NotThinned, 2>(corsika::Corsika::RawStream<Corsika::NotThinned, 2>& stream, bool force);

  }
}

namespace corsika
{
    namespace Corsika
    {
        template class RawStream<Thinned, 1>;
        template class RawStream<NotThinned, 1>;
        
        template class RawStream<Thinned, 2>;
        template class RawStream<NotThinned, 2>;
    }
}



// Configure (x)emacs for this file ...
// Local Variables:
// mode:c++
// compile-command: "make -C .. -k"
// End:
