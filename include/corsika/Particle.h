/**
 \file
 
 
 \author Javier Gonzalez
 \version $Id: TEMPLATE.h.tpl,v 1.5 2003/09/25 14:38:19 lukas Exp $
 \date 04 May 2011
 */

#pragma once
#include <boost/shared_ptr.hpp>
#include <corsika/Block.h>
#include <cmath>
#include <memory>


namespace corsika
{
    typedef boost::shared_ptr<struct Particle> ParticlePtr;
    
    /**
     \class Particle Particle.h "corsika/Particle.h"
     
     \ingroup corsika
     */
    struct Particle
    {
        enum Type
        {
            eUndefined = 0,
            eElectron = 11, ePositron = -11,
            eNuElectron = 12, eAntiNuElectron = -12,
            eMuon = 13, eAntiMuon = -13,
            eNuMuon = 14, eAntiNuMuon = -14,
            eTau = 15, eAntiTau = -15,
            eNuTau = 16, eAntiNuTau = -16,
            ePhoton = 22,
            ePiZero = 111,
            eRhoZero = 113,
            ePiPlus = 211, ePiMinus = -211,
            eRhoPlus = 213, eRhoMinus = -213,
            eEta = 221,
            eOmegaMeson = 223,
            eKaon0L = 130, eKaon0S = 310,
            eKaonPlus = 321, eKaonMinus = -321,
            eDeltaMinus = 1114,
            eLambda = 3122, eAntiLambda = -3122,
            eSigmaPlus = 3222, eSigmaZero = 3212, eSigmaMinus = 3112,
            eAntiSigmaPlus = -3222, eAntiSigmaZero = -3212, eAntiSigmaMinus = -3112,
            eXiZero = 3322, eXiMinus = 3312,
            eAntiXiZero = -3322, eAntiXiMinus = -3312,
            eOmegaMinus = 3332,
            eAntiOmegaMinus = -3332,
            eLambdac = 4122,
            eNeutron = 2112, eAntiNeutron = -2112,
            eDeltaZero = 2114,
            eProton = 2212, eAntiProton = -2212,
            eDeltaPlus = 2214, eDeltaPlusPlus = 2224,
            eDecayedMuon = 9900013, eDecayedAntiMuon = -9900013,
            // Selected nuclei.
            // Note: This is an inconsistency left in for hysterical raisins only.
            //       Cf. NucleusProperties instead!
            //       Usage example thereof can be found in CONEXFile.cc.
            eIron = 1000026056
        };
        
        Particle();
        Particle(const ParticleData<Thinned>& particle);
        Particle(const ParticleData<NotThinned>& particle);
        
        
        int PDGCode() const;
        
        int CorsikaCode() const;
        
        std::string GetName() const;
        
        int HadronicGeneration() const
        { return int(std::fmod(fDescription, 1000) / 10); }
        
        int ObservingLevel() const
        { return int(std::fmod(fDescription, 10)) - 1; }
        
        bool IsParticle() const
        { return 0 < fDescription && fDescription < 100000; }
        bool IsNucleus() const
        { return 100000 <= fDescription && fDescription < 9900000; }
        bool IsCherenkov() const
        { return 9900000 <= fDescription; }
        
        float fDescription;     // Particle ID
        float fPx, fPy, fPz;    // GeV
        float fX, fY;           // cm
        float fTorZ;            // ns or cm
        float fWeight;          // absent if CORSIKA compiled w/o thinning
        
        /// Calculate particle type code for a given (A, Z)
        static int NucleusCode(const int theCharge, const int theAtomicNumber);
        
        float Mass() const;
        float Momentum() const;
        float KineticEnergy() const;
        float TotalEnergy() const;
        
        bool HasParent() const { return bool(fParent); }
        void SetParent(const Particle& p) { fParent.reset(new Particle(p)); }
        Particle& GetParent() { return *fParent; }
        
        void SetGrandParent(const Particle& p) { fGrandParent.reset(new Particle(p));  }
        Particle& GetGrandParent() { return *fGrandParent; }
        
        bool HasMuonInfo() const { return bool(fMuonInfo); }
        void SetMuonInfo(const Particle& p) { fMuonInfo.reset(new Particle(p));  }
        Particle& GetMuonInfo() { return *fMuonInfo; }
        
        std::string String() const;
        
    private:
        ParticlePtr fParent, fMuonInfo, fGrandParent;
        
    };
}
