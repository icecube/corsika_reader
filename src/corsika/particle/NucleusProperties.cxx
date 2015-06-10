/**
   \file
   Properties of nuclei

   \author Lukas Nellen
   \version $Id$
   \date 01 Jun 2005
*/

static const char CVSId[] =
  "$Id$";

#include <corsika/particle/NucleusProperties.h>
#include <corsika/particle/ParticleList.h>
#include <corsika/Constants.h>
#include <corsika/CorsikaParticle.h>
#include <boost/format.hpp>
using boost::format;
using boost::io::str;


namespace {

  const char* NuclearNames[] = {
    "neutron",
    "H",
    "He",
    "Li",
    "Be",
    "B",
    "C",
    "N",
    "O",
    "F",
    "Ne",
    "Na",
    "Mg",
    "Al",
    "Si",
    "P",
    "S",
    "Cl",
    "Ar",
    "K",
    "Ca",
    "Sc",
    "Ti",
    "V",
    "Cr",
    "Mn",
    "Fe"
  };

  unsigned int kMaxNuclearNameIndex = sizeof(NuclearNames) / sizeof(char*) - 1;

}


namespace corsika {

  NucleusProperties::NucleusProperties(const int theType) :
    fType(theType)
  {
  }


  int
  NucleusProperties::GetType()
    const
  {
    return fType;
  }


  std::string
  NucleusProperties::GetName()
    const
  {
    unsigned int Z = (fType / kChargeFactor) % kChargeMask;
    unsigned int A = (fType / kAtomicNumberFactor) % kAtomicNumberMask;

    if (Z <= kMaxNuclearNameIndex)
      return str(format("%1% %2%") % NuclearNames[Z] % A);
    else
      return str(format("N(%1%) %2%") % Z % A);
  }


  double
  NucleusProperties::GetAtomicNumber()
    const
  {
    return (fType / kAtomicNumberFactor) % kAtomicNumberMask;
  }


  /**
     A very simple calculation of the mass: \f$ m = m_p * A \f$.

     \todo Replace this with a more precise expression
  */

  double
  NucleusProperties::GetMass()
    const
  {
    static double protonMass = ParticleList::Get(CorsikaParticle::eProton).GetMass();
    return GetAtomicNumber() * protonMass;
  }


  int
  NucleusProperties::TypeCode(const unsigned int theCharge,
                              const unsigned int theAtomicNumber)
  {
    return
      kNucleusBase
      + kChargeFactor       * theCharge
      + kAtomicNumberFactor * theAtomicNumber;
  }


  /// Check if type code is a valid nucleus
  bool
  NucleusProperties::IsNucleus(const int theType)
  {
    return kNucleusBase < theType && theType < kNucleusMax;
  }

} // namespace corsika


// Configure (x)emacs for this file ...
// Local Variables:
// mode: c++
// compile-command: "make -C .. -k"
// End:
