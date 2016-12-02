/**
   \file
   On-disk block of raw CORSIKA files

   \author Lukas Nellen
   \version $Id$
   \date 19 Nov 2003
*/
#include <corsika/CorsikaBlock.h>
#include <corsika/CorsikaIOException.h>
#include <sstream>
#include <string.h>

namespace corsika
{
    void BlockID::SetID(const char* const theID)
    {
        if (strlen(theID) != kLength) throw CorsikaIOException("Invalid length for BlockID string");
        strncpy(fID, theID, kLength);
    }
    
    bool BlockID::Is(const char* const theID) const
    {
        return strncmp(fID, theID, kLength) == 0;
    }
    std::string ParticleData<Thinned>::String() const
    {
        std::ostringstream out;
        out << "Description: "  << fDescription << std::endl
        << "Momentum:    (" << fPx << ", " << fPy << ", " << fPz << ")" << std::endl
        << "Position:    (" << fX << ", " << fY << ")" << std::endl
        << "T or Z:       " << fTorZ << std::endl;
        return out.str();
    }
    std::string ParticleData<NotThinned>::String() const
    {
        std::ostringstream out;
        out << "Description: "  << fDescription << std::endl
        << "Momentum:    (" << fPx << ", " << fPy << ", " << fPz << ")" << std::endl
        << "Position:    (" << fX << ", " << fY << ")" << std::endl
        << "T or Z:       " << fTorZ << std::endl;
        return out.str();
    }
    
    template class Block<Thinned>;
    template class Block<NotThinned>;
}
