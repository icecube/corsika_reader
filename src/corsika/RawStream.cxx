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
    template <class Thinning, typename Padding> struct RawStreamT: RawStream
    {
        static const unsigned int kBlocksInDiskBlock = Thinning::kSubBlocksPerBlock;
        
        struct DiskBlock
        {
            Padding padding_start;
            Block<Thinning>  fBlock[kBlocksInDiskBlock];
            Padding padding_end;
        } __attribute__((packed));
        
        boost::shared_ptr<FileStream> file;
        std::string filename;
        size_t current_block;
        size_t current_disk_block;
        bool buffer_valid;
        bool valid;
        DiskBlock buffer;
        
        RawStreamT(boost::shared_ptr<FileStream> file, std::string filename, int64_t len64): file(file), filename(filename), current_block(0), current_disk_block(0)
        {
            *reinterpret_cast<int64_t*>(&buffer) = len64; // Copy value over
            buffer_valid = file->read(sizeof(DiskBlock) - 8, (char*)&buffer + 8) > 0;
            valid = buffer_valid && buffer.fBlock[0].IsRunHeader() && (buffer.padding_start == buffer.padding_end);
        }
        
        template<typename T> bool read_block(T& block)
        {
            return false;
        }
        bool read_block(Block<Thinning>& block)
        {
            if (!buffer_valid)
            {
                if (!ReadDiskBlock())
                    return false;
            }
            
            block = buffer.fBlock[current_disk_block];
            if (++current_disk_block >= kBlocksInDiskBlock)
            {
                current_block++;
                current_disk_block = 0;
                buffer_valid = false;
            }
            return true;
        }
        bool GetNextBlock(Block<Thinned>& theBlock)
        {
            return read_block(theBlock);
        }
        bool GetNextBlock(Block<NotThinned>& theBlock)
        {
            return read_block(theBlock);
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
                if (GetNextPosition() > thePosition)
                {
                    file.reset(FileStream::open(filename.c_str()));
                    if (!file) throw CorsikaIOException("Failed in dumb seek");
                    current_block = 0;
                    current_disk_block = 0;
                    buffer_valid = false;
                }
                Block<Thinning> block;
                while (thePosition > 0 && thePosition > GetNextPosition())
                    GetNextBlock(block);
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
            if (buffer.padding_start != buffer.padding_end) throw CorsikaIOException("Padding mismatch\n");
            buffer_valid = true;
            return true;
        }
    };
    
    RawStreamPtr RawStream::Create(const std::string& theName)
    {
        boost::shared_ptr<FileStream> file(FileStream::open(theName.c_str()));
        if (!file) throw CorsikaIOException("Error opening Corsika file '" + theName + "'.\n");
        
        int64_t len64;
        file->read(8, &len64);
        int32_t len32 = *reinterpret_cast<int32_t*>(&len64);

        if (len64 == corsika::Thinned::kBytesPerBlock)
            return RawStreamPtr(new RawStreamT<Thinned, int64_t>(file, theName, len64)); // 64bit thinned
        else if (len64 == corsika::NotThinned::kBytesPerBlock)
            return RawStreamPtr( new RawStreamT<NotThinned, int64_t>(file, theName, len64)); // 64bit not-thinned
        else if (len32 == corsika::Thinned::kBytesPerBlock)
            return RawStreamPtr(new RawStreamT<Thinned, int32_t>(file, theName, len64)); // 32bit thinned
        else if (len32 == corsika::NotThinned::kBytesPerBlock)
            return RawStreamPtr(new RawStreamT<NotThinned, int32_t>(file, theName, len64)); // 32bit not-thinned
        
        throw CorsikaIOException("Can't determine type of corsika file\n");
    }
}
