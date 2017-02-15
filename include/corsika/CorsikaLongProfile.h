/**
   \file


   \author Javier Gonzalez
   \version $Id:
   \date 14 Feb 2015
*/

#ifndef _corsika_CorsikaLongProfile_h_
#define _corsika_CorsikaLongProfile_h_

static const char CVSId_corsika_CorsikaLongProfile[] =
"$Id$";

#include <vector>
#include <corsika/GaisserHillasParameter.h>

namespace corsika {

  class CorsikaLongProfile {
  public:
    CorsikaLongProfile(){}
    ~CorsikaLongProfile(){}

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
    std::vector<double> fdEdX;
    std::vector<double> fDepth_dE;

    double fCalorimetricEnergy;
    GaisserHillasParameter fGaisserHillas;

  };

}

#endif
