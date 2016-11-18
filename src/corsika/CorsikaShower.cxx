/**
   \file


   \author Javier Gonzalez
   \version $Id: TEMPLATE.cc.tpl,v 1.4 2003/09/25 14:38:19 lukas Exp $
   \date 04 May 2011
*/

static const char CVSId[] =
"$Id$";


#include <corsika/CorsikaShower.h>
#include <corsika/particle/ParticleList.h>
#include <corsika/CorsikaBlock.h>
#include <iostream>

using namespace corsika;
using namespace std;


CorsikaShower::CorsikaShower(const Block<NotThinned>::EventHeader& header,
                             const Block<NotThinned>::EventTrailer& trailer,
                             CorsikaShowerFileParticleIterator* particleIt):
  fEventHeader(header),
  fEventTrailer(trailer),
  fPrimaryParticle(ParticleList::CorsikaToPDG(int(header.fParticleId))),
  fEnergy(header.fEnergy),
  fMuonNumber(trailer.fMuons),
  fZenith(header.fTheta),
  fAzimuth(header.fPhi),
  fMinRadiusCut(header.fRMaxThinning),
  fShowerNumber(int(header.fEventNumber)),
  //fShowerRunId(int(fRunNumber)),
  fEMEnergyCutoff(header.fCutoffElectrons),
  fMuonEnergyCutoff(header.fCutoffMuons),
  fParticleIterator(particleIt),
      fParticles(fParticleIterator)
{}





// Configure (x)emacs for this file ...
// Local Variables:
// mode:c++
// compile-command: "make -C .. -k"
// End:
