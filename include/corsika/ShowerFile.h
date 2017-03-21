/**
 \file
 Reader for Corsika generated shower file
 
 \author Troy Porter, Lukas Nellen, Javier Gonzalez
 \version $Id$
 \date 07 Jul 2003, 29 Jan 2004
 */

#pragma once
#include <corsika/RawStream.h>
#include <corsika/IOException.h>
#include <corsika/Shower.h>
#include <corsika/LongFile.h>
#include <corsika/FileIndex.h>
#include <string>
#include <map>
#include <vector>

namespace corsika
{
    
    enum Status
    {
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
    struct CorsikaShowerFile
    {   
        CorsikaShowerFile();
        CorsikaShowerFile(const std::string& theFileName, bool requireParticleFile = true);
        
        virtual ~CorsikaShowerFile() {}
        
        virtual void Open(const std::string& theFileName, bool scan = true);
        
        virtual void Close();
        
        virtual Status FindEvent(unsigned int eventId);
        
        virtual size_t GetNEvents();
        
        
        bool IsOpen()
        { return bool(fRawStream); }
        
        static bool IsValid(const std::string& theFileName);
        
        const CorsikaShower& GetCurrentShower() const {return fCurrentShower;}
        CorsikaShower& GetCurrentShower() {return fCurrentShower;}
        
        const corsika::RunHeader& GetRunHeader() const { return fRunHeader; }
        
        const bool IsThinned() const { return fIsThinned; }
        
    private:
        typedef std::vector<size_t> PositionVector;
        typedef std::map<unsigned int, unsigned int> IdToPositionMap;
        
        template <class Thinning>
        Status ReadRunHeader();
        template <class Thinning>
        Status Read();
        Status ReadLongFile();
        template <class Thinning>
        Status ReadLongBlocks();
        
        CorsikaShower fCurrentShower;
        corsika::RunHeader fRunHeader;
        
        RawStreamPtr fRawStream;
        std::string fLongFile;
        FileIndex fIndex;
        unsigned int fCurrentPosition;
        unsigned int fObservationLevel;
        bool fIsThinned;
        bool fFileScanned;
        
        boost::shared_ptr<CorsikaLongFile> fCorsikaLongFile;
    };
}
