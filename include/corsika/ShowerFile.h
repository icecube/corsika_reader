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
     \class ShowerFile ShowerFile.h "corsika/ShowerFile.h"
     
     \brief Read data from the output of CORSIKA
     
     \ingroup corsika
     */
    struct ShowerFile
    {   
        ShowerFile();
        ShowerFile(const std::string& theFileName, bool requireParticleFile = true);
        
        virtual ~ShowerFile() {}
        
        /// Open file
        virtual void Open(const std::string& theFileName, bool scan = true);
        
        /// Close file
        virtual void Close();
        
        /// Find an event and position to read it
        virtual Status FindEvent(unsigned int eventId);
        
        /// Get number of showers in file
        virtual size_t GetNEvents();
        
        
        /// File is open
        bool IsOpen()
        { return bool(fRawStream); }
        
        /// File is valid
        static bool IsValid(const std::string& theFileName);
        
        /// Get the shower in the current position
        const Shower& GetCurrentShower() const {return fCurrentShower;}
        Shower& GetCurrentShower() {return fCurrentShower;}
        
        /// Get the run header
        const corsika::RunHeader& GetRunHeader() const { return fRunHeader; }
        
        /// Shower is thinned
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
        
        Shower fCurrentShower;
        corsika::RunHeader fRunHeader;
        
        RawStreamPtr fRawStream;
        std::string fLongFile;
        FileIndex fIndex;
        unsigned int fCurrentPosition;
        unsigned int fObservationLevel;
        bool fIsThinned;
        bool fFileScanned;
        
        boost::shared_ptr<LongFile> fCorsikaLongFile;
    };
}
