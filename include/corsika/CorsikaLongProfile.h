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

namespace corsika {

  class CorsikaLongProfile {
  public:
    std::vector<double> fChargeProfile;
    std::vector<double> fGammaProfile;
    std::vector<double> fElectronProfile;
    std::vector<double> fMuonProfile;
    std::vector<double> fDepth;

  };

}
