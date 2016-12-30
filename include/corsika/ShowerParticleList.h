/**
   \file
   Interface for particles lists in a shower file from simulation
   \author Lukas Nellen
   \author Javier Gonzalez
   \version $Id$
   \date 4-Apr-2003
*/

#ifndef _corsika_ShowerParticleList_h_
#define _corsika_ShowerParticleList_h_

static const char CVSId_corsika_ShowerParticleList[] =
"$Id$";

#include <corsika/CorsikaShowerFileParticleIterator.h>


namespace corsika {

  /**
     \class ShowerParticleList ShowerParticleList.h "corsika/ShowerParticleList.h"

     \brief Interface class for accessing a list of particles from a shower file
  */
  class ShowerParticleList
  {
  public:
    typedef CorsikaShowerFileParticleIterator iterator;

    ShowerParticleList(CorsikaShowerFileParticleIterator* theInterface = 0) :
      fFileInterface(theInterface)
    {
    }

    virtual ~ShowerParticleList() { }

    boost::optional<CorsikaParticle> NextParticle()
      {
          return fFileInterface->NextParticle();
      }
      
      /*
    CorsikaShowerFileParticleIterator
    begin() const
    {
      fFileInterface->Rewind();
      return ++(*fFileInterface);
    }

    CorsikaShowerFileParticleIterator
    end() const
    {
      return fInvalid;
    }*/

  private:
    CorsikaShowerFileParticleIterator* fFileInterface;
    CorsikaShowerFileParticleIterator fInvalid;
  };

}

#endif

// Configure (x)emacs for this file ...
// Local Variables:
// mode:c++
// compile-command: "cd ..; make -k"
// End:
