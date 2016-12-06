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
#include <corsika/CorsikaIOException.h>
#include <corsika/RawParticleIterator.h>

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
        static boost::shared_ptr<VRawStream> Create(const std::string& theName);
    };
    typedef boost::shared_ptr<VRawStream> RawStreamPtr;
}
