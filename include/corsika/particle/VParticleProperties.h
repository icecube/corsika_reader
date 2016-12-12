/**
   \file
   Particle property interface

   \author Lukas Nellen
   \version $Id$
   \date 03 Apr 2004
*/

#pragma once
#include <string>

namespace corsika
{

  /**
     \class VParticleProperties VParticleProperties.h "corsika/VParticleProperties.h"

     \brief Internal interface for particle properties. This is
     intended to be implemented for elementary particles and nuclei.

     \note This is an internal interface and not available for user code.

     \author Lukas Nellen
     \date 03 Apr 2004
     \ingroup particles
  */

  class VParticleProperties {

  public:
    virtual ~VParticleProperties() { }

    /// Get particle type (using PDG particle codes)
    virtual int GetType() const = 0;

    /// Get particle name
    virtual std::string GetName() const = 0;

    /// Get particle mass (in Auger units)
    virtual double GetMass() const = 0;

  };


  typedef std::shared_ptr<const VParticleProperties> ParticlePropertiesPtr;


}
