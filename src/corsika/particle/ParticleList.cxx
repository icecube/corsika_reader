/**
   \file
   A simple list of particle properties

   \author Javier Gonzalez
   \version $Id$
   \date 04 Jan 2015
*/
#include <corsika/particle/ParticleList.h>
#include <corsika/Particle.h>
#include <corsika/Units.h>
#include <corsika/IOException.h>
#include <vector>
#include <sstream>

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
    
    if (!NucleusProperties::IsNucleus(code))
    {
        std::ostringstream msg;
        msg << "Cannot create particle properties for particle id " << code << ".";
        throw IOException(msg.str());
    }
    
    nuclei_.insert(std::pair<int, NucleusProperties>(code, NucleusProperties(code)));
    return nuclei_.find(code)->second;
}


void ParticleList::SetList()
{
    if (particles_.size()>0) return;
    
    std::vector<ParticleProperties> prop =
    {
        ParticleProperties(CorsikaParticle::eUndefined,       "Undefined",       0.),
        ParticleProperties(CorsikaParticle::eElectron,        "e-",              kElectronMass),
        ParticleProperties(CorsikaParticle::ePositron,        "e+",              kElectronMass),
        ParticleProperties(CorsikaParticle::eNuElectron,      "nu_e",            0.),
        ParticleProperties(CorsikaParticle::eAntiNuElectron,  "anti_nu_e",       0.),
        ParticleProperties(CorsikaParticle::eMuon,            "mu-",             kMuonMass),
        ParticleProperties(CorsikaParticle::eAntiMuon,        "mu+",             kMuonMass),
        ParticleProperties(CorsikaParticle::eNuMuon,          "nu_mu",           0.),
        ParticleProperties(CorsikaParticle::eAntiNuMuon,      "anti_nu_mu",      0.),
        ParticleProperties(CorsikaParticle::eTau,             "tau-",            kTauMass),
        ParticleProperties(CorsikaParticle::eAntiTau,         "tau+",            kTauMass),
        ParticleProperties(CorsikaParticle::eNuTau,           "nu_tau",          0.),
        ParticleProperties(CorsikaParticle::eAntiNuTau,       "anti_nu_tau",     0.),
        ParticleProperties(CorsikaParticle::ePhoton,          "gamma",           0.),
        ParticleProperties(CorsikaParticle::ePiZero,          "pi0",             kPiZeroMass),
        ParticleProperties(CorsikaParticle::ePiPlus,          "pi+",             kPiChargedMass),
        ParticleProperties(CorsikaParticle::ePiMinus,         "pi-",             kPiChargedMass),
        ParticleProperties(CorsikaParticle::eEta,             "eta",             kEtaMass),
        ParticleProperties(CorsikaParticle::eKaon0L,          "kaon0L",          kKaonZeroMass),
        ParticleProperties(CorsikaParticle::eKaon0S,          "kaon0S",          kKaonZeroMass),
        ParticleProperties(CorsikaParticle::eKaonPlus,        "kaon+",           kKaonChargedMass),
        ParticleProperties(CorsikaParticle::eKaonMinus,       "kaon-",           kKaonChargedMass),
        ParticleProperties(CorsikaParticle::eLambda,          "lambda",          kLambdaMass),
        ParticleProperties(CorsikaParticle::eLambdac,         "lambda_c+",       kLambdacMass),
        ParticleProperties(CorsikaParticle::eAntiLambda,      "anti_lambda",     kLambdaMass),
        ParticleProperties(CorsikaParticle::eSigmaPlus,       "sigma+",          kSigmaPlusMass),
        ParticleProperties(CorsikaParticle::eSigmaZero,       "sigma0",          kSigmaZeroMass),
        ParticleProperties(CorsikaParticle::eSigmaMinus,      "sigma-",          kSigmaMinusMass),
        ParticleProperties(CorsikaParticle::eAntiSigmaPlus,   "anti_sigma+",     kSigmaPlusMass),
        ParticleProperties(CorsikaParticle::eAntiSigmaZero,   "anti_sigma0",     kSigmaZeroMass),
        ParticleProperties(CorsikaParticle::eAntiSigmaMinus,  "anti_sigma-",     kSigmaMinusMass),
        ParticleProperties(CorsikaParticle::eXiZero,          "xi0",             kXiZeroMass),
        ParticleProperties(CorsikaParticle::eXiMinus,         "xi-",             kXiMinusMass),
        ParticleProperties(CorsikaParticle::eAntiXiZero,      "anti_xi0",        kXiZeroMass),
        ParticleProperties(CorsikaParticle::eAntiXiMinus,     "anti_xi-",        kXiMinusMass),
        ParticleProperties(CorsikaParticle::eOmegaMinus,      "omega-",          kOmegaMinusMass),
        ParticleProperties(CorsikaParticle::eAntiOmegaMinus,  "anti_omega-",     kOmegaMinusMass),
        ParticleProperties(CorsikaParticle::eNeutron,         "neutron",         kNeutronMass),
        ParticleProperties(CorsikaParticle::eAntiNeutron,     "anti_neutron",    kNeutronMass),
        ParticleProperties(CorsikaParticle::eProton,          "proton",          kProtonMass),
        ParticleProperties(CorsikaParticle::eAntiProton,      "anti_proton",     kProtonMass),
        ParticleProperties(CorsikaParticle::eRhoZero,         "rho_zero",        kRhoZeroMass),
        ParticleProperties(CorsikaParticle::eRhoPlus,         "rho_plus",        kRhoPlusMass),
        ParticleProperties(CorsikaParticle::eRhoMinus,        "rho_minus",       kRhoMinusMass),
        ParticleProperties(CorsikaParticle::eOmegaMeson,      "omega_meson",     kOmegaMesonMass),
        ParticleProperties(CorsikaParticle::eDeltaMinus,      "delta_minus",     kDeltaMinusMass),
        ParticleProperties(CorsikaParticle::eDeltaZero,       "delta_zero",      kDeltaZeroMass),
        ParticleProperties(CorsikaParticle::eDeltaPlus,       "delta_plus",      kDeltaPlusMass),
        ParticleProperties(CorsikaParticle::eDeltaPlusPlus,   "delta_plus_plus", kDeltaPlusPlusMass),
        ParticleProperties(CorsikaParticle::eDecayedMuon,     "decayed mu-",     kMuonMass),
        ParticleProperties(CorsikaParticle::eDecayedAntiMuon, "decayed mu+",     kMuonMass),
    };
    
    for (auto& p: prop)
        if (particles_.find(p.GetType()) == particles_.end())
            particles_.insert(std::pair<int, ParticleProperties>(p.GetType(), p));
}

void ParticleList::InitCorsikaToPDGMap()
{
    corsikaToPDGMap_ =
    {
        {1, CorsikaParticle::ePhoton},
        {2, CorsikaParticle::ePositron},
        {3, CorsikaParticle::eElectron},
        
        {5, CorsikaParticle::eAntiMuon},
        {6, CorsikaParticle::eMuon},
        {7, CorsikaParticle::ePiZero},
        {8, CorsikaParticle::ePiPlus},
        {9, CorsikaParticle::ePiMinus},
        
        {10, CorsikaParticle::eKaon0L},
        {11, CorsikaParticle::eKaonPlus},
        {12, CorsikaParticle::eKaonMinus},
        {13, CorsikaParticle::eNeutron},
        {14, CorsikaParticle::eProton},
        {15, CorsikaParticle::eAntiProton},
        {16, CorsikaParticle::eKaon0S},
        {17, CorsikaParticle::eEta},
        {18, CorsikaParticle::eLambda},
        
        {19, CorsikaParticle::eSigmaPlus},
        {20, CorsikaParticle::eSigmaZero},
        {21, CorsikaParticle::eSigmaMinus},
        {22, CorsikaParticle::eXiZero},
        {23, CorsikaParticle::eXiMinus},
        {24, CorsikaParticle::eOmegaMinus},
        
        {25, CorsikaParticle::eAntiNeutron},
        {26, CorsikaParticle::eAntiLambda},
        
        {27, CorsikaParticle::eAntiSigmaPlus},
        {28, CorsikaParticle::eAntiSigmaZero},
        {29, CorsikaParticle::eAntiSigmaMinus},
        {30, CorsikaParticle::eAntiXiZero},
        {31, CorsikaParticle::eAntiXiMinus}, // actually plus?
        {32, CorsikaParticle::eAntiOmegaMinus}, // actually plus?
        
        {50, CorsikaParticle::eOmegaMeson},
        {51, CorsikaParticle::eRhoZero},
        {52, CorsikaParticle::eRhoPlus},
        {53, CorsikaParticle::eRhoMinus},
        {54, CorsikaParticle::eDeltaPlusPlus},
        {55, CorsikaParticle::eDeltaPlus},
        {56, CorsikaParticle::eDeltaZero},
        {57, CorsikaParticle::eDeltaMinus},
        
        // not in the enumeration type
        // {58, CorsikaParticle::eAntiDeltaPlusPlus},
        // {59, CorsikaParticle::eAntiDeltaPlus},
        // {60, CorsikaParticle::eAntiDeltaZero},
        // {61, CorsikaParticle::eAntiDeltaMinus},
        // {62, CorsikaParticle::eKZeroResonance},
        // {63, CorsikaParticle::eKPlusResonance},
        // {64, CorsikaParticle::eKMinusResonance},
        // {65, CorsikaParticle::eAntiKZeroResonance},
        
        {66, CorsikaParticle::eNuElectron},
        {67, CorsikaParticle::eAntiNuElectron},
        {68, CorsikaParticle::eNuMuon},
        {69, CorsikaParticle::eAntiNuMuon},
        
        {71, CorsikaParticle::eEta},
        {72, CorsikaParticle::eEta},
        {73, CorsikaParticle::eEta},
        {74, CorsikaParticle::eEta},
        
        {95, CorsikaParticle::eDecayedAntiMuon},
        {96, CorsikaParticle::eDecayedMuon},
    };
}


int ParticleList::CorsikaToPDG(int theCorsikaCode)
{
    if (theCorsikaCode < 100)
    {
        if (!corsikaToPDGMap_.size()) InitCorsikaToPDGMap();
        
        auto index = corsikaToPDGMap_.find(theCorsikaCode);
        
        if (index != corsikaToPDGMap_.end())
            return index->second;
        else
            return CorsikaParticle::eUndefined;
    }
    else if (theCorsikaCode < 9900) // nucleus
    {
        int Z = theCorsikaCode % 100;
        int A = theCorsikaCode / 100;
        return CorsikaParticle::NucleusCode(Z, A);
    }
    // Cherenkov
    return CorsikaParticle::eUndefined;
}


std::string ParticleList::NameFromPDG(int pdgCode)
{
    return ParticleList::Get(pdgCode).GetName();
}

std::string ParticleList::NameFromCorsika(int corsikaCode)
{
    return ParticleList::Get(CorsikaToPDG(corsikaCode)).GetName();
}
