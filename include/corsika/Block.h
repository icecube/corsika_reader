/**
 \file
 On-disk block of raw CORSIKA files
 
 \note This is an internal data structure for the Corsika reader
 that is not installed with the rest of the framework.
 
 \author Lukas Nellen
 \version $Id$
 \date 19 Nov 2003
 */

#pragma once
#include <string>

namespace corsika
{
    
    // These are here so the code is explicit. These define the file structure.
    static const size_t kParticlesInBlock  = 39;
    static const size_t kLongEntriesPerBlock = 26;
    static const size_t kSubBlocksPerBlock = 21;
    
    struct Thinned
    {
        static const size_t kWordsPerSubBlock = 312;
    };
    
    struct NotThinned
    {
        static const size_t kWordsPerSubBlock = 273;
    };
    
    
    /*!
     \struct BlockID
     \brief Sub-block used in CORSIKA files
     */
    struct BlockID
    {
        char fID[4];
        bool operator==(const char* str) const;
    };
    
    /*!
     \template GenericBlock
     \brief Generic block struct for Corsika files
     */
    template <class Thinning> struct GenericBlock
    {
        float fPad[Thinning::kWordsPerSubBlock];
    };
    
    /*!
     \template ParticleData
     \brief struct with particle data
     */
    template <class Thinning> struct ParticleData
    {
    };
    
    /*!
     \template ParticleData
     \brief struct with particle data
     */
    template<> struct ParticleData<Thinned>
    {
        bool IsParticle() const
        { return 0 < fDescription && fDescription < 100000; }
        bool IsNucleus() const
        { return 100000 <= fDescription && fDescription < 9900000; }
        bool IsCherenkov() const
        { return 9900000 <= fDescription; }
        std::string String() const;
        
        float fDescription;     // Particle ID
        float fPx, fPy, fPz;    // GeV
        float fX, fY;           // cm
        float fTorZ;            // ns or cm
        float fWeight;          // absent if CORSIKA compiled w/o thinning
    }; // ParticleData
    
    /*!
     \template ParticleData
     \brief struct with particle data
     */
    template<> struct ParticleData<NotThinned>
    {
        bool IsParticle() const
        { return 0 < fDescription && fDescription < 100000; }
        bool IsNucleus() const
        { return 100000 <= fDescription && fDescription < 9900000; }
        bool IsCherenkov() const
        { return 9900000 <= fDescription; }
        std::string String() const;
        
        float fDescription;     // Particle ID
        float fPx, fPy, fPz;    // GeV
        float fX, fY;           // cm
        float fTorZ;            // ns or cm
    }; // ParticleData
    
    /*!
     \template CherenkovData
     \brief struct with Cherenkov data
     */
    template <class Thinning> struct CherenkovData
    {
    };
    
    /*!
     \template CherenkovData
     \brief struct with Cherenkov data
     */
    template<> struct CherenkovData<Thinned>
    {
        float fPhotonsInBunch;
        float fX, fY;           // cm
        float fU, fV;           // cos to X and Y axis
        float fT;                       // ns
        float fProductionHeight;        // cm
        float fWeight;          // absent if CORSIKA compiled w/o thinning
    }; // CherenkovData
    
    /*!
     \template CherenkovData
     \brief struct with Cherenkov data
     */
    template<> struct CherenkovData<NotThinned>
    {
        float fPhotonsInBunch;
        float fX, fY;           // cm
        float fU, fV;           // cos to X and Y axis
        float fT;                       // ns
        float fProductionHeight;        // cm
    }; // CherenkovData
    
    /*!
     \struct RunHeader
     \brief run header struct for Corsika files
     */
    struct RunHeader
    {
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
    struct EventHeader
    {
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
    struct RunTrailer
    {
        BlockID fID;
        float fRunNumber;
        float fEventsProcessed;
        
        float fPad[270];
    }; // RunTrailer
    
    /*!
     \struct EventTrailer
     \brief event trailer struct for Corsika files
     */
    struct EventTrailer
    {
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
    
    struct LongitudinalEntry
    {
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
    
    struct LongitudinalBlock
    {
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
     \class Block Block.h "corsika/Block.h"
     
     \ingroup corsika
     */
    template <typename Thinning> struct Block
    {
        struct ParticleBlock
        {
            ParticleData<Thinning> fParticle[kParticlesInBlock];
        };
        struct CherenkovBlock
        {
            CherenkovData<Thinning> fParticle[kParticlesInBlock];
        };
        bool IsRunHeader() const { return AsRunHeader.fID == "RUNH"; }
        bool IsRunTrailer() const { return AsRunTrailer.fID == "RUNE"; }
        bool IsEventHeader() const { return AsEventHeader.fID == "EVTH"; }
        bool IsEventTrailer() const { return AsEventTrailer.fID == "EVTE"; }
        bool IsLongitudinal() const { return AsEventTrailer.fID == "LONG"; }
        bool IsControl() const { return IsRunHeader() || IsRunTrailer() || IsEventHeader() || IsEventTrailer(); }
        float* Data() { return AsGenericBlock.fPad; }
        std::string ID(){ return std::string(AsRunHeader.fID.fID); }
        
        union
        {
            GenericBlock<Thinning> AsGenericBlock;
            RunHeader AsRunHeader;
            RunTrailer AsRunTrailer;
            EventHeader AsEventHeader;
            EventTrailer AsEventTrailer;
            ParticleBlock AsParticleBlock;
            CherenkovBlock AsCherenkovBlock;
            LongitudinalBlock AsLongitudinalBlock;
        };
        
    };
}
