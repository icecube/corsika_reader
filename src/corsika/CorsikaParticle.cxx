/**
   \file


   \author Javier Gonzalez
   \version $Id: TEMPLATE.cc.tpl,v 1.4 2003/09/25 14:38:19 lukas Exp $
   \date 04 May 2011
*/

#include <corsika/CorsikaParticle.h>
#include <corsika/CorsikaBlock.h>
#include <corsika/particle/ParticleList.h>
#include <corsika/CorsikaUnits.h>
#include <corsika/particle/NucleusProperties.h>
#include <iostream>
#include <sstream>
#include <corsika/particle/ParticleList.h>

using namespace corsika;
using namespace std;

CorsikaParticle::CorsikaParticle():
  fDescription(0),
  fPx(0),
  fPy(0),
  fPz(0),
  fX(0),
  fY(0),
  fTorZ(0),
  fWeight(0)
{
}


CorsikaParticle::CorsikaParticle(const ParticleData<Thinned>& particle):
  fDescription(particle.fDescription),
  fPx(particle.fPx),
  fPy(particle.fPy),
  fPz(particle.fPz),
  fX(particle.fX),
  fY(particle.fY),
  fTorZ(particle.fTorZ),
  fWeight(particle.fWeight)
{
}


CorsikaParticle::CorsikaParticle(const ParticleData<NotThinned>& particle):
  fDescription(particle.fDescription),
  fPx(particle.fPx),
  fPy(particle.fPy),
  fPz(particle.fPz),
  fX(particle.fX),
  fY(particle.fY),
  fTorZ(particle.fTorZ),
  fWeight(1)
{
}


CorsikaParticle::~CorsikaParticle()
{
}


int
CorsikaParticle::NucleusCode(const int theCharge, const int theAtomicNumber)
{
  return corsika::NucleusProperties::TypeCode(theCharge, theAtomicNumber);
}

std::string
CorsikaParticle::GetName() const
{
  return ParticleList::Get(PDGCode()).GetName();
}

int
CorsikaParticle::PDGCode()
const
{
  return ParticleList::CorsikaToPDG(abs(int(fDescription/1000)));
}

int
CorsikaParticle::CorsikaCode()
const
{
  return abs(int(fDescription/1000));
}

float
CorsikaParticle::Mass() const
{
  return ParticleList::Get(PDGCode()).GetMass();
}

float
CorsikaParticle::Momentum() const
{
  return std::sqrt(fPx*fPx + fPy*fPy + fPz*fPz);
}

float
CorsikaParticle::KineticEnergy() const
{
  const float momentum = Momentum();
  const float mass = Mass();
  return std::sqrt(momentum*momentum + mass*mass) - mass;
}

float
CorsikaParticle::TotalEnergy() const
{
  const float momentum = Momentum();
  const float mass = Mass();
  return std::sqrt(momentum*momentum + mass*mass);
}

std::string
CorsikaParticle::String() const
{
  ostringstream out;
  out << "CorsikaParticle:" << endl
      << "   " << "PDGCode:            " << PDGCode() << " (" << GetName() << ")" << endl
      << "   " << "CorsikaCode:        " << CorsikaCode() << endl
      << "   " << "Momentum (GeV):     (" << fPx/GeV << ", " << fPy/GeV << ", " << fPz/GeV << ")" << endl
      << "   " << "Position  (km):     (" << fX/km << ", " << fY/km << ")" << endl
      << "   " << "T or Z (ns or cm):  " << fTorZ << endl
      << "   " << "Weight:             " << fWeight << endl;
  return out.str();
}


// Configure (x)emacs for this file ...
// Local Variables:
// mode:c++
// compile-command: "make -C .. -k"
// End:
