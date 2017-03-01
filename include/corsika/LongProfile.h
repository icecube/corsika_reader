/**
 \file
 
 
 \author Javier Gonzalez
 \version $Id:
 \date 14 Feb 2015
 */

#pragma once
#include <vector>
#include <corsika/GaisserHillasParameter.h>

namespace corsika
{
    struct CorsikaLongProfile
    {
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
