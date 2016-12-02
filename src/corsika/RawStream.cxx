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
using namespace corsika;

template <class Thinning, int Padding>
RawStream<Thinning, Padding>::RawStream(std::istream& in, boost::shared_ptr<std::ifstream> file, std::string filename, bool randomAccess):
VRawStream(filename, file),
  fDiskStream(&in),
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
  long rc = boost::iostreams::read(*fDiskStream, reinterpret_cast<char*>(&fDiskBlockBuffer), size);
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
void RawStream<Thinning, Padding>::SeekTo(size_t thePosition)
{
  //cout << "SeekTo" << endl;
  size_t newBlockNumber = thePosition / kBlocksInDiskBlock;
  size_t newIndexInBlock = thePosition % kBlocksInDiskBlock;
  //if (newBlockNumber == fCurrentBlockNumber && newIndexInBlock == fIndexInDiskBlock)
  //  return;

  //cout << "will SeekTo " << newBlockNumber << ", " << newIndexInBlock << ", " << thePosition << endl;

  if (fRandomAccess) {
    if (fCurrentBlockNumber != newBlockNumber) {
      fCurrentBlockNumber = newBlockNumber;
    }
    fBlockBufferValid   = false;
    fIndexInDiskBlock   = newIndexInBlock;

    const size_t size = sizeof(DiskBlock);
    if (fDiskStream->tellg() < 0){
      fDiskStream->clear();
    }
    fDiskStream->seekg(fCurrentBlockNumber * size);
    //cout << "seeked to " << fDiskStream->tellg() << endl;
  }
  else if (fFile) {
    size_t current = GetNextPosition();
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

  const size_t currentBlockNumber = GetNextPosition();
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
  if ( len32 == corsika::Thinned::kBytesPerBlock || len64 == corsika::Thinned::kBytesPerBlock)
    thinned = true;
  else if ( len32 == corsika::NotThinned::kBytesPerBlock || len64 == corsika::NotThinned::kBytesPerBlock)
    thinned = false;
  else {
    invalid = true;
  }
  if (len64 != corsika::Thinned::kBytesPerBlock && len64 != corsika::NotThinned::kBytesPerBlock)
    size_32 = true;
}

boost::shared_ptr<VRawStream> RawStreamFactory::Create(const std::string& theName)
{
    boost::shared_ptr<std::ifstream> file(new std::ifstream(theName.c_str()));
    if (!(*file)) throw CorsikaIOException("Error opening Corsika file '" + theName + "'.\n");
    
    Compression c = eNone;
    if (boost::algorithm::ends_with(theName, ".bz2")) c = eBZip2;
    else if (boost::algorithm::ends_with(theName, ".gz")) c = eGZip;
    
    
    std::istream* input = file.get();
    bool randomAccess = c==eNone;
    FormatSpec spec(*input);
    boost::shared_ptr<std::istream> f = GetFilter(*input, c);
    if (f)
    {
        spec = FormatSpec(*f);
        input = f.get();
        c = eNone;
    }
    boost::shared_ptr<VRawStream> ret;
    if (spec.thinned)
    {
        if (spec.size_32) ret.reset( new RawStream<Thinned, 1>(*input, file, theName, randomAccess));
        else ret.reset( new RawStream<Thinned, 2>(*input, file, theName, randomAccess));
    }
    else {
        if (spec.size_32) ret.reset( new RawStream<NotThinned, 1>(*input, file, theName, randomAccess));
        else ret.reset( new RawStream<NotThinned, 2>(*input, file, theName, randomAccess));
    }
    if (f) ret->Hold(f);
    
    
    if (!ret) {
        ostringstream msg;
        msg << "failed to initialize CORSIKA stream from " << theName;
        throw CorsikaIOException(msg.str());
    }
    return ret;
}





namespace corsika {

    boost::shared_ptr<std::istream> GetFilter(std::istream& in, Compression c)
    {
      boost::shared_ptr<std::istream> f;
      if (c != corsika::eNone) {
        boost::iostreams::filtering_istream* filter = new boost::iostreams::filtering_istream();
        if (c == corsika::eBZip2) {
          //cout << "using bzip2 decompression 2" << endl;
          filter->set_auto_close(false);
          filter->push(boost::iostreams::bzip2_decompressor());
        }
        else if (c == corsika::eGZip) {
          //cout << "using gzip decompression 2" << endl;
          filter->push(boost::iostreams::gzip_decompressor());
        }
        filter->push(in);
        f.reset(filter);
      }
      return f;
    }


    template <class Thinning, int Padding>
    FileIndex Scan(corsika::RawStream<Thinning, Padding>& stream, bool force)
    {
      FileIndex index;

      if (!force && !stream.IsSeekable())
        return index;

      size_t startingBlockNumber = stream.GetNextPosition();
      if (stream.IsSeekable())
        stream.SeekTo(0);

      int eventsSoFar = 0;

      unsigned int blockIndex = 0;
      bool foundEventHeader = false;
      bool foundRunHeader = false;
      bool foundLongBlock = false;

      Block<Thinning> blockUnth;
      while (stream.GetNextBlock(blockUnth) &&
             !blockUnth.IsRunTrailer()) {
        ++blockIndex;
        if (blockUnth.IsEventHeader()) {
          foundEventHeader = true;
          foundLongBlock = false;
          size_t rawPosition = stream.GetNextPosition();
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
        if (blockIndex >400 && !foundRunHeader)
            throw corsika::CorsikaIOException("Error scanning Corsika ground file: could not find run header");
        
          if (blockIndex >400 && !foundEventHeader) break;
      }

      if (!blockUnth.IsRunTrailer()) {
        ostringstream msg;
        msg << "Error scanning Corsika ground file: could not find run end.";
        if (foundEventHeader)
          msg << endl << "Found " << eventsSoFar << " event headers.";
    
        throw corsika::CorsikaIOException(msg.str());
      }

      if (index.eventHeaders.size() != index.eventTrailers.size())
        throw corsika::CorsikaIOException("Found different number of event-headers and -trailers");
    
      if (index.longBlocks.size() > 0 && index.eventHeaders.size() != index.longBlocks.size()) {
        ostringstream msg;
        msg << "Found different number of event-headers and longitudinal blocks ("
            << index.eventHeaders.size() << " != " << index.longBlocks.size() << ")";
        throw corsika::CorsikaIOException(msg.str());
      }

      if (stream.IsSeekable())
        stream.SeekTo(startingBlockNumber);
      return index;
    }


    template FileIndex Scan<Thinned, 1>(corsika::RawStream<Thinned, 1>& stream, bool force);
    template FileIndex Scan<NotThinned, 1>(corsika::RawStream<NotThinned, 1>& stream, bool force);
    template FileIndex Scan<Thinned, 2>(corsika::RawStream<Thinned, 2>& stream, bool force);
    template FileIndex Scan<NotThinned, 2>(corsika::RawStream<NotThinned, 2>& stream, bool force);

}

namespace corsika
{
    template class RawStream<Thinned, 1>;
    template class RawStream<NotThinned, 1>;
    
    template class RawStream<Thinned, 2>;
    template class RawStream<NotThinned, 2>;
}



// Configure (x)emacs for this file ...
// Local Variables:
// mode:c++
// compile-command: "make -C .. -k"
// End:
