/**
   \file


   \author Javier Gonzalez
   \version $Id: TEMPLATE.h.tpl,v 1.5 2003/09/25 14:38:19 lukas Exp $
   \date 04 May 2011
*/

#ifndef _io_CorsikaShower_h_
#define _io_CorsikaShower_h_

static const char CVSId_io_CorsikaShower[] =
"$Id$";

#include <corsika/CorsikaBlock.h>
#include <corsika/CorsikaShowerFileParticleIterator.h>
#include <corsika/GaisserHillasParameter.h>

#include <vector>

namespace corsika {

  class CorsikaShower {
  public:
    CorsikaShower(): fParticleIterator(0) {}
    CorsikaShower(const EventHeader& header,
                  const EventTrailer& trailer,
                  CorsikaShowerFileParticleIterator* particleIt);
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


    CorsikaShowerFileParticleIterator& GetParticleIt() const
    {
      fParticleIterator->Rewind();
      //++(*fParticleIterator);
      return *fParticleIterator;
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

    // EventTrailer& EventTrailer()
    // { return fEventTrailer; }

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
    struct EventHeader fEventHeader;
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

    CorsikaShowerFileParticleIterator* fParticleIterator;
  };

} // io


#endif // _io_CorsikaShower_h_

// Configure (x)emacs for this file ...
// Local Variables:
// mode:c++
// compile-command: "make -C .. -k"
// End:
