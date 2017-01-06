/**
   \file
   A simple list of particle properties

   \author Javier Gonzalez
   \version $Id$
   \date 04 Jan 2015
*/

#pragma once
#include <corsika/particle/VParticleProperties.h>
#include <corsika/particle/ParticleProperties.h>
#include <corsika/particle/NucleusProperties.h>
#include <map>

namespace corsika
{

  /**
     \class ParticleList ParticleList.h

     \brief Class to hold properties of particles.

     \author Javier Gonzalez
     \version $Id$
     \date 04 Jan 2015
  */

  class ParticleList {
  public:

    const VParticleProperties& operator()(int code) const
    { return ParticleList::Get(code); }

    static const VParticleProperties& Get(int code);

    /// Convert Corsika particle code to PDG
    static int CorsikaToPDG(int theCorsikaCode);

    static std::string NameFromPDG(int pdgCode);
    static std::string NameFromCorsika(int corsikaCode);

  private:
    static void InitCorsikaToPDGMap();

    static void SetList();

    static std::map<int, int> corsikaToPDGMap_;
    static std::map<int, ParticleProperties> particles_;
    static std::map<int, NucleusProperties> nuclei_;
  };

}
