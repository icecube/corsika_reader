/**
   \file
   Raw disk file as specified by the CORSIKA manual.

   \author Lukas Nellen
   \version $Id$
   \date 08 Dec 2003
*/

#pragma once
#include <boost/shared_ptr.hpp>
#include <corsika/Block.h>
#include <corsika/IOException.h>

namespace corsika
{
    typedef boost::shared_ptr<struct RawStream> RawStreamPtr;
    
    /**
     \class RawStream RawStream.h "corsika/RawStream.h"
     
     \brief Raw disk file.
     
     This class provides block-wise read access to a Corsika ground
     particles file on disk. Simple random access is supported.
     
     This class handles the grouping of individual blocks into a disk
     block with padding. It doesn't provide tools for unpacking the
     individual particles from a block.
     
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
        /// It is possible to call SeekTo
        virtual bool IsSeekable() const = 0;
        
        /// This is a stream of a thinned corsika file
        virtual bool IsThinned() const = 0;
        
        static RawStreamPtr Create(const std::string& theName);
    };
}
