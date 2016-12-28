/**
   \file
   A simple list of particle properties

   \author Javier Gonzalez
   \version $Id$
   \date 04 Jan 2015
*/

static const char CVSId[] =
  "$Id$";


#include <corsika/particle/ParticleList.h>
#include <corsika/particle/NucleusProperties.h>
#include <corsika/CorsikaParticle.h>
#include <corsika/CorsikaUnits.h>
#include <corsika/CorsikaIOException.h>
#include <vector>
#include <sstream>
#include <iostream>

using namespace corsika;

namespace particle_constants {
  // these were originally in a header file as enumerations, but I want to keep all particle information in one place
  const double kElectronMass      = 0.510998902 * MeV;
  const double kMuonMass          = 105.658357  * MeV;
  const double kTauMass           = 1776.99     * MeV;
  const double kProtonMass        = 938.271998  * MeV;
  const double kNeutronMass       = 939.56533   * MeV;
  const double kDeuteronMass      = 1875.612762 * MeV;
  const double kLambdaMass        = 1115.683    * MeV;
  const double kLambdacMass       = 2286.46     * MeV;
  const double kSigmaZeroMass     = 1192.642    * MeV;
  const double kSigmaPlusMass     = 1189.37     * MeV;
  const double kSigmaMinusMass    = 1197.449    * MeV;
  const double kXiZeroMass        = 1314.83     * MeV;
  const double kXiMinusMass       = 1321.31     * MeV;
  const double kOmegaMinusMass    = 1672.45     * MeV;
  const double kPiZeroMass        = 134.9766    * MeV;
  const double kPiChargedMass     = 139.57018   * MeV;
  const double kKaonZeroMass      = 497.672     * MeV; // | K^0 _s - K^0 _l | ~ 10^-12 MeV
  const double kKaonChargedMass   = 493.677     * MeV;
  const double kEtaMass           = 547.30      * MeV;
  const double kRhoZeroMass       = 775.49      * MeV;
  const double kRhoPlusMass       = 775.11      * MeV;
  const double kRhoMinusMass      = 775.11      * MeV;
  const double kOmegaMesonMass    = 782.65      * MeV;
  const double kDeltaMinusMass    = 1.232       * MeV;
  const double kDeltaZeroMass     = 1.232       * MeV;
  const double kDeltaPlusMass     = 1.232       * MeV;
  const double kDeltaPlusPlusMass = 1.232       * MeV;

   // Particle lifetimes
  const double kMuonLifetime          = 2.19703e-6 * s;
  const double kNeutronLifetime       = 885.7      * s;
  const double kLambdaLifetime        = 2.632e-10  * s;
  const double kSigmaZeroLifetime     = 7.4e-20    * s;
  const double kSigmaPlusLifetime     = 0.8018e-10 * s;
  const double kSigmaMinusLifetime    = 1.479e-10  * s;
  const double kXiZeroLifetime        = 2.9e-10    * s;
  const double kXiMinusLifetime       = 1.639e-10  * s;
  const double kOmegaMinusLifetime    = 0.821e-10  * s;
  const double kPiZeroLifetime        = 8.4e-17    * s;
  const double kPiChargedLifetime     = 2.6033e-8  * s;
  const double kKaonZeroShortLifetime = 0.8934e-10 * s;
  const double kKaonZeroLongLifetime  = 5.17e-8    * s;
  const double kKaonChargedLifetime   = 1.2384e-8  * s;


}

using namespace particle_constants;

std::map<int, int> ParticleList::corsikaToPDGMap_;
std::map<int, ParticleProperties> ParticleList::particles_;
std::map<int, NucleusProperties> ParticleList::nuclei_;

const VParticleProperties& ParticleList::Get(int code)
{
  SetList();

  std::map<int,ParticleProperties>::iterator it = particles_.find(code);
  if (it != particles_.end())
    return it->second;

  if (!NucleusProperties::IsNucleus(code)) {
    std::ostringstream msg;
    msg << "Cannot create particle properties for particle id "
        << code
        << ".";
    std::cerr << msg.str() << std::endl;
    throw CorsikaIOException(msg.str());
  }

  nuclei_.insert(std::pair<int, NucleusProperties>(code, NucleusProperties(code)));
  return nuclei_.find(code)->second;

}


void
ParticleList::SetList()
{
  if (particles_.size()>0)
    return;

  std::vector<ParticleProperties> prop;
  prop.push_back(ParticleProperties(CorsikaParticle::eUndefined,       "Undefined",       0.));
  prop.push_back(ParticleProperties(CorsikaParticle::eElectron,        "e-",              kElectronMass));
  prop.push_back(ParticleProperties(CorsikaParticle::ePositron,        "e+",              kElectronMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eNuElectron,      "nu_e",            0.));
  prop.push_back(ParticleProperties(CorsikaParticle::eAntiNuElectron,  "anti_nu_e",       0.));
  prop.push_back(ParticleProperties(CorsikaParticle::eMuon,            "mu-",             kMuonMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eAntiMuon,        "mu+",             kMuonMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eNuMuon,          "nu_mu",           0.));
  prop.push_back(ParticleProperties(CorsikaParticle::eAntiNuMuon,      "anti_nu_mu",      0.));
  prop.push_back(ParticleProperties(CorsikaParticle::eTau,             "tau-",            kTauMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eAntiTau,         "tau+",            kTauMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eNuTau,           "nu_tau",          0.));
  prop.push_back(ParticleProperties(CorsikaParticle::eAntiNuTau,       "anti_nu_tau",     0.));
  prop.push_back(ParticleProperties(CorsikaParticle::ePhoton,          "gamma",           0.));
  prop.push_back(ParticleProperties(CorsikaParticle::ePiZero,          "pi0",             kPiZeroMass));
  prop.push_back(ParticleProperties(CorsikaParticle::ePiPlus,          "pi+",             kPiChargedMass));
  prop.push_back(ParticleProperties(CorsikaParticle::ePiMinus,         "pi-",             kPiChargedMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eEta,             "eta",             kEtaMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eKaon0L,          "kaon0L",          kKaonZeroMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eKaon0S,          "kaon0S",          kKaonZeroMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eKaonPlus,        "kaon+",           kKaonChargedMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eKaonMinus,       "kaon-",           kKaonChargedMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eLambda,          "lambda",          kLambdaMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eLambdac,         "lambda_c+",       kLambdacMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eAntiLambda,      "anti_lambda",     kLambdaMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eSigmaPlus,       "sigma+",          kSigmaPlusMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eSigmaZero,       "sigma0",          kSigmaZeroMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eSigmaMinus,      "sigma-",          kSigmaMinusMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eAntiSigmaPlus,   "anti_sigma+",     kSigmaPlusMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eAntiSigmaZero,   "anti_sigma0",     kSigmaZeroMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eAntiSigmaMinus,  "anti_sigma-",     kSigmaMinusMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eXiZero,          "xi0",             kXiZeroMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eXiMinus,         "xi-",             kXiMinusMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eAntiXiZero,      "anti_xi0",        kXiZeroMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eAntiXiMinus,     "anti_xi-",        kXiMinusMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eOmegaMinus,      "omega-",          kOmegaMinusMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eAntiOmegaMinus,  "anti_omega-",     kOmegaMinusMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eNeutron,         "neutron",         kNeutronMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eAntiNeutron,     "anti_neutron",    kNeutronMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eProton,          "proton",          kProtonMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eAntiProton,      "anti_proton",     kProtonMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eRhoZero,         "rho_zero",        kRhoZeroMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eRhoPlus,         "rho_plus",        kRhoPlusMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eRhoMinus,        "rho_minus",       kRhoMinusMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eOmegaMeson,      "omega_meson",     kOmegaMesonMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eDeltaMinus,      "delta_minus",     kDeltaMinusMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eDeltaZero,       "delta_zero",      kDeltaZeroMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eDeltaPlus,       "delta_plus",      kDeltaPlusMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eDeltaPlusPlus,   "delta_plus_plus", kDeltaPlusPlusMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eDecayedMuon,     "decayed mu-",     kMuonMass));
  prop.push_back(ParticleProperties(CorsikaParticle::eDecayedAntiMuon, "decayed mu+",     kMuonMass));

  for (unsigned int i = 0; i != prop.size(); ++i) {
    if(particles_.find(prop[i].GetType()) == particles_.end())
      particles_.insert(std::pair<int, ParticleProperties>(prop[i].GetType(), prop[i]));
  }
}


void
ParticleList::InsertCorsikaToPDG(int theCorsikaCode, int thePDGCode)
{
  if ( !corsikaToPDGMap_.insert(std::make_pair(theCorsikaCode,
                                               thePDGCode)).second ) {
    std::ostringstream msg;
    msg << "Cannot insert pair ("
        << theCorsikaCode << ", " << thePDGCode
        << ") into CorsikaToPDG map.";
    throw CorsikaIOException(msg.str());
  }
}

void
ParticleList::InitCorsikaToPDGMap()
{
  InsertCorsikaToPDG(1, CorsikaParticle::ePhoton);
  InsertCorsikaToPDG(2, CorsikaParticle::ePositron);
  InsertCorsikaToPDG(3, CorsikaParticle::eElectron);

  InsertCorsikaToPDG(5, CorsikaParticle::eAntiMuon);
  InsertCorsikaToPDG(6, CorsikaParticle::eMuon);
  InsertCorsikaToPDG(7, CorsikaParticle::ePiZero);
  InsertCorsikaToPDG(8, CorsikaParticle::ePiPlus);
  InsertCorsikaToPDG(9, CorsikaParticle::ePiMinus);

  InsertCorsikaToPDG(10, CorsikaParticle::eKaon0L);
  InsertCorsikaToPDG(11, CorsikaParticle::eKaonPlus);
  InsertCorsikaToPDG(12, CorsikaParticle::eKaonMinus);
  InsertCorsikaToPDG(13, CorsikaParticle::eNeutron);
  InsertCorsikaToPDG(14, CorsikaParticle::eProton);
  InsertCorsikaToPDG(15, CorsikaParticle::eAntiProton);
  InsertCorsikaToPDG(16, CorsikaParticle::eKaon0S);
  InsertCorsikaToPDG(17, CorsikaParticle::eEta);
  InsertCorsikaToPDG(18, CorsikaParticle::eLambda);

  InsertCorsikaToPDG(19, CorsikaParticle::eSigmaPlus);
  InsertCorsikaToPDG(20, CorsikaParticle::eSigmaZero);
  InsertCorsikaToPDG(21, CorsikaParticle::eSigmaMinus);
  InsertCorsikaToPDG(22, CorsikaParticle::eXiZero);
  InsertCorsikaToPDG(23, CorsikaParticle::eXiMinus);
  InsertCorsikaToPDG(24, CorsikaParticle::eOmegaMinus);

  InsertCorsikaToPDG(25, CorsikaParticle::eAntiNeutron);
  InsertCorsikaToPDG(26, CorsikaParticle::eAntiLambda);

  InsertCorsikaToPDG(27, CorsikaParticle::eAntiSigmaPlus);
  InsertCorsikaToPDG(28, CorsikaParticle::eAntiSigmaZero);
  InsertCorsikaToPDG(29, CorsikaParticle::eAntiSigmaMinus);
  InsertCorsikaToPDG(30, CorsikaParticle::eAntiXiZero);
  InsertCorsikaToPDG(31, CorsikaParticle::eAntiXiMinus); // actually plus?
  InsertCorsikaToPDG(32, CorsikaParticle::eAntiOmegaMinus); // actually plus?

  InsertCorsikaToPDG(50, CorsikaParticle::eOmegaMeson);
  InsertCorsikaToPDG(51, CorsikaParticle::eRhoZero);
  InsertCorsikaToPDG(52, CorsikaParticle::eRhoPlus);
  InsertCorsikaToPDG(53, CorsikaParticle::eRhoMinus);
  InsertCorsikaToPDG(54, CorsikaParticle::eDeltaPlusPlus);
  InsertCorsikaToPDG(55, CorsikaParticle::eDeltaPlus);
  InsertCorsikaToPDG(56, CorsikaParticle::eDeltaZero);
  InsertCorsikaToPDG(57, CorsikaParticle::eDeltaMinus);

  // not in the enumeration type
  // InsertCorsikaToPDG(58, CorsikaParticle::eAntiDeltaPlusPlus);
  // InsertCorsikaToPDG(59, CorsikaParticle::eAntiDeltaPlus);
  // InsertCorsikaToPDG(60, CorsikaParticle::eAntiDeltaZero);
  // InsertCorsikaToPDG(61, CorsikaParticle::eAntiDeltaMinus);
  // InsertCorsikaToPDG(62, CorsikaParticle::eKZeroResonance);
  // InsertCorsikaToPDG(63, CorsikaParticle::eKPlusResonance);
  // InsertCorsikaToPDG(64, CorsikaParticle::eKMinusResonance);
  // InsertCorsikaToPDG(65, CorsikaParticle::eAntiKZeroResonance);

  InsertCorsikaToPDG(66, CorsikaParticle::eNuElectron);
  InsertCorsikaToPDG(67, CorsikaParticle::eAntiNuElectron);
  InsertCorsikaToPDG(68, CorsikaParticle::eNuMuon);
  InsertCorsikaToPDG(69, CorsikaParticle::eAntiNuMuon);

  InsertCorsikaToPDG(71, CorsikaParticle::eEta);
  InsertCorsikaToPDG(72, CorsikaParticle::eEta);
  InsertCorsikaToPDG(73, CorsikaParticle::eEta);
  InsertCorsikaToPDG(74, CorsikaParticle::eEta);

  InsertCorsikaToPDG(95, CorsikaParticle::eDecayedAntiMuon);
  InsertCorsikaToPDG(96, CorsikaParticle::eDecayedMuon);
}


int
ParticleList::CorsikaToPDG(int theCorsikaCode)
{
  if (theCorsikaCode < 100) {

    if (!corsikaToPDGMap_.size())
      InitCorsikaToPDGMap();

    std::map<int, int>::const_iterator index =
      corsikaToPDGMap_.find(theCorsikaCode);

    if (index != corsikaToPDGMap_.end()) {
      return index->second;
    } else {
      return CorsikaParticle::eUndefined;
    }

  } else if (theCorsikaCode < 9900) {                   // nucleus

    unsigned int Z = theCorsikaCode % 100;
    unsigned int A = theCorsikaCode / 100;
    return CorsikaParticle::NucleusCode(Z, A);

  } else {                                              // Cherenkov
    return CorsikaParticle::eUndefined;
  }
} // CorsikaToPDG


std::string
ParticleList::NameFromPDG(int pdgCode)
{
  return ParticleList::Get(pdgCode).GetName();
}

std::string
ParticleList::NameFromCorsika(int corsikaCode)
{
  return ParticleList::Get(CorsikaToPDG(corsikaCode)).GetName();
}

// Configure (x)emacs for this file ...
// Local Variables:
// mode:c++
// compile-command: "make -C .. -k"
// End:
