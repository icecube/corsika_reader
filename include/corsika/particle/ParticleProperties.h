/**
   \file
   Properties of elementary particles

   \author Lukas Nellen
   \version $Id$
   \date 03 Apr 2004
*/

#ifndef _corsika_ParticleProperties_h_
#define _corsika_ParticleProperties_h_

static const char CVSId_corsika_ParticleProperties[] =
  "$Id$";

#include <corsika/particle/VParticleProperties.h>
#include <string>

namespace corsika {

  /**
     \class ParticleProperties ParticleProperties.h "corsika/ParticleProperties.h"

     \brief Provide particle properties that depend only on its type.

     \author Lukas Nellen
     \date 03 Apr 2004
  */

  class ParticleProperties : public VParticleProperties {

  public:
    ParticleProperties(const int theType,
                       const std::string& theName,
                       const double theMass);

    /// Get particle type (using PDG particle codes)
    virtual int GetType() const { return fType; }

    /// Get particle name
    virtual std::string GetName() const { return fName; }

    /// Get particle mass (in Auger units)
    virtual double GetMass() const { return fMass; }

  private:
    /// Particle type ID (PDG types)
    int fType;
    /// The particle name
    std::string fName;
    /// particle mass
    double fMass;

  };

} // corsika


#endif // _corsika_ParticleProperties_h_

// Configure (x)emacs for this file ...
// Local Variables:
// mode: c++
// compile-command: "make -C .. -k"
// End:
