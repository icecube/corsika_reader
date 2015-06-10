/**
   \file


   \author Javier Gonzalez
   \version $Id: TEMPLATE.h.tpl,v 1.5 2003/09/25 14:38:19 lukas Exp $
   \date 04 May 2011
*/

#ifndef _corsika_CorsikaParticle_h_
#define _corsika_CorsikaParticle_h_

static const char CVSId_corsika_CorsikaParticle[] =
"$Id$";


#include <corsika/CorsikaBlock.h>
#include <cmath>
#include <boost/shared_ptr.hpp>

namespace corsika {


    struct ParticleData;

    class CorsikaParticle {
    public:

      enum Type {
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
	//       Cf. corsika::NucleusProperties instead!
	//       Usage example thereof can be found in CONEXFile.cc.
	eIron = 1000026056
      };

      CorsikaParticle();
      CorsikaParticle(const Corsika::Block<Corsika::Thinned>::ParticleData* particle);
      CorsikaParticle(const Corsika::Block<Corsika::NotThinned>::ParticleData* particle);
      ~CorsikaParticle();


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

      bool HasParent() const
      { return fParent; }

      void SetParent(CorsikaParticle& p)
      { fParent.reset(new CorsikaParticle(p)); }
      CorsikaParticle& GetParent()
      { return *fParent; }

      void SetGrandParent(CorsikaParticle p)
      { fGrandParent.reset(new CorsikaParticle(p));  }
      CorsikaParticle& GetGrandParent()
      { return *fGrandParent; }

      bool HasMuonInfo() const
      { return fMuonInfo; }

      void SetMuonInfo(CorsikaParticle p)
      { fMuonInfo.reset(new CorsikaParticle(p));  }
      CorsikaParticle& GetMuonInfo()
      { return *fMuonInfo; }

      std::string String() const;

    private:
      boost::shared_ptr<CorsikaParticle> fParent;
      boost::shared_ptr<CorsikaParticle> fMuonInfo;
      boost::shared_ptr<CorsikaParticle> fGrandParent;

   }; // CorsikaParticle

} // corsika


#endif // _corsika_CorsikaParticle_h_

// Configure (x)emacs for this file ...
// Local Variables:
// mode:c++
// compile-command: "make -C .. -k"
// End:
