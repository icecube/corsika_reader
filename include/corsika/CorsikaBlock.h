/**
   \file
   On-disk block of raw CORSIKA files

   \note This is an internal data structure for the Corsika reader
   that is not installed with the rest of the framework.

   \author Lukas Nellen
   \version $Id$
   \date 19 Nov 2003
*/

#ifndef _corsika_Corsika_Block_h_
#define _corsika_Corsika_Block_h_

static const char CVSId_corsika_Corsika_Block[] =
  "$Id$";

#include <cstddef>
#include <string>
#include <sstream>


namespace corsika
{

    // These are here so the code is explicit. These define the file structure.
    static const int kParticlesInBlock  = 39;
    static const int kLongEntriesPerBlock = 26;

    struct Thinned {
      static const int kWordsPerSubBlock = 312;
      static const int kSubBlocksPerBlock = 21;
      static const int kParticlesInBlock  = corsika::kParticlesInBlock;
      static const int kBytesPerBlock = 26208;
    };

    struct NotThinned {
      static const int kWordsPerSubBlock = 273;
      static const int kSubBlocksPerBlock = 21;
      static const int kParticlesInBlock  = corsika::kParticlesInBlock;
      static const int kBytesPerBlock = 22932;
    };


      /*!
        \class BlockID
        \brief Sub-block used in CORSIKA files
      */
      class BlockID {
      public:
        /// Length of sub-block identifier
        static const size_t kLength = 4;

        /// set from c-string (for testing)
        void SetID(const char* const theID);

        /// Compare ID's
        bool Is(const char* const theID) const;

        //private:
        char fID[kLength];      // hold strings like RUNH etc to
                                // identify the block type
      }; // BlockID

    /*!
      \struct GenericBlock
      \brief Generic block struct for Corsika files
    */
    template <class Thinning>
    struct GenericBlock {
      static const int kWordsPerSubBlock = Thinning::kWordsPerSubBlock;
      float fPad[kWordsPerSubBlock];
    };

    /*!
      \struct ParticleData
      \brief struct with particle data
    */
    template <class Thinning>
    struct ParticleData {
    };

    /*!
      \struct ParticleData
      \brief struct with particle data
    */
    template<>
    struct ParticleData<Thinned> {
      bool IsParticle() const
      { return 0 < fDescription && fDescription < 100000; }
      bool IsNucleus() const
      { return 100000 <= fDescription && fDescription < 9900000; }
      bool IsCherenkov() const
      { return 9900000 <= fDescription; }
      std::string String() const
      {
        std::ostringstream out;
        out << "Description: "  << fDescription << std::endl
            << "Momentum:    (" << fPx << ", " << fPy << ", " << fPz << ")" << std::endl
            << "Position:    (" << fX << ", " << fY << ")" << std::endl
            << "T or Z:       " << fTorZ << std::endl;
      return out.str();
      }


      float fDescription;     // Particle ID
      float fPx, fPy, fPz;    // GeV
      float fX, fY;           // cm
      float fTorZ;            // ns or cm
      float fWeight;          // absent if CORSIKA compiled w/o thinning
    }; // ParticleData

    /*!
      \struct ParticleData
      \brief struct with particle data
    */
    template<>
    struct ParticleData<NotThinned> {
      bool IsParticle() const
      { return 0 < fDescription && fDescription < 100000; }
      bool IsNucleus() const
      { return 100000 <= fDescription && fDescription < 9900000; }
      bool IsCherenkov() const
      { return 9900000 <= fDescription; }
      std::string String() const
      {
        std::ostringstream out;
        out << "Description: "  << fDescription << std::endl
            << "Momentum:    (" << fPx << ", " << fPy << ", " << fPz << ")" << std::endl
            << "Position:    (" << fX << ", " << fY << ")" << std::endl
            << "T or Z:       " << fTorZ << std::endl;
      return out.str();
      }


      float fDescription;     // Particle ID
      float fPx, fPy, fPz;    // GeV
      float fX, fY;           // cm
      float fTorZ;            // ns or cm
    }; // ParticleData

    /*!
      \struct CherenkovData
      \brief struct with Cherenkov data
    */
    template <class Thinning>
    struct CherenkovData {
    };

    /*!
      \struct CherenkovData
      \brief struct with Cherenkov data
    */
    template<>
    struct CherenkovData<Thinned> {
      float fPhotonsInBunch;
      float fX, fY;           // cm
      float fU, fV;           // cos to X and Y axis
      float fT;                       // ns
      float fProductionHeight;        // cm
      float fWeight;          // absent if CORSIKA compiled w/o thinning
    }; // CherenkovData

    /*!
      \struct CherenkovData
      \brief struct with Cherenkov data
    */
    template<>
    struct CherenkovData<NotThinned> {
      float fPhotonsInBunch;
      float fX, fY;           // cm
      float fU, fV;           // cos to X and Y axis
      float fT;                       // ns
      float fProductionHeight;        // cm
    }; // CherenkovData

    /*!
      \struct CherenkovBlock
      \brief block of Cherenkov data
    */

    /*!
      \struct RunHeader
      \brief run header struct for Corsika files
    */
    struct RunHeader {
      static const int kMaxObservationLevels = 10;

      BlockID fID;
      float fRunNumber;
      float fDateStart;
      float fVersion;

      float fObservationLevels;
      float fObservationHeight[kMaxObservationLevels]; // in cm

      float fSpectralSlope;
      float fEMin, fEMax;     // in GeV

      float fFlagEGS4;
      float fFlagNKG;

      float fCutoffHadrons;   // in GeV
      float fCutoffMuons;     // in GeV
      float fCutoffElectrons; // in GeV
      float fCutoffPhotons;   // in GeV

      float fConstC[50];
      float fConstCC[20];
      float fConstCKA[40];
      float fConstCETA[5];
      float fConstCSTRBA[11];
      float fConstUNUSED[4];
      float fConstCAN[50];
      float fConstCANN[50];
      float fConstAATM[5];
      float fConstBATM[5];
      float fConstCATM[5];
      float fConstNFLAIN;
      float fConstNFLDIF;
      float fConstNFLPI;
      float fConstNFLCHE;

    }; // RunHeader

    /*!
      \struct EventHeader
      \brief event header struct for Corsika files
    */
    struct EventHeader {
      static const int kMaxObservationLevels = 10;

      BlockID fID;
      float fEventNumber;
      float fParticleId;
      float fEnergy;
      float fStartingAltitude;        // g cm^-2
      float fFirstTarget;     //
      float fZFirst;          // cm
      float fPx, fPy, fPz;    // GeV
      float fTheta;           // zenith in radians
      float fPhi;             // azimuth in radians

      float fRandomSequences;
      struct {
	float fSeed;
	float fInitialCallsMod;
	float fInitialCallsDiv;
      } fSeeds[10];

      float fRunNumber;
      float fDateStart;
      float fVersion;

      float fObservationLevels;
      float fObservationHeight[kMaxObservationLevels]; // in cm

      float fSpectralSlope;   //
      float fEMin, fEMax;     // in GeV

      float fCutoffHadrons;   // in GeV
      float fCutoffMuons;     // in GeV
      float fCutoffElectrons; // in GeV
      float fCutoffPhotons;   // in GeV

      float fNFLAIN;
      float fNFLDIF;
      float fNFLPI0;
      float fNFLPIF;
      float fNFLCHE;
      float fNFRAGM;

      float fBx, fBz;         // magnetic field in mu T
      float fFlagEGS4;
      float fFlagNKG;

      float fFlagLowEnergyModel;
      float fFlagHighEnergyModel;
      float fFlagCerenkov;
      float fFlagNeutrino;
      float fFlagCurved;
      float fFlagComputer;    // 1: IBM, 2: Transputer, 3: DEC/UNIX, 4: Mac,
                                // 5: VAX/VMS, 6: GNU/Linux

      float fThetaMin, fThetaMax; // degrees
      float fPhiMin, fPhiMax; // degrees

      float fCerenkovBunch;
      float fCerenkovNumberX, fCerenkovNumberY;
      float fCerenkovGridX, fCerenkovGridY; // cm
      float fCerenkovDetectorX, fCerenkovDetectorY; // cm
      float fCerenkovOutputFlag;

      float fArrayRotation;
      float fFlagExtraMuonInformation;

      float fMultipleScatteringStep;
      float fCerenkovBandwidthMin, fCerenkovBandwidthMax; // nm
      float fUsersOfEvent;
      float fCoreX[20], fCoreY[20]; // cm

      float fFlagSIBYLL, fFlagSIBYLLCross;
      float fFlagQGSJET, fFlagQGSJETCross;
      float fFlagDPMJET, fFlagDPMJETCross;
      float fFlagVENUSCross;
      float fFlagMuonMultiple; // 0: Gauss, 1: Moilere
      float fNKGRadialRange;  // cm
      float fEFractionThinningH; // Energy fraction of thinning
                                   // level
      // These are in the CORSIKA manual but not in Lukas's original code
      float fEFractionThinningEM; // Energy fraction of thinning level EM
      float fWMaxHadronic, fWMaxEM;
      float fRMaxThinning;
      float fInnerAngle, fOuterAngle;

      float fHighLowEnergyTransition;
      float fSkimmingIncidence;
      float fSkimmingAltitude;
      float fStartingHeight;
      float fCharmON;

      float fPad[114];
    }; // EventHeader

    /*!
      \struct RunTrailer
      \brief run trailer struct for Corsika files
    */
    struct RunTrailer {
      BlockID fID;
      float fRunNumber;
      float fEventsProcessed;

      float fPad[270];
    }; // RunTrailer

    /*!
      \struct EventTrailer
      \brief event trailer struct for Corsika files
    */
    struct EventTrailer {
      BlockID fID;
      float fEventNumber;

      float fPhotons;
      float fElectrons;
      float fHadrons;
      float fMuons;
      float fParticles;

      // NKG output
      float fLateral1X[21], fLateral1Y[21]; // cm^-2
      float fLateral1XY[21], fLateral1YX[21]; // cm^-2

      float fLateral2X[21], fLateral2Y[21]; // cm^-2
      float fLateral2XY[21], fLateral2YX[21]; // cm^-2

      float fElectronNumber[10]; // in steps of 100 g cm^-2
      float fAge[10];         // in steps of 100 g cm^-2
      float fDistances[10];   // cm
      float fLocalAge1[10];

      float fLevelHeightMass[10]; // g cm^-2
      float fLevelHeightDistance[10]; // cm
      float fDistanceBinsAge[10]; // cm
      float fLocalAge2[10];

      // Longitudinal distribution
      float fLongitudinalPar[6];
      float fChi2;
      // Added according to the CORSIKA manual
      float fWeightedPhotons;
      float fWeightedElectrons;
      float fWeightedHadrons;
      float fWeightedMuons;

      float fPad[7];
    }; // EventTrailer

    struct LongitudinalEntry {
      float fDepth;
      float fGamma;
      float fEplus;
      float fEminus;
      float fMuPlus;
      float fMuMinus;
      float fHadron;
      float fCharged;
      float fNuclei;
      float fCherenkov;
    };

    struct LongitudinalBlock {
      BlockID fID;
      float fEventNumber;
      float fParticleID;
      float fTotalEnergy;
      float fStepsAndBlocks;
      float fCurrentBlock;
      float fFirstInteractionAltitude;
      float fZenith;
      float fAzimuth;
      float fHadronCutoff;
      float fMuonCutoff;
      float fElectronCutoff;
      float fPhotonCutoff;

      LongitudinalEntry fEntries[kLongEntriesPerBlock];
    };

    /**
       \class Block CorsikaBlock.h "Corsika/CorsikaBlock.h"

       \brief This class represents a corsika block. It deals with all
       the different sub-types of blocks. Grouping of Blocks into a
       block on disk is done by the corsika::Corsika::RawFile class.

       \note This type deals with the machine-dependent representation
       on-disk. This class has to be reviewed when porting to different
       architectures or compilers.

       \todo Implement support to test padding requirements when
       configuring the software. Alternatively, think of a way to
       determine the size of a corsika block with padding at run-time
       and use C++'s placement new to deal with padding offsets.

       \todo Deal with byte-ordering issues to makes showers
       transferable between MSB and LSB architectures.

       \author Lukas Nellen
       \date 19 Nov 2003
       \ingroup corsika
    */
    template <class Thinning=Thinned>
    class Block {
    public:
      static const int kParticlesInBlock  = corsika::kParticlesInBlock;
      static const int kLongEntriesPerBlock = corsika::kLongEntriesPerBlock;

      typedef corsika::BlockID BlockID;

      typedef corsika::GenericBlock<Thinning> GenericBlock;
      typedef corsika::RunHeader RunHeader;
      typedef corsika::RunTrailer RunTrailer;
      typedef corsika::EventHeader EventHeader;
      typedef corsika::EventTrailer EventTrailer;
      typedef corsika::LongitudinalEntry LongitudinalEntry;
      typedef corsika::LongitudinalBlock LongitudinalBlock;

      typedef corsika::ParticleData<Thinning> ParticleData;

      struct ParticleBlock {
        ParticleData fParticle[kParticlesInBlock];
      };

      typedef corsika::CherenkovData<Thinning> CherenkovData;

      struct CherenkovBlock {
        CherenkovData fParticle[kParticlesInBlock];
      };

      /*!
        \union SubBlock
        \brief union of blocks
      */
      union SubBlock {
        GenericBlock fGenericBlock;
        RunHeader fRunHeader;
        RunTrailer fRunTrailer;
        EventHeader fEventHeader;
        EventTrailer fEventTrailer;
        ParticleBlock fParticleBlock;
        CherenkovBlock fCherenkovBlock;
        LongitudinalBlock fLongitudinalBlock;
      }; // SubBlock

      bool IsRunHeader() const { return fSubBlock.fRunHeader.fID.Is("RUNH"); }
      bool IsRunTrailer() const { return fSubBlock.fRunTrailer.fID.Is("RUNE"); }
      bool IsEventHeader() const { return fSubBlock.fEventHeader.fID.Is("EVTH"); }
      bool IsEventTrailer() const { return fSubBlock.fEventTrailer.fID.Is("EVTE"); }
      bool IsLongitudinal() const { return fSubBlock.fEventTrailer.fID.Is("LONG"); }
      bool IsControl() const
      { return IsRunHeader() || IsRunTrailer() || IsEventHeader() || IsEventTrailer(); }

      const RunHeader& AsRunHeader() const
      { return fSubBlock.fRunHeader; }
      const RunTrailer& AsRunTrailer() const
      { return fSubBlock.fRunTrailer; }
      const EventHeader& AsEventHeader() const
      { return fSubBlock.fEventHeader; }
      const EventTrailer& AsEventTrailer() const
      { return fSubBlock.fEventTrailer; }
      const ParticleBlock& AsParticleBlock() const
      { return fSubBlock.fParticleBlock; }
      const CherenkovBlock& AsCherenkovBlock() const
      { return fSubBlock.fCherenkovBlock; }
      const LongitudinalBlock& AsLongitudinalBlock() const
      { return fSubBlock.fLongitudinalBlock; }

      std::string ID() const
      { return std::string(fSubBlock.fRunHeader.fID.fID); }

      float* Data()
      { return &fSubBlock.fGenericBlock.fPad[0]; }

      Block<Thinning>
      CopyBlock() const
      {
	Block<Thinning> out;
	for (int i = 0; i != GenericBlock::kWordsPerSubBlock; ++i) {
	  out.fSubBlock.fGenericBlock.fPad[i] = fSubBlock.fGenericBlock.fPad[i];
	}
	return out;
      }
    private:
      SubBlock fSubBlock;

    }; // Block
} // io


#endif // _corsika_Corsika_Block_h_

// Configure (x)emacs for this file ...
// Local Variables:
// mode: c++
// compile-command: "make -C .. -k"
// End:
