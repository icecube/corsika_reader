/**
   \file
   Raw disk file as specified by the CORSIKA manual.

   \author Lukas Nellen
   \version $Id$
   \date 08 Dec 2003
*/

#pragma once

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <corsika/CorsikaBlock.h>
#include <vector>
#include <map>
#include <corsika/CorsikaIOException.h>
#include <corsika/RawParticleIterator.h>
#include <corsika/FileStream.h>

namespace corsika
{
    struct VRawStream: public boost::enable_shared_from_this<VRawStream>
    {
      /// Read one block and advance
      virtual bool GetNextBlock(Block<Thinned>& theBlock) = 0;
      virtual bool GetNextBlock(Block<NotThinned>& theBlock) = 0;

      /// Number of the block read by the next call to GetNextBlock
      virtual size_t GetNextPosition() const = 0;

      /// Seek to a given block, the next block will be \a thePosition
      virtual void SeekTo(size_t thePosition) = 0;
      virtual bool IsSeekable() const = 0;

      virtual bool IsValid() = 0;
      virtual bool IsThinned() const = 0;

      virtual boost::shared_ptr<VRawParticleIterator> GetVParticleIt(size_t start=0) const = 0;
      virtual void Close() = 0;
    };

    struct RawStreamFactory
    {
        static boost::shared_ptr<VRawStream> Create(const std::string& theName);
    };

    template<class Stream, class BlockThinning>
    struct NextBlockDispatch {
      static bool GetNextBlock(Stream& stream, Block<BlockThinning>& theBlock)
      {
        return false;
      }
    };
    template<class Stream>
    struct NextBlockDispatch<Stream, typename Stream::ThinningType> {
      static bool GetNextBlock(Stream& stream, Block<typename Stream::ThinningType>& theBlock)
      {
        return stream.GetNextBlockImpl(theBlock);
      }
    };

    /**
     \class RawStream RawCorsikaFile.h "Corsika/RawCorsikaFile.h"

     \brief Raw disk file.

     This class provides block-wise read access to a Corsika ground
     particles file on disk. Simple random access is supported.

     This class handles the grouping of individual blocks into a disk
     block with padding. It doesn't provide tools for unpacking the
     individual particles from a block.

     \author Lukas Nellen
     \author Javier Gonzalez
     \date 08 Dec 2003
     \ingroup corsika
    */

    template <class Thinning, int Padding=1>
    class RawStream: public VRawStream {
    public:
      typedef Thinning ThinningType;

      static const unsigned int kBlocksInDiskBlock = Thinning::kSubBlocksPerBlock;

      /// Padding bytes at the beginning of a raw block
      static const unsigned int kPaddingBeginning  = Padding;
      /// Padding bytes at the end of a raw block
      static const unsigned int kPaddingEnd        = Padding;


      class DiskBlock {
      public:
        /// initial padding - works also for size 0
        int fPaddingBeginning[kPaddingBeginning];
        Block<Thinning>  fBlock[kBlocksInDiskBlock];
        /// final padding - works also for size 0
        int fPaddingEnd[kPaddingEnd];

      };


      //RawStream(std::istream& in, bool randomAccess=true);
        RawStream(boost::shared_ptr<FileStream> file, std::string filename): file(file), filename(filename),
        fCurrentBlockNumber(0),
        fDiskBlockBuffer(),
        fIndexInDiskBlock(0),
        fBlockBufferValid(false)
        {}

      ~RawStream()
      { Close(); }

      /// Read one block and advance
      bool GetNextBlock(Block<Thinned>& theBlock)
      { return NextBlockDispatch<RawStream<Thinning, Padding>, Thinned>::GetNextBlock(*this, theBlock); }
      bool GetNextBlock(Block<NotThinned>& theBlock)
      { return NextBlockDispatch<RawStream<Thinning, Padding>, NotThinned>::GetNextBlock(*this, theBlock); }
      bool GetNextBlockImpl(Block<Thinning>& theBlock);

      /// Number of the block read by the next call to GetNextBlock
      size_t GetNextPosition() const
      { return fIndexInDiskBlock + kBlocksInDiskBlock * fCurrentBlockNumber; }

      bool IsSeekable() const { return true; }

      /// Seek to a given block, the next block will be \a thePosition
      void SeekTo(size_t thePosition);

      bool IsValid();

      bool IsThinned() const
      { return Thinning::kBytesPerBlock == Thinned::kBytesPerBlock; }

      boost::shared_ptr<VRawParticleIterator> GetVParticleIt(size_t start=0) const
      { return boost::dynamic_pointer_cast<VRawParticleIterator>(GetParticleIt(start)); }

      boost::shared_ptr<RawParticleIterator<Thinning> > GetParticleIt(size_t start=0) const
      {
          if (start == 0) // if there is something we KNOW, it is that particles are not in block zero.
              start = GetNextPosition();
        
          VRawStream* stream = const_cast<RawStream<Thinning, Padding>*>(this);
          return boost::shared_ptr<RawParticleIterator<Thinning> >(new RawParticleIterator<Thinning>(stream->shared_from_this(), start));
      }

      const DiskBlock& DiskBlockBuffer() const
      { return fDiskBlockBuffer; }


      void Close();

      

        
    private:
        RawStream(const RawStream& other);
        RawStream& operator=(const RawStream& other);
     
      /// Read the block at the current position from disk
      bool ReadDiskBlock();
        
        boost::shared_ptr<FileStream> file;
        std::string filename;

        size_t  fCurrentBlockNumber;
        size_t  fIndexInDiskBlock;
        bool          fBlockBufferValid;

      DiskBlock fDiskBlockBuffer;

     

    };
}
