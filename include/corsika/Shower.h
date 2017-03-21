/**
 \file
 
 
 \author Javier Gonzalez
 \version $Id: TEMPLATE.h.tpl,v 1.5 2003/09/25 14:38:19 lukas Exp $
 \date 04 May 2011
 */

#pragma once
#include <corsika/Block.h>
#include <corsika/ShowerParticleStream.h>
#include <corsika/GaisserHillasParameter.h>
#include <vector>

namespace corsika
{
    struct CorsikaShower
    {
        CorsikaShower(): particle_stream(0) {}
        CorsikaShower(const EventHeader& header, const EventTrailer& trailer, ShowerParticleStream* particle_stream);
        ~CorsikaShower(){}
        
        int GetPrimary() const            {return fPrimaryParticle;   }
        float GetEnergy() const           {return fEnergy;         }
        float GetMuonNumber() const       {return fMuonNumber;      }
        float GetZenith() const           {return fZenith;          }
        float GetAzimuth() const          {return fAzimuth;         }
        float GetMinRadiusCut() const     {return fMinRadiusCut;    }
        int GetShowerNumber() const       {return fShowerNumber;      }
        //int GetShowerRunId() const      {//return fShowerRunId;     }
        float GetEMEnergyCutoff() const   {return fEMEnergyCutoff;  }
        float GetMuonEnergyCutoff() const {return fMuonEnergyCutoff;}
        
        
        ShowerParticleStream& ParticleStream() const
        {
            particle_stream->Rewind();
            //++(*fParticleIterator);
            return *particle_stream;
        }
        
        GaisserHillasParameter GetGaisserHillasParams() const
        {return fGH; }
        void  SetGaisserHillasParams(const GaisserHillasParameter& gh)
        { fGH = gh; }
        
        float GetCalorimetricEnergy() const
        { return fCalorimetricEnergy; }
        
        void SetCalorimetricEnergy(float e)
        { fCalorimetricEnergy = e; }
        
        int HighEnergyHadronicModel()
        { return int(fEventHeader.fFlagHighEnergyModel); }
        
        int LowEnergyHadronicModel()
        { return int(fEventHeader.fFlagLowEnergyModel); }
        
        float ArrayRotation()
        { return fEventHeader.fArrayRotation; }
        
        
        const EventHeader& GetEventHeader() const
        { return fEventHeader; }
        
        const EventTrailer& GetEventTrailer() const
        { return fEventTrailer; }
        
        std::vector<double> fdEdX;
        std::vector<double> fChargeProfile;
        std::vector<double> fGammaProfile;
        std::vector<double> fElectronProfile;
        std::vector<double> fPositronProfile;
        std::vector<double> fMuonProfile;
        std::vector<double> fAntiMuonProfile;
        std::vector<double> fHadronProfile;
        std::vector<double> fNucleiProfile;
        std::vector<double> fCherenkovProfile;
        std::vector<double> fDepth;
        std::vector<double> fDepth_dE;
        
    private:
        EventHeader fEventHeader;
        EventTrailer fEventTrailer;
        
        int fPrimaryParticle;
        float fEnergy;
        float fMuonNumber;
        float fZenith;
        float fAzimuth;
        float fMinRadiusCut;
        int fShowerNumber;
        //int fShowerRunId;
        float fEMEnergyCutoff;
        float fMuonEnergyCutoff;
        float fCalorimetricEnergy;
        GaisserHillasParameter fGH;
        
        ShowerParticleStream* particle_stream; // TODO who owns this pointer?
    };
}
