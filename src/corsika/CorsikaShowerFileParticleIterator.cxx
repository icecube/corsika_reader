/**
   \file
   Implementation file for CorsikaShowerFileParticleIterator class
   \author Troy Porter
   \version $Id$
   \date 22 May 2003
*/

#include <corsika/CorsikaShowerFileParticleIterator.h>
#include <corsika/CorsikaIOException.h>
#include <corsika/particle/ParticleList.h>
#include <corsika/CorsikaParticle.h>
#include <corsika/CorsikaUnits.h>
#include <corsika/Constants.h>
#include <corsika/RawStream.h>
#include <sstream>
#include <cmath>
using namespace corsika;


CorsikaShowerFileParticleIterator::
CorsikaShowerFileParticleIterator() :
  fTimeOffset(0),
  fObservationLevel(0),
  fIsThinned(false),
  fKeepMuProd(false)
{ }

CorsikaShowerFileParticleIterator::
CorsikaShowerFileParticleIterator(const VRawStream& rawStream,
				  EventHeader event_header,
                                   unsigned long int start,
                                  const double timeOffset,
                                  const unsigned int observationLevel,
                                  const bool isThinned,
                                  const bool keepMuProd) :
  event_header_(event_header),
  iterator_(rawStream.GetVParticleIt(start)),
  fTimeOffset(timeOffset),
  fObservationLevel(observationLevel),
  fIsThinned(isThinned),
  fKeepMuProd(keepMuProd)
{
  Rewind();
}


bool
CorsikaShowerFileParticleIterator::operator==(const CorsikaShowerFileParticleIterator& other) const
{
  if (!IsValid() || !other.IsValid()) {
    return IsValid() == other.IsValid();
  }

  bool ret =
    iterator_         == other.iterator_ &&
    fTimeOffset       == other.fTimeOffset &&
    fObservationLevel == other.fObservationLevel &&
    fIsThinned        == other.fIsThinned &&
    fKeepMuProd       == other.fKeepMuProd;

  return ret;
}


void
CorsikaShowerFileParticleIterator::increment()
{
  boost::optional<CorsikaParticle> parent;
  boost::optional<CorsikaParticle> grandparent;
  boost::optional<CorsikaParticle> muaddi;
  while((value_ = iterator_->GetCorsikaParticle())) {
    const int particleId =
      ParticleList::CorsikaToPDG(int(value_->fDescription/1000));
    const short unsigned int obsLevel =  fmod(value_->fDescription, 10);
    if (value_->fDescription < 0 && !parent) {
      parent = value_;
      continue;
    }
    if (value_->fDescription < 0 && parent) {
      grandparent = value_;
      continue;
    }
    const int temp = int(value_->fDescription/1000);
    if ((temp == 75 || temp == 76)  && !muaddi) {
      muaddi = value_;
      continue;
    }

    if (particleId == CorsikaParticle::eUndefined || (!fKeepMuProd && (particleId == CorsikaParticle::eDecayedMuon || particleId == CorsikaParticle::eDecayedAntiMuon)) || obsLevel != fObservationLevel) {
      // reset and continue
      parent = boost::none;
      grandparent = boost::none;
      muaddi = boost::none;
      continue;
    }

    if (grandparent && parent) {
      value_->SetParent(parent.get());
      value_->SetGrandParent(grandparent.get());
    }
    if (muaddi) {
      value_->SetMuonInfo(muaddi.get());
    }

    // deal with corsika's idiosyncrasies here
    value_->fTorZ -= fTimeOffset;

    return;
  }
}
