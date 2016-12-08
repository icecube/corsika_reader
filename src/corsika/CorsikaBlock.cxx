/**
   \file
   On-disk block of raw CORSIKA files

   \author Lukas Nellen
   \version $Id$
   \date 19 Nov 2003
*/
#include <corsika/CorsikaBlock.h>
#include <sstream>

namespace corsika
{
    bool BlockID::operator==(const char* str) const
    {
        return strncmp(fID, str, 4) == 0;
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
}
