/**
 \file
 \author Javier Gonzalez
 \version $Id: TEMPLATE.cc.tpl,v 1.4 2003/09/25 14:38:19 lukas Exp $
 \date 04 May 2011
 */

#include <corsika/Particle.h>
#include <corsika/particle/ParticleList.h>
#include <corsika/Units.h>
#include <sstream>
using namespace corsika;

Particle::Particle():
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

Particle::Particle(const ParticleData<Thinned>& particle):
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

Particle::Particle(const ParticleData<NotThinned>& particle):
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

int Particle::NucleusCode(const int theCharge, const int theAtomicNumber)
{
    return NucleusProperties::TypeCode(theCharge, theAtomicNumber);
}

std::string Particle::GetName() const
{
    return ParticleList::Get(PDGCode()).GetName();
}

int Particle::PDGCode() const
{
    return ParticleList::CorsikaToPDG(abs(int(fDescription/1000)));
}

int Particle::CorsikaCode() const
{
    return abs(int(fDescription/1000));
}

float Particle::Mass() const
{
    return ParticleList::Get(PDGCode()).GetMass();
}

float Particle::Momentum() const
{
    return std::sqrt(fPx*fPx + fPy*fPy + fPz*fPz);
}

float Particle::KineticEnergy() const
{
    const float momentum = Momentum();
    const float mass = Mass();
    return std::sqrt(momentum*momentum + mass*mass) - mass;
}

float Particle::TotalEnergy() const
{
    const float momentum = Momentum();
    const float mass = Mass();
    return std::sqrt(momentum*momentum + mass*mass);
}

std::string Particle::String() const
{
    std::ostringstream out;
    out << "Particle:" << std::endl
    << "   " << "PDGCode:            " << PDGCode() << " (" << GetName() << ")" << std::endl
    << "   " << "CorsikaCode:        " << CorsikaCode() << std::endl
    << "   " << "Momentum (GeV):     (" << fPx/GeV << ", " << fPy/GeV << ", " << fPz/GeV << ")" << std::endl
    << "   " << "Position  (km):     (" << fX/km << ", " << fY/km << ")" << std::endl
    << "   " << "T or Z (ns or cm):  " << fTorZ << std::endl
    << "   " << "Weight:             " << fWeight << std::endl;
    return out.str();
}
