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


namespace corsika
{
    
    
    
    
    template <class Thinning, int Padding> void RawStream<Thinning, Padding>::Close()
    {
        file.reset();
    }
    
    template <class Thinning, int Padding> bool RawStream<Thinning, Padding>::GetNextBlockImpl(Block<Thinning>& theBlock)
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
    
    
    template <class Thinning, int Padding>
    void RawStream<Thinning, Padding>::SeekTo(size_t thePosition)
    {
        //cout << "SeekTo" << endl;
        size_t newBlockNumber = thePosition / kBlocksInDiskBlock;
        size_t newIndexInBlock = thePosition % kBlocksInDiskBlock;
        //if (newBlockNumber == fCurrentBlockNumber && newIndexInBlock == fIndexInDiskBlock)
        //  return;
        
        //cout << "will SeekTo " << newBlockNumber << ", " << newIndexInBlock << ", " << thePosition << endl;
        
        if (file->seekable) {
            if (fCurrentBlockNumber != newBlockNumber) {
                fCurrentBlockNumber = newBlockNumber;
            }
            fBlockBufferValid   = false;
            fIndexInDiskBlock   = newIndexInBlock;
            
            const size_t size = sizeof(DiskBlock);
            
            file->seek(fCurrentBlockNumber * size);
        }
        else
        {
            size_t current = GetNextPosition();
            if (current > thePosition) {
                Close();
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
        
        
        std::string String()
        {
            std::ostringstream str;
            str << (thinned?"thinned, ":"not thinned, ")
            << (int(!size_32)+1)*32 << " bit,"
            << (invalid?"invalid, ":"valid, ");
            return str.str();
        }
    };
    
    boost::shared_ptr<VRawStream> RawStreamFactory::Create(const std::string& theName)
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
    template class RawStream<Thinned, 1>;
    template class RawStream<NotThinned, 1>;
    template class RawStream<Thinned, 2>;
    template class RawStream<NotThinned, 2>;
}
