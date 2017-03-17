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

namespace
{
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
    /*
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
    */
}

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
        ParticleProperties(Particle::eUndefined,       "Undefined",       0.),
        ParticleProperties(Particle::eElectron,        "e-",              kElectronMass),
        ParticleProperties(Particle::ePositron,        "e+",              kElectronMass),
        ParticleProperties(Particle::eNuElectron,      "nu_e",            0.),
        ParticleProperties(Particle::eAntiNuElectron,  "anti_nu_e",       0.),
        ParticleProperties(Particle::eMuon,            "mu-",             kMuonMass),
        ParticleProperties(Particle::eAntiMuon,        "mu+",             kMuonMass),
        ParticleProperties(Particle::eNuMuon,          "nu_mu",           0.),
        ParticleProperties(Particle::eAntiNuMuon,      "anti_nu_mu",      0.),
        ParticleProperties(Particle::eTau,             "tau-",            kTauMass),
        ParticleProperties(Particle::eAntiTau,         "tau+",            kTauMass),
        ParticleProperties(Particle::eNuTau,           "nu_tau",          0.),
        ParticleProperties(Particle::eAntiNuTau,       "anti_nu_tau",     0.),
        ParticleProperties(Particle::ePhoton,          "gamma",           0.),
        ParticleProperties(Particle::ePiZero,          "pi0",             kPiZeroMass),
        ParticleProperties(Particle::ePiPlus,          "pi+",             kPiChargedMass),
        ParticleProperties(Particle::ePiMinus,         "pi-",             kPiChargedMass),
        ParticleProperties(Particle::eEta,             "eta",             kEtaMass),
        ParticleProperties(Particle::eKaon0L,          "kaon0L",          kKaonZeroMass),
        ParticleProperties(Particle::eKaon0S,          "kaon0S",          kKaonZeroMass),
        ParticleProperties(Particle::eKaonPlus,        "kaon+",           kKaonChargedMass),
        ParticleProperties(Particle::eKaonMinus,       "kaon-",           kKaonChargedMass),
        ParticleProperties(Particle::eLambda,          "lambda",          kLambdaMass),
        ParticleProperties(Particle::eLambdac,         "lambda_c+",       kLambdacMass),
        ParticleProperties(Particle::eAntiLambda,      "anti_lambda",     kLambdaMass),
        ParticleProperties(Particle::eSigmaPlus,       "sigma+",          kSigmaPlusMass),
        ParticleProperties(Particle::eSigmaZero,       "sigma0",          kSigmaZeroMass),
        ParticleProperties(Particle::eSigmaMinus,      "sigma-",          kSigmaMinusMass),
        ParticleProperties(Particle::eAntiSigmaPlus,   "anti_sigma+",     kSigmaPlusMass),
        ParticleProperties(Particle::eAntiSigmaZero,   "anti_sigma0",     kSigmaZeroMass),
        ParticleProperties(Particle::eAntiSigmaMinus,  "anti_sigma-",     kSigmaMinusMass),
        ParticleProperties(Particle::eXiZero,          "xi0",             kXiZeroMass),
        ParticleProperties(Particle::eXiMinus,         "xi-",             kXiMinusMass),
        ParticleProperties(Particle::eAntiXiZero,      "anti_xi0",        kXiZeroMass),
        ParticleProperties(Particle::eAntiXiMinus,     "anti_xi-",        kXiMinusMass),
        ParticleProperties(Particle::eOmegaMinus,      "omega-",          kOmegaMinusMass),
        ParticleProperties(Particle::eAntiOmegaMinus,  "anti_omega-",     kOmegaMinusMass),
        ParticleProperties(Particle::eNeutron,         "neutron",         kNeutronMass),
        ParticleProperties(Particle::eAntiNeutron,     "anti_neutron",    kNeutronMass),
        ParticleProperties(Particle::eProton,          "proton",          kProtonMass),
        ParticleProperties(Particle::eAntiProton,      "anti_proton",     kProtonMass),
        ParticleProperties(Particle::eRhoZero,         "rho_zero",        kRhoZeroMass),
        ParticleProperties(Particle::eRhoPlus,         "rho_plus",        kRhoPlusMass),
        ParticleProperties(Particle::eRhoMinus,        "rho_minus",       kRhoMinusMass),
        ParticleProperties(Particle::eOmegaMeson,      "omega_meson",     kOmegaMesonMass),
        ParticleProperties(Particle::eDeltaMinus,      "delta_minus",     kDeltaMinusMass),
        ParticleProperties(Particle::eDeltaZero,       "delta_zero",      kDeltaZeroMass),
        ParticleProperties(Particle::eDeltaPlus,       "delta_plus",      kDeltaPlusMass),
        ParticleProperties(Particle::eDeltaPlusPlus,   "delta_plus_plus", kDeltaPlusPlusMass),
        ParticleProperties(Particle::eDecayedMuon,     "decayed mu-",     kMuonMass),
        ParticleProperties(Particle::eDecayedAntiMuon, "decayed mu+",     kMuonMass),
    };
    
    for (auto& p: prop)
        if (particles_.find(p.GetType()) == particles_.end())
            particles_.insert(std::pair<int, ParticleProperties>(p.GetType(), p));
}

void ParticleList::InitCorsikaToPDGMap()
{
    corsikaToPDGMap_ =
    {
        {1, Particle::ePhoton},
        {2, Particle::ePositron},
        {3, Particle::eElectron},
        
        {5, Particle::eAntiMuon},
        {6, Particle::eMuon},
        {7, Particle::ePiZero},
        {8, Particle::ePiPlus},
        {9, Particle::ePiMinus},
        
        {10, Particle::eKaon0L},
        {11, Particle::eKaonPlus},
        {12, Particle::eKaonMinus},
        {13, Particle::eNeutron},
        {14, Particle::eProton},
        {15, Particle::eAntiProton},
        {16, Particle::eKaon0S},
        {17, Particle::eEta},
        {18, Particle::eLambda},
        
        {19, Particle::eSigmaPlus},
        {20, Particle::eSigmaZero},
        {21, Particle::eSigmaMinus},
        {22, Particle::eXiZero},
        {23, Particle::eXiMinus},
        {24, Particle::eOmegaMinus},
        
        {25, Particle::eAntiNeutron},
        {26, Particle::eAntiLambda},
        
        {27, Particle::eAntiSigmaPlus},
        {28, Particle::eAntiSigmaZero},
        {29, Particle::eAntiSigmaMinus},
        {30, Particle::eAntiXiZero},
        {31, Particle::eAntiXiMinus}, // actually plus?
        {32, Particle::eAntiOmegaMinus}, // actually plus?
        
        {50, Particle::eOmegaMeson},
        {51, Particle::eRhoZero},
        {52, Particle::eRhoPlus},
        {53, Particle::eRhoMinus},
        {54, Particle::eDeltaPlusPlus},
        {55, Particle::eDeltaPlus},
        {56, Particle::eDeltaZero},
        {57, Particle::eDeltaMinus},
        
        // not in the enumeration type
        // {58, Particle::eAntiDeltaPlusPlus},
        // {59, Particle::eAntiDeltaPlus},
        // {60, Particle::eAntiDeltaZero},
        // {61, Particle::eAntiDeltaMinus},
        // {62, Particle::eKZeroResonance},
        // {63, Particle::eKPlusResonance},
        // {64, Particle::eKMinusResonance},
        // {65, Particle::eAntiKZeroResonance},
        
        {66, Particle::eNuElectron},
        {67, Particle::eAntiNuElectron},
        {68, Particle::eNuMuon},
        {69, Particle::eAntiNuMuon},
        
        {71, Particle::eEta},
        {72, Particle::eEta},
        {73, Particle::eEta},
        {74, Particle::eEta},
        
        {95, Particle::eDecayedAntiMuon},
        {96, Particle::eDecayedMuon},
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
            return Particle::eUndefined;
    }
    else if (theCorsikaCode < 9900) // nucleus
    {
        int Z = theCorsikaCode % 100;
        int A = theCorsikaCode / 100;
        return Particle::NucleusCode(Z, A);
    }
    // Cherenkov
    return Particle::eUndefined;
}

std::string ParticleList::NameFromPDG(int pdgCode)
{
    return ParticleList::Get(pdgCode).GetName();
}

std::string ParticleList::NameFromCorsika(int corsikaCode)
{
    return ParticleList::Get(CorsikaToPDG(corsikaCode)).GetName();
}
