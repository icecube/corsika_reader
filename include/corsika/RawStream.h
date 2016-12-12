/**
   \file
   Raw disk file as specified by the CORSIKA manual.

   \author Lukas Nellen
   \version $Id$
   \date 08 Dec 2003
*/

#pragma once
#include <corsika/CorsikaBlock.h>
#include <corsika/CorsikaIOException.h>

namespace corsika
{
    typedef std::shared_ptr<struct RawStream> RawStreamPtr;
    
    /**
     \struct VRawStream
     
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
    struct RawStream
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
        
        static RawStreamPtr Create(const std::string& theName);
    };
}
