/**
   \file
   Implementation of the VShowerFileParticleIterator for an Corsika generated
   shower file
   \author Troy Porter
   \author Lukas Nellen
   \version $Id$
   \date 22 May 2003
*/

#pragma once
#include <corsika/CorsikaBlock.h>
#include <corsika/RawParticleIterator.h>
#include <corsika/CorsikaParticle.h>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/optional.hpp>

namespace corsika
{


  struct CorsikaShowerFileParticleIterator
  {

    boost::optional<CorsikaParticle> NextParticle();

    

    CorsikaShowerFileParticleIterator();
    CorsikaShowerFileParticleIterator(RawStreamPtr rawStream,
				      EventHeader event_header, // passed by value
                                      unsigned long int start,
                                      const double timeOffset,
                                      const unsigned int observationLevel,
                                      const bool isThinned,
                                      const bool keepMuProd);


    virtual void Rewind() { iterator_->Rewind(); }



  private:

    bool IsValid() const { return iterator_ && iterator_->IsValid(); }

    mutable boost::optional<CorsikaParticle> value_;
    EventHeader event_header_;
    RawParticleIteratorPtr iterator_;

    double fTimeOffset;
    unsigned int fObservationLevel;
    bool fIsThinned;
    bool fKeepMuProd;
  };

}
