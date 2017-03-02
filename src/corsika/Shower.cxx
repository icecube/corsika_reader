/**
   \file


   \author Javier Gonzalez
   \version $Id: TEMPLATE.cc.tpl,v 1.4 2003/09/25 14:38:19 lukas Exp $
   \date 04 May 2011
*/

#include <corsika/Shower.h>
#include <corsika/particle/ParticleList.h>
#include <corsika/Block.h>
#include <iostream>

using namespace corsika;


CorsikaShower::CorsikaShower(const struct EventHeader& header,
                             const struct EventTrailer& trailer,
                             ShowerParticleStream* particle_stream):
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
  particle_stream(particle_stream)
{}
