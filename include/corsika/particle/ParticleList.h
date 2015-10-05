/**
   \file
   A simple list of particle properties

   \author Javier Gonzalez
   \version $Id$
   \date 04 Jan 2015
*/

#ifndef _corsika_ParticleList_h_
#define _corsika_ParticleList_h_

static const char CVSId_corsika_ParticleList[] =
  "$Id$";

#include <corsika/particle/VParticleProperties.h>
#include <corsika/particle/ParticleProperties.h>
#include <corsika/particle/NucleusProperties.h>
#include <map>
#include <boost/shared_ptr.hpp>


namespace corsika {

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
    static void InsertCorsikaToPDG(int theCorsikaCode, int thePDGCode);
    static void InitCorsikaToPDGMap();

    static void SetList();

    static std::map<int, int> corsikaToPDGMap_;
    static std::map<int, corsika::ParticleProperties> particles_;
    static std::map<int, corsika::NucleusProperties> nuclei_;
  };

} // corsika


#endif // _corsika_ParticleList_h_

// Configure (x)emacs for this file ...
// Local Variables:
// mode: c++
// compile-command: "make -C .. -k"
// End:
