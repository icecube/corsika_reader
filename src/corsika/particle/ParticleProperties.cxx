/**
   \file
   Properties of elementary particles

   \author Lukas Nellen
   \version $Id$
   \date 03 Apr 2004
*/

static const char CVSId[] =
  "$Id$";


#include <corsika/particle/ParticleProperties.h>

namespace corsika {


  ParticleProperties::ParticleProperties(const int theType,
                                         const std::string& theName,
                                         const double theMass) :
    fType(theType),
    fName(theName),
    fMass(theMass)
  {
  }


} // utl

// Configure (x)emacs for this file ...
// Local Variables:
// mode:c++
// compile-command: "make -C .. -k"
// End:
