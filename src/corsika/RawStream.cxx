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
        
        /// Read one block and advance
        bool GetNextBlock(Block<Thinned>& theBlock)
        { return NextBlockDispatch<RawStream<Thinning, Padding>, Thinned>::GetNextBlock(*this, theBlock); }
        bool GetNextBlock(Block<NotThinned>& theBlock)
        { return NextBlockDispatch<RawStream<Thinning, Padding>, NotThinned>::GetNextBlock(*this, theBlock); }
        
        bool GetNextBlockImpl(Block<Thinning>& theBlock)
        {
            if (!fBlockBufferValid)
            {
                if (!ReadDiskBlock())
                    return false;
            }
            
            theBlock = fDiskBlockBuffer.fBlock[fIndexInDiskBlock];
            if (++fIndexInDiskBlock >= kBlocksInDiskBlock) {
                ++fCurrentBlockNumber;
                fIndexInDiskBlock = 0;
                fBlockBufferValid = false;
            }
            return true;
        }
        
        /// Number of the block read by the next call to GetNextBlock
        size_t GetNextPosition() const
        { return fIndexInDiskBlock + kBlocksInDiskBlock * fCurrentBlockNumber; }
        
        bool IsSeekable() const { return true; }
        
        /// Seek to a given block, the next block will be \a thePosition
        void SeekTo(size_t thePosition)
        {
            size_t newBlockNumber = thePosition / kBlocksInDiskBlock;
            size_t newIndexInBlock = thePosition % kBlocksInDiskBlock;
            //if (newBlockNumber == fCurrentBlockNumber && newIndexInBlock == fIndexInDiskBlock) return
            if (file->seekable)
            {
                if (fCurrentBlockNumber != newBlockNumber)
                    fCurrentBlockNumber = newBlockNumber;
                fBlockBufferValid   = false;
                fIndexInDiskBlock   = newIndexInBlock;
                file->seek(fCurrentBlockNumber * sizeof(DiskBlock));
            }
            else
            {
                size_t current = GetNextPosition();
                if (current > thePosition) {
                    file.reset(FileStream::open(filename.c_str()));
                    if (!file) throw CorsikaIOException("Failed in dumb seek");
                    fCurrentBlockNumber = 0;
                    fIndexInDiskBlock = 0;
                    fBlockBufferValid = false;
                    
                    
                    
                    current = GetNextPosition();
                }
                Block<Thinning> block;
                while (thePosition > 0 && thePosition > current) {
                    GetNextBlock(block);
                    current = GetNextPosition();
                }
                current = GetNextPosition();
            }
        }
        
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
    
        
        bool ReadDiskBlock()
        {
            const unsigned int size = sizeof(DiskBlock);
            long rc = file->read(size, &fDiskBlockBuffer);
            if (rc <= 0) {
                return false;
            }
            //cout << "  --> " << fDiskBlockBuffer.fBlock[0].ID() << " at " << fDiskStream->tellg() << endl;
            for (int i = 0; i != Padding; ++i) {
                if (fDiskBlockBuffer.fPaddingBeginning[i] != fDiskBlockBuffer.fPaddingEnd[i]) {
                    std::ostringstream msg;
                    msg << "Error reading block. Sizes do not match: " << i <<  ", "<< fDiskBlockBuffer.fPaddingBeginning[i] << " =? " << fDiskBlockBuffer.fPaddingEnd[i] << std::endl;
                    throw CorsikaIOException(msg.str());
                }
            }
            fBlockBufferValid = true;
            return true;
        }
        
        boost::shared_ptr<FileStream> file;
        std::string filename;
        
        size_t  fCurrentBlockNumber;
        size_t  fIndexInDiskBlock;
        bool          fBlockBufferValid;
        
        DiskBlock fDiskBlockBuffer;
    };
    
    template <class Thinning, int Padding>
    bool
    RawStream<Thinning, Padding>::IsValid()
    {
        //cout << "IsValid" << endl;
        if (!file->seekable) {
            return true;
        }
        
        const size_t currentBlockNumber = GetNextPosition();
        const bool blockBufferValid = fBlockBufferValid;
        
        bool fail = false;
        Block<Thinning> block;
        SeekTo(0);
        std::ostringstream msg;
        if (!GetNextBlock(block)) {
            msg << "Failed getting next block" << std::endl;
            fail = true;
        }
        if (!block.IsRunHeader()) {
            msg << "First block is not run header" << std::endl;
            fail = true;
        }
        
        if (fDiskBlockBuffer.fPaddingBeginning[0] !=
            Thinning::kSubBlocksPerBlock*Thinning::kParticlesInBlock*sizeof(typename Block<Thinning>::ParticleData)) {
            msg << "Unexpected block size. "
            << fDiskBlockBuffer.fPaddingBeginning[0] << " != "
            << Thinning::kSubBlocksPerBlock*Thinning::kParticlesInBlock*sizeof(typename Block<Thinning>::ParticleData) << std::endl;
            fail = true;
        }
        else {
            msg << "So far so good. "
            << fDiskBlockBuffer.fPaddingBeginning[0] << " != "
            << Thinning::kSubBlocksPerBlock*Thinning::kParticlesInBlock*sizeof(typename Block<Thinning>::ParticleData) << std::endl;
        }
        if (fDiskBlockBuffer.fPaddingBeginning[0] != fDiskBlockBuffer.fPaddingEnd[0]) {
            msg << "Block begin and end do not match. "
            << fDiskBlockBuffer.fPaddingBeginning[0] << " != " << fDiskBlockBuffer.fPaddingEnd[0] << std::endl;
            fail = true;
        }
        else {
            msg << "So far so good. "
            << fDiskBlockBuffer.fPaddingBeginning[0] << " != " << fDiskBlockBuffer.fPaddingEnd[0] << std::endl;
        }
        
        // leave things as they were
        fBlockBufferValid = blockBufferValid;
        SeekTo(currentBlockNumber);
        
        return !fail;
    }
    
    
    struct FormatSpec {
        bool thinned;
        bool size_32;
        bool invalid;
        
        FormatSpec(FileStream& in): thinned(false), size_32(false), invalid(false)
        {
            char buf[8];
            in.read(8, buf);
            const long long len64 = *reinterpret_cast<long long*>(buf);
            const int len32 = *reinterpret_cast<int*>(buf);
            
            if ( len32 == corsika::Thinned::kBytesPerBlock || len64 == corsika::Thinned::kBytesPerBlock)
            thinned = true;
            else if ( len32 == corsika::NotThinned::kBytesPerBlock || len64 == corsika::NotThinned::kBytesPerBlock)
            thinned = false;
            else
            invalid = true;
            
            if (len64 != corsika::Thinned::kBytesPerBlock && len64 != corsika::NotThinned::kBytesPerBlock)
            size_32 = true;
        }
    };
    
    boost::shared_ptr<VRawStream> VRawStream::Create(const std::string& theName)
    {
        
        FileStream* file0 = FileStream::open(theName.c_str());
        if (!file0) throw CorsikaIOException("Error opening Corsika file '" + theName + "'.\n");
        FormatSpec spec(*file0);
        delete file0;
        
        
        boost::shared_ptr<FileStream> file(FileStream::open(theName.c_str()));
        
        boost::shared_ptr<VRawStream> ret;
        if (spec.thinned)
        {
            if (spec.size_32) ret.reset( new RawStream<Thinned, 1>(file, theName));
            else ret.reset( new RawStream<Thinned, 2>(file, theName));
        }
        else {
            if (spec.size_32) ret.reset( new RawStream<NotThinned, 1>(file, theName));
            else ret.reset( new RawStream<NotThinned, 2>(file, theName));
        }
        
        return ret;
    }
}
