/**
   \file
   Raw disk file as specified by the CORSIKA manual.

   \author Lukas Nellen
   \version $Id$
   \date 08 Dec 2003
*/

#ifndef _corsika_Corsika_RawFile_h_
#define _corsika_Corsika_RawFile_h_

#include <corsika/CorsikaBlock.h>
#include <string>
#include <fstream>
#include <corsika/CorsikaIOException.h>
#include <boost/iostreams/filtering_stream.hpp>
#include <corsika/RawParticleIterator.h>
#include <corsika/RawStream.h>

namespace corsika
{

    class VRawFile {
    public:

      virtual ~VRawFile(){}

      virtual void Open(const std::string& theName) = 0;

      /// Close file (no-op for closed file).
      virtual void Close() = 0;

      /// Read one block and advance
      //bool GetNextBlock(Block<Thinning>& theBlock);

      /// Number of the block read by the next call to GetNextBlock
      virtual size_t GetNextPosition() const = 0;

      /// Seek to a given block, the next block will be \a thePosition
      virtual void SeekTo(size_t thePosition) = 0;

      /// Check if the file is open
      virtual bool IsOpen() const = 0;

      virtual bool IsValid() = 0;

      virtual bool IsThinned() const = 0;

      virtual boost::shared_ptr<VRawParticleIterator> GetVParticleIt(size_t start=0) const = 0;

      std::string   fName;
    };


    /**
     \class RawFile RawCorsikaFile.h "Corsika/RawCorsikaFile.h"

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

    class RawFile: public VRawFile {
    public:

      RawFile();

      /// Construct and open file
      RawFile(const std::string& theName);
      RawFile(std::istream& in, Compression c=eNone);
      RawFile(const RawFile& other);
      RawFile& operator=(const RawFile& other);


      void Open(const std::string& theName);
      void Open(std::istream& in, Compression c=eNone);

      /// Close file (no-op for closed file).
      void Close()
      { if(fCorsikaStream) fCorsikaStream->Close(); }

      /// Check if the file is open
      bool IsOpen() const
      { return true; }

      //------------------------------------------------
      // All the rest are delegated to the stream class
      //------------------------------------------------

      /// Read one block and advance
      template <class Thinning>
      bool GetNextBlock(Block<Thinning>& theBlock)
      { return fCorsikaStream->GetNextBlock(theBlock); }

      /// Number of the block read by the next call to GetNextBlock
      size_t GetNextPosition() const
      { return fCorsikaStream->GetNextPosition(); }

      /// Seek to a given block, the next block will be \a thePosition
      void SeekTo(size_t thePosition)
      { fCorsikaStream->SeekTo(thePosition); }
      virtual bool IsSeekable() const
      { return fCorsikaStream->IsSeekable(); }

      bool IsValid()
      { return fCorsikaStream && fCorsikaStream->IsValid(); }

      bool IsThinned() const
      { return fCorsikaStream->IsThinned(); }

      boost::shared_ptr<VRawParticleIterator> GetVParticleIt(size_t start=0) const
      { return fCorsikaStream->GetVParticleIt(start); }

      FileIndex Scan()
      { return fCorsikaStream->Scan(false); }

    private:

      boost::shared_ptr<VRawStream> fCorsikaStream;

      bool fFromFile;
    };

} // corsika


#endif // _corsika_Corsika_RawFile_h_

// Configure (x)emacs for this file ...
// Local Variables:
// mode:c++
// compile-command: "make -C .. -k"
// End:
