/**
   \file
   Properties of nuclei

   \author Lukas Nellen
   \version $Id$
   \date 01 Jun 2005
*/

#pragma once
#include <corsika/particle/VParticleProperties.h>

namespace corsika
{

  /**
     \class NucleusProperties NucleusProperties.h "corsika/NucleusProperties.h"

     \brief Class to hold properties of nuclei.

     Nuclei are coded using an extension of the PDG particle codes for
     Monte Carlo. The scheme uses 7 digit codes. Therefore, we code
     nuclei with a type code 1000ZZZAAA.

     \author Lukas Nellen
     \date 01 Jun 2005
     \ingroup particles
  */

  class NucleusProperties : public VParticleProperties {

  public:
    static const int kNucleusBase        = 1000000000;
    static const int kNucleusMax         = 1000100300;
    static const int kAtomicNumberFactor = 1;
    static const int kAtomicNumberMask   = 1000;
    static const int kChargeFactor       = 1000;
    static const int kChargeMask         = 1000000;

    NucleusProperties(const int theType);

    /// Get particle type (using PDG particle codes)
    virtual int GetType() const;

    /// Get particle name
    virtual std::string GetName() const;

    /// Get atomic mass number A of particle
    virtual double GetAtomicNumber() const;

    /// Get particle mass (in Auger units)
    virtual double GetMass() const;

    /// Calculate the particle type code from Z and A.
    static int TypeCode(const unsigned int theCharge, const unsigned int theAtomicNumber);

    /// Check if type code is a valid nucleus
    static bool IsNucleus(const int theType);

  private:
    int fType;

  };

}
