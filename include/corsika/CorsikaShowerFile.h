/**
   \file
   Reader for Corsika generated shower file

   \author Troy Porter, Lukas Nellen, Javier Gonzalez
   \version $Id$
   \date 07 Jul 2003, 29 Jan 2004
*/

#pragma once
#include <corsika/RawStream.h>
#include <corsika/CorsikaIOException.h>
#include <corsika/CorsikaShower.h>
#include <corsika/CorsikaLongFile.h>
#include <corsika/FileIndex.h>
#include <string>
#include <map>
#include <vector>

namespace corsika
{

  enum Status {
    eSuccess,
    eFail,
    eEOF
  };

  /**
     \class CorsikaShowerFile CorsikaShowerFile.h "corsika/CorsikaShowerFile.h"

     \brief Read data from the output of CORSIKA

     \author Lukas Nellen
     \author Javier Gonzalez
     \date 29 Jan 2004
     \ingroup corsika
  */
  class CorsikaShowerFile {

  public:

    CorsikaShowerFile();
    CorsikaShowerFile(const std::string& theFileName, const bool requireParticleFile = true);

    virtual ~CorsikaShowerFile();

    virtual void Open(const std::string& theFileName, bool scan = true);

    virtual void Close();

    virtual Status FindEvent(unsigned int eventId);

    virtual size_t GetNEvents();

    bool IsValid();

    bool IsOpen()
    { return bool(fRawStream); }

    static bool IsValid(const std::string& theFileName);

    const CorsikaShower& GetCurrentShower() const {return fCurrentShower;}
    CorsikaShower& GetCurrentShower() {return fCurrentShower;}

  private:
    typedef std::vector<size_t> PositionVector;
    typedef std::map<unsigned int, unsigned int> IdToPositionMap;

    template <class Thinning>
    Status Read();
    Status ReadLongFile();
    template <class Thinning>
    Status ReadLongBlocks();

    CorsikaShower fCurrentShower;

    RawStreamPtr fRawStream;
    std::string fLongFile;
    FileIndex fIndex;
    unsigned int fCurrentPosition;
    unsigned int fObservationLevel;
    bool fIsThinned;
    bool fFileScanned;

    boost::shared_ptr<corsika::CorsikaLongFile> fCorsikaLongFile;
  };

}
