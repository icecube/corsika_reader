/**
 \file
 Implement raw CORSIKA file
 
 \author Lukas Nellen
 \version $Id$
 \date 08 Dec 2003
 */
#include <string>
#include <sstream>
#include <corsika/RawStream.h>
#include <corsika/FileStream.h>

namespace corsika
{
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
     \template RawStream
     
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
    struct RawStream: public VRawStream
    {
        typedef Thinning ThinningType;
        static const unsigned int kBlocksInDiskBlock = Thinning::kSubBlocksPerBlock;
        
        struct DiskBlock
        {
            int padding_start[Padding];
            Block<Thinning>  fBlock[kBlocksInDiskBlock];
            int padding_end[Padding];
            
            bool padding_ok()
            {
                for (int i = 0; i < Padding; i++)
                    if (padding_start[i] != padding_end[i])
                        return false;
                return true;
            }
        };
        
        boost::shared_ptr<FileStream> file;
        std::string filename;
        size_t current_block;
        size_t current_disk_block;
        bool buffer_valid;
        bool valid;
        DiskBlock buffer;
        
        RawStream(boost::shared_ptr<FileStream> file, std::string filename, int64_t len64): file(file), filename(filename), current_block(0), current_disk_block(0)
        {
            *reinterpret_cast<int64_t*>(&buffer) = len64; // Copy value over
            buffer_valid = file->read(sizeof(DiskBlock) - 8, (char*)&buffer + 8) > 0;
            valid = buffer_valid && buffer.fBlock[0].IsRunHeader() && buffer.padding_ok();
        }
        
        /// Read one block and advance
        bool GetNextBlock(Block<Thinned>& theBlock)
        {
            return NextBlockDispatch<RawStream<Thinning, Padding>, Thinned>::GetNextBlock(*this, theBlock);
        }
        bool GetNextBlock(Block<NotThinned>& theBlock)
        {
            return NextBlockDispatch<RawStream<Thinning, Padding>, NotThinned>::GetNextBlock(*this, theBlock);
        }
        
        bool GetNextBlockImpl(Block<Thinning>& theBlock)
        {
            if (!buffer_valid)
            {
                if (!ReadDiskBlock())
                    return false;
            }
            
            theBlock = buffer.fBlock[current_disk_block];
            if (++current_disk_block >= kBlocksInDiskBlock)
            {
                current_block++;
                current_disk_block = 0;
                buffer_valid = false;
            }
            return true;
        }
        
        /// Number of the block read by the next call to GetNextBlock
        size_t GetNextPosition() const
        {
            return current_disk_block + kBlocksInDiskBlock * current_block;
        }
        
        bool IsSeekable() const { return true; }
        
        /// Seek to a given block, the next block will be \a thePosition
        void SeekTo(size_t thePosition)
        {
            size_t newBlockNumber = thePosition / kBlocksInDiskBlock;
            size_t newIndexInBlock = thePosition % kBlocksInDiskBlock;
            //if (newBlockNumber == fCurrentBlockNumber && newIndexInBlock == fIndexInDiskBlock) return
            if (file->seekable)
            {
                current_block = newBlockNumber;
                buffer_valid   = false;
                current_disk_block   = newIndexInBlock;
                file->seek(current_block * sizeof(DiskBlock));
            }
            else
            {
                size_t current = GetNextPosition();
                if (current > thePosition) {
                    file.reset(FileStream::open(filename.c_str()));
                    if (!file) throw CorsikaIOException("Failed in dumb seek");
                    current_block = 0;
                    current_disk_block = 0;
                    buffer_valid = false;
                    
                    current = GetNextPosition();
                }
                Block<Thinning> block;
                while (thePosition > 0 && thePosition > current)
                {
                    GetNextBlock(block);
                    current = GetNextPosition();
                }
                current = GetNextPosition();
            }
        }
        bool IsValid()
        {
            return valid;
        }
        bool IsThinned() const
        {
            return Thinning::kBytesPerBlock == Thinned::kBytesPerBlock;
        }
        bool ReadDiskBlock()
        {
            if (file->read(sizeof(DiskBlock), &buffer) <= 0) return false;
            if (!buffer.padding_ok()) throw CorsikaIOException("Padding mismatch\n");
            buffer_valid = true;
            return true;
        }
    };
    
    RawStreamPtr VRawStream::Create(const std::string& theName)
    {
        boost::shared_ptr<FileStream> file(FileStream::open(theName.c_str()));
        if (!file) throw CorsikaIOException("Error opening Corsika file '" + theName + "'.\n");
        
        int64_t len64;
        file->read(8, &len64);
        int32_t len32 = *reinterpret_cast<int32_t*>(&len64);

        if (len64 == corsika::Thinned::kBytesPerBlock)
            return RawStreamPtr(new RawStream<Thinned, 2>(file, theName, len64)); // 64bit thinned
        else if (len64 == corsika::NotThinned::kBytesPerBlock)
            return RawStreamPtr( new RawStream<NotThinned, 2>(file, theName, len64)); // 64bit not-thinned
        else if (len32 == corsika::Thinned::kBytesPerBlock)
            return RawStreamPtr(new RawStream<Thinned, 1>(file, theName, len64)); // 32bit thinned
        else if (len32 == corsika::NotThinned::kBytesPerBlock)
            return RawStreamPtr(new RawStream<NotThinned, 1>(file, theName, len64)); // 32bit not-thinned
        
        throw CorsikaIOException("Can't determine type of corsika file\n");
    }
}
