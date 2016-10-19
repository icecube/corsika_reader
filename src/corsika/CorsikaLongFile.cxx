/**
   \file
   Implementation of the Corsika shower reader

   \author Lukas Nellen
   \version $Id$
   \date 29 Jan 2004
*/

static const char CVSId[] =
  "$Id$";

#include <corsika/Constants.h>
#include <corsika/CorsikaLongFile.h>
#include <corsika/CorsikaIOException.h>

#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <cstdio>
#include <iostream>

#include <boost/tokenizer.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace corsika;

namespace fs = boost::filesystem;

typedef boost::tokenizer<boost::char_separator<char> > mytok;

static void log(const std::string& mess)
{
  std::cout << mess << std::endl;
}
static void log(const std::ostringstream& mess)
{
  std::cout << mess.str() << std::endl;
}

#define INFO(mess) log(mess);
#define ERROR(mess) log(mess);
#define FATAL(mess) log(mess);

namespace {
  bool SplitLine(string l, vector<string>& vec, string s = " ")
  {
    boost::char_separator<char> sep(s.c_str());
    mytok tok(l,sep);
    vec.clear();
    for (mytok::const_iterator titer=tok.begin();titer!=tok.end();++titer) {
      vec.push_back(*titer);
    }
    return true;
  }

  string particleProfileStr = " LONGITUDINAL DISTRIBUTION IN";
  string dEdX_ProfileStr = " LONGITUDINAL ENERGY DEPOSIT";
  string GHFit_Str = " FIT OF THE HILLAS CURVE";

}

CorsikaLongFile::CorsikaLongFile(string filename, double zenith):
  fFilename(filename),
  fCosZenith(cos(zenith)),
  fIsSlantDepthProfile(false),
  fN(0),
  fDx(0.),
  fNBinsParticles(0),
  fNBinsEnergyDeposit(0),
  fLongDataFile(new std::ifstream(fFilename.c_str()))
{
  // check if file exists and is readable here
  Scan();
}


CorsikaLongProfile
CorsikaLongFile::GetProfile(int event)
{
  if (event < 0) {
    throw CorsikaIOException("event ID must be non negative.");
  }
  if (event+1 > fN) {
    ostringstream msg;
    msg << "Trying to get event " << event << ", but file " << fFilename << " has only " << fN << " events (starting at zero).";
    throw CorsikaIOException(msg.str());
  }
  return FetchProfile(event);
}

void
CorsikaLongFile::Scan()
{
  // scan for number of bins and dX,
  // also check if there are energy deposit profiles

  static const double g = 1;
  static const double cm = 1;
  static const double cm2 = cm*cm;
  static const double deg = 3.14159/180;

  // Read CORSIKA profile if available
  if (!fLongDataFile->is_open()) {
    return;
  }

  string line;
  while (getline(*fLongDataFile.get(), line) &&
         (fPartProfiles.size()==0 || fdEdXProfiles.size()==0)) {

    if (line.find(particleProfileStr) != string::npos) {
      fPartProfiles.push_back(fLongDataFile->tellg());
      char s1[50], s2[50], s3[50], isVertOrSlant[50];
      sscanf(line.c_str(), "%s %s %s %i %s", s1, s2, s3, &fNBinsParticles, isVertOrSlant);
      string testStr(isVertOrSlant);
      if (testStr == "VERTICAL")
        fIsSlantDepthProfile = false;
      else if (testStr == "SLANT")
        fIsSlantDepthProfile = true;
      else {
        ostringstream err;
        err << "Corsika longitunal file \"" << fFilename << "\" is invalid:\n"
          "line: \"" << line << "\" "
          "contains invalid string at                ^^^^^^^\n"
          "which is neither VERTICAL nor SLANT !";
        ERROR(err);
        throw CorsikaIOException(err.str());
      }
    }

    if (line.find(dEdX_ProfileStr) != string::npos) {
      fdEdXProfiles.push_back(fLongDataFile->tellg());
      char s1[50], s2[50], s3[50], s4[50], isVertOrSlant[50], s5 [50], s6 [50];
      sscanf(line.c_str(),
             "%s %s %s %s %i %s %s %s %f",
             s1, s2, s3, s4, &fNBinsEnergyDeposit, isVertOrSlant, s5, s6, &fDx);
      string testStr(isVertOrSlant);
      if (testStr == "VERTICAL") {
        fIsSlantDepthProfile = false;
        fDx /= fCosZenith;
      } else if (testStr == "SLANT")
        fIsSlantDepthProfile = true;
      else {
        ostringstream err;
        err << "Corsika longitunal file \"" << fFilename << "\" is invalid:\n"
          "line: \"" << line << "\" "
          "contains invalid string at                ^^^^^^^\n"
          " which is neither VERTICAL nor SLANT !";
        ERROR(err);
        throw CorsikaIOException(err.str());
      }
    }
  } // --  end scan file  ---
  while (getline(*fLongDataFile.get(), line)) {
    if (line.find(particleProfileStr) != string::npos) {
      fPartProfiles.push_back(fLongDataFile->tellg());
    }
    if (line.find(dEdX_ProfileStr) != string::npos) {
      fdEdXProfiles.push_back(fLongDataFile->tellg());
    }
  }
  fN = (fPartProfiles.size()?fPartProfiles.size():fdEdXProfiles.size());
}


CorsikaLongProfile
CorsikaLongFile::FetchProfile(int findShower)
{
  bool aux_flag = false;
  int i = 0;
  int j = 0;
  string line;

  vector<double> auxDepth(fNBinsParticles);
  vector<double> auxCharge(fNBinsParticles);
  vector<double> auxMuons(fNBinsParticles);
  vector<double> auxGammas(fNBinsParticles);
  vector<double> auxElectrons(fNBinsParticles);

  vector<double> auxDepth_dE (fNBinsEnergyDeposit);
  vector<double> auxDeltaEn (fNBinsEnergyDeposit);

  corsika::GaisserHillasParameter gh;
  double energyDepositSum = 0.;

  // Read CORSIKA profile if available
  if (!fLongDataFile->is_open()) {
    ERROR("Reading failed for some reason.");
    return CorsikaLongProfile();
  }
  fLongDataFile->seekg(fPartProfiles[findShower]);

  vector<string> tokens;
  while (getline(*fLongDataFile.get(), line)) {
    SplitLine(line, tokens);
    if (line.find(particleProfileStr) != string::npos) {
      getline(*fLongDataFile.get(), line);
      SplitLine(line, tokens);
    }
    if (line.find(dEdX_ProfileStr) != string::npos || aux_flag) {
      if (line.find(dEdX_ProfileStr) != string::npos) {
        getline(*fLongDataFile.get(), line);
        SplitLine(line, tokens);
      }
      if (tokens.size() > 1 && tokens[0] == string("DEPTH") && tokens[1] == string("GAMMA")) {
        aux_flag = true;
        getline(*fLongDataFile.get(), line);
        SplitLine(line, tokens);
      }
      if (line.find(GHFit_Str) != string::npos)
        break;
      if (aux_flag && j < fNBinsParticles) {

        float xdepth = 0.0;
        float gamma = 0.0;
        float emIoniz = 0.0;
        float emCut = 0.0;
        float muonIoniz = 0.0;
        float muonCut = 0.0;
        float hadronIoniz = 0.0;
        float hadronCut = 0.0;
        float neutrino = 0.0;
        float sumEnergy = 0.0;

        sscanf(line.c_str(), "%f %f %f %f %f %f %f %f %f %f",
               &xdepth, &gamma, &emIoniz, &emCut, &muonIoniz, &muonCut,
               &hadronIoniz, &hadronCut, &neutrino, &sumEnergy);

        if (!fIsSlantDepthProfile)
          xdepth /= fCosZenith;

        // dEdX profile has slightly different depth-bins
        auxDepth_dE[j] = xdepth;
        //cout << j << " " << hadroncut  << " " << muoncut
        //   << " " << neutrino << " " << sumenergy << endl;
        // Subtracting neutrino energy
        // and take fraction for muons and hadrons from
        //     Barbosa et al Astro. Part. Phys. 22, (2004) p. 159
        static float muonFraction = 0.575;
        static float hadronFraction = 0.261;
        static float hadronGroundFraction = 0.390;
        auxDeltaEn[j] = sumEnergy - neutrino -
          muonFraction * muonCut -
          hadronFraction * hadronCut;

        if ( j < fNBinsEnergyDeposit-1 )
          energyDepositSum += auxDeltaEn[j];
        else {
          const double groundEnergy =
            (1.-hadronGroundFraction)*hadronCut
            + hadronIoniz + muonIoniz
            + emIoniz+emCut + gamma;
          energyDepositSum += groundEnergy;
        }

        auxDeltaEn[j] /= fDx;

        ++j;
      }
    }

    if (line.find(dEdX_ProfileStr) == string::npos || (tokens[0] == "DEPTH" && tokens[1] == "GAMMAS" && i < fNBinsParticles)) {
      if (aux_flag)
        continue;

      float xdepth = 0.0;
      float chargedparticles = 0.0;
      float gammas, positrons, electrons, muplus, muminus, hadrons;
      float nuclei, cerenkov;
      sscanf(line.c_str(), "%f %f %f %f %f %f %f %f %f %f",
             &xdepth, &gammas, &positrons, &electrons, &muplus, &muminus,
             &hadrons, &chargedparticles, &nuclei, &cerenkov);

      if (xdepth > 0) {
        //BRD 17/2/05 CORSIKA depths are vertical assuming a flat
        //Earth.  So apply cosTheta correction here.
        // RU Wed Jun 13 14:46:44 CEST 2007
        // if SLANT was used in CORSIKA, don't do the fCosZenith correction
        if (!fIsSlantDepthProfile)
          xdepth /= fCosZenith;
        auxDepth[i] = xdepth;
        auxCharge[i] = chargedparticles;
        auxMuons[i] = muplus + muminus;
        auxGammas[i] = gammas;
        auxElectrons[i] = electrons + positrons;
        ++i;
      }
    }
  }

  float anmax;
  float ax0;
  float axmax;
  float achi;
  while (getline(*fLongDataFile.get(), line)) {
    if (line.find(" PARAMETERS         = ") != string::npos) {

      float aapar, abpar, acpar;
      sscanf(line.c_str(), " PARAMETERS         =   %f %f %f %f %f %f", &anmax,
             &ax0, &axmax, &aapar, &abpar, &acpar);
      getline(*fLongDataFile.get(), line);
      sscanf(line.c_str(), " CHI**2/DOF         = %f", &achi);

      const bool hasValidGHfit = (achi>0) && (achi*fNBinsParticles<1e15);

      double A = aapar * (g/cm2);
      double B = abpar;
      double C = acpar / (g/cm2);

      if (!fIsSlantDepthProfile) {
        A /= fCosZenith;
        B /= fCosZenith;
        C /= fCosZenith;
        axmax /= fCosZenith;
      }

      // corsika::GaisserHillas6Parameter gh;
      gh.SetXMax(axmax*(g/cm2), 0);
      gh.SetNMax(anmax, 0);
      gh.SetXZero(ax0*(g/cm2), 0);
      gh.SetChiSquare(fNBinsParticles*achi, fNBinsParticles);
      gh.SetA(A, 0.);
      gh.SetB(B, 0.);
      gh.SetC(C, 0.);

      // ostringstream info;
      // info << "Nmax = " << anmax << ", "
      //   "Xmax = " << axmax << ", "
      //   "X0 = " << ax0 << ", "
      //   "zenith = " << acos(fCosZenith)/deg
      //      << (fIsSlantDepthProfile ? " (SLANT DEPTH)" : " (VERTICAL DEPTH)");
      //INFO(info);

      // Adding GH information to simulated shower
      // if (!theShower.HasGHParameters() && hasValidGHfit)
      //   theShower.MakeGHParameters(gh);

      // if (!hasValidGHfit) {
      //   ostringstream err;
      //   err << "CORISKA shower with invalid GH fit: Xmax=" << axmax
      //       << " Nmax=" << anmax << " X0=" << ax0
      //       << " chi2/ndf=" << achi;
      //   ERROR(err);
      // }

      break;
    }
  }
  // end of read profile

  // -------------------------------------------------------------
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // -------------------------------------------------------------
  // CORSIKA writes into the last bin of the dEdX profile not only
  // the energy deposit in the atmosphere, but also the energy hitting
  // the ground (observation level). For vertical event this can be
  // remarkable, and should not be used for FD simulations !
  //
  // - Fix this by replacing the entries in the last bin by GH fitted
  //   extrapolations.
  // - Add one more bin in depth (extrapolation) to make sure
  //   that also for a different atmospheric profile model the
  //   shower will hit the ground, and does not just disappear
  //   somewhere in the middle of the atmosphere !

  // go three bins back, since CORSIKA sometimes has a funny second-last bin
  //const double normDepth_dedx = auxDepth_dE[fNBinsEnergyDeposit-3];
  const double normN_dedx = 1;
  // const double normN_dedx =
  //   theShower.HasGHParameters() ?
  //     theShower.GetGHParameters().Eval(normDepth_dedx * g/cm2) : 1;
  const double normdEdX = auxDeltaEn[i-3];
  /*
    cout << " fNBinsEnergyDeposit=" <<fNBinsEnergyDeposit
    << " normDepth_dedx=" << normDepth_dedx
    << " normN_dedx=" << normN_dedx
    << " normdEdX=" << normdEdX/1e6
    << endl;
  */

  // const double normDepth_part = auxDepth[fNBinsEnergyDeposit-1];
  const double normN_part = 1.;
  // const double normN_part =
  //   theShower.HasGHParameters() ?
  //     theShower.GetGHParameters().Eval(normDepth_part * g/cm2) : 1.;
  const double normCharge = auxCharge[i-1];
  const double normMuons = auxMuons[i-1];
  const double normGammas = auxGammas[i-1];
  const double normElectrons = auxElectrons[i-1];

  const double lastBinDepth_dEdX = auxDepth_dE[fNBinsEnergyDeposit-1];
  const double lastBinDepth_part = auxDepth[fNBinsParticles-1];

  /*
    for (int i=0; i<fNBinsEnergyDeposit; i++) {
    cout << " profile i=" << i
    << " depth=" << auxDepth_dE[i]
    << " dedx=" << auxDeltaEn[i]/1e6
    << " " << theShower.GetGHParameters().Eval(auxDepth_dE[i]*g/cm2)/1e6
    << " dedxGH=" << normdEdX/normN_dedx * theShower.GetGHParameters().Eval(auxDepth_dE[i]*g/cm2)/1e6
    << endl;
    }
  */

  // recalculate last dedx bins
  for (int iCorrect = 0; iCorrect < 2; ++iCorrect) {
    const int binCorrect = fNBinsEnergyDeposit - 2 + iCorrect;
    //const double binDepth = auxDepth_dE[binCorrect];
    //cout << " last-bin: i=" << binCorrect << " depth=" << binDepth << " " << auxDeltaEn[binCorrect]/1e6 << flush;
    auxDeltaEn[binCorrect] = normdEdX / normN_dedx;
    // auxDeltaEn[binCorrect] = normdEdX / normN_dedx *
    //   (theShower.HasGHParameters() ?
    //      theShower.GetGHParameters().Eval(binDepth*g/cm2) : 1);
    //cout << " last-bin (new): " << auxDeltaEn[binCorrect]/1e6 << endl;
  }

  // prolongate profiles by some bins
  const int nBinAdd = 2;
  for (int iBinAdd = 0; iBinAdd < nBinAdd; ++iBinAdd) {
    const double addDepth_dEdX = lastBinDepth_dEdX + fDx * (iBinAdd+1);
    const double addDepth_part = lastBinDepth_part + fDx * (iBinAdd+1);
    const double Nch_dedx = 1;
    // const double Nch_dedx =
    //   theShower.HasGHParameters() ?
    //     theShower.GetGHParameters().Eval(addDepth_dEdX * g/cm2) : 1;
    const double Nch_part = 1;
    // const double Nch_part =
    //   theShower.HasGHParameters() ?
    //     theShower.GetGHParameters().Eval(addDepth_part * g/cm2) : 1;
    const double adddEdX = normdEdX / normN_dedx * Nch_dedx;

    auxDepth_dE.push_back(addDepth_dEdX);
    auxDeltaEn.push_back(adddEdX);

    const double fac = Nch_part / normN_part;
    const double addCharge = normCharge * fac;
    const double addMuons = normMuons * fac;
    const double addGammas = normGammas * fac;
    const double addElectrons = normElectrons * fac;

    auxDepth.push_back(addDepth_part);
    auxCharge.push_back(addCharge);
    auxMuons.push_back(addMuons);
    auxGammas.push_back(addGammas);
    auxElectrons.push_back(addElectrons);

    /*
      cout << " iBinAdd=" << iBinAdd << " addDepth_dEdX=" << addDepth_dEdX
      << " addDepth_part=" << addDepth_part
      << " adddedx=" << adddEdX/1e6
      << " Nch_part=" << Nch_part
      << " addCharge=" << addCharge
      << endl;
    */
  }

  // -------------------------------------------------------------
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // -------------------------------------------------------------
  CorsikaLongProfile profile;
  profile.fChargeProfile = auxCharge;
  profile.fGammaProfile = auxGammas;
  profile.fElectronProfile = auxElectrons;
  profile.fMuonProfile = auxMuons;
  profile.fDepth = auxDepth;
  //profile.fdEdX = auxDeltaEn;
  //profile.fDepth_dE = auxDepth_dE;

  //profile.SetGaisserHillasParams(gh);
  profile.fCalorimetricEnergy = energyDepositSum;
  profile.fGaisserHillas = gh;
  return profile;
}
