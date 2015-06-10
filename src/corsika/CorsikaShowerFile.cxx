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
#include <corsika/CorsikaShowerFile.h>
#include <corsika/CorsikaShowerFileParticleIterator.h>
#include <corsika/CorsikaIOException.h>
#include <corsika/RawCorsikaFile.h>
#include <corsika/CorsikaBlock.h>
#include <corsika/CorsikaShower.h>
#include <corsika/particle/ParticleList.h>

#include <sstream>
#include <string>
#include <cmath>
#include <iostream>

#include <boost/tokenizer.hpp>

using corsika::kSpeedOfLight;
using corsika::kEarthRadius;
using namespace std;
using namespace corsika;

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


CorsikaShowerFile::~CorsikaShowerFile()
{
}


CorsikaShowerFile::CorsikaShowerFile() :
  fRawStream(),
  fCurrentPosition(0),
  fObservationLevel(1),
  fIsThinned(true),
  fFileScanned(false)
{
}


CorsikaShowerFile::CorsikaShowerFile(const std::string& theFileName,
                                     const bool requireParticleFile) :
  fRawStream(),
  fCurrentPosition(0),
  fObservationLevel(1),
  fIsThinned(true),
  fFileScanned(false)
{
  // only call Open() if the particle file is required (default behaviour)
  if (requireParticleFile) {
    Open(theFileName);
  }
}


void
CorsikaShowerFile::Open(const std::string& theFileName, bool scan)
{
  Close();

  // Compute the name for the long file
  string file = theFileName;

  if (file.find(".part") == string::npos)
    fLongFile = file + ".long";
  else
    fLongFile = file.replace(file.find(".part"), 5, ".long");

  // ostringstream msg;
  // msg << "CORSIKA longitudinal file: " << fLongFile;
  // INFO(msg);

  fRawStream = Corsika::RawStreamFactory::Create(theFileName);
  fIsThinned = fRawStream->IsThinned();
  if (scan && fRawStream->IsSeekable()) {
    fIndex = fRawStream->Scan(false);
    fFileScanned = true;
  }
  else if (!fRawStream->IsSeekable()){
    ostringstream msg;
    msg << "File " << theFileName << " does not support random access. This class can not handle it";
    throw CorsikaIOException(msg.str());
  }
}


bool
CorsikaShowerFile::IsValid(const std::string& theFileName)
{
  CorsikaShowerFile file;
  file.Open(theFileName, false);
  return file.IsValid();
}

bool
CorsikaShowerFile::IsValid()
{
  return fRawStream->IsValid();
}


void
CorsikaShowerFile::Close()
{
  fRawStream.reset();

  fIndex.eventHeaders.clear();
  fIndex.eventTrailers.clear();
  fIndex.longBlocks.clear();
  fIndex.IDToPosition.clear();
}


template <class Thinning>
Status
CorsikaShowerFile::Read()
{
  if (!fRawStream || fCurrentPosition >= fIndex.eventHeaders.size())
    return eEOF;

  fRawStream->SeekTo(fIndex.eventHeaders[fCurrentPosition]);

  Corsika::Block<Thinning> headerBlock;
  if (!fRawStream->GetNextBlock(headerBlock)) {
    ostringstream err;
    err << "Cannot read CORSIKA shower header for position "
        << fCurrentPosition;
    FATAL(err);
    return eFail;
  }

  if (!headerBlock.IsEventHeader()) {
    ostringstream err;
    err << "First block at position " << fCurrentPosition
        << " is not event header";
    FATAL(err);
    return eFail;
  }
  const typename Corsika::EventHeader& header = headerBlock.AsEventHeader();

  fRawStream->SeekTo(fIndex.eventTrailers[fCurrentPosition]);

  Corsika::Block<Thinning> trailerBlock;
  if (!fRawStream->GetNextBlock(trailerBlock)) {
    ostringstream err;
    err << "Cannot read CORSIKA shower trailer for position "
        << fCurrentPosition;
    FATAL(err);
    return eFail;
  }
  if (!trailerBlock.IsEventTrailer()) {
    ostringstream err;
    err << "Block at position " << fCurrentPosition
        << " is not event trailer";
    FATAL(err);
    return eFail;
  }

  const typename Corsika::EventTrailer& trailer = trailerBlock.AsEventTrailer();

  if (fObservationLevel > header.fObservationLevels) {
    ostringstream info;
    info << "The requested observation level: " << fObservationLevel
         << " does not exist (max obs. level: "
         << header.fObservationLevels << "), "
         << "switching to level 1.";
    fObservationLevel = 1;
    INFO(info);
  }

  // Corsika starts at the top of the atmosphere, not
  const float heightObsLevel =
    header.fObservationHeight[int(header.fObservationLevels) - 1]; // in cm
  const float heightFirstInt = abs(header.fZFirst); // in cm

  double hReference;
  const double hAtmBoundary = (header.fStartingHeight>0? header.fStartingHeight: 112.8292*1e5); // in cm

  // for the SLANT and CURVED options, clock starts at the margin of
  // the atmosphere. This is indicated by fZFirst < 0
  if (header.fZFirst < 0.) {
    hReference = hAtmBoundary;
  } else {
      hReference = heightFirstInt;
  }

  double timeShift = 0;

  const double Zenith = header.fTheta;
  const double cosZenith = cos(Zenith);

  if (header.fFlagCurved) {

    INFO("CURVED version");

    if (ParticleList::CorsikaToPDG(int(header.fParticleId)) == CorsikaParticle::ePhoton)
      hReference = heightFirstInt;

    timeShift = (pow((kEarthRadius + heightObsLevel)*cosZenith, 2) +
                 pow(hReference - heightObsLevel, 2) +
                 2*(kEarthRadius + heightObsLevel)*(hReference - heightObsLevel));
    timeShift = sqrt(timeShift);
    timeShift -= (kEarthRadius + heightObsLevel)*cosZenith;
    timeShift /= kSpeedOfLight;

    ostringstream info;
    info << "TimeShift to core: " << timeShift/1e9; // output in ns
    INFO(info);

  } else
    timeShift = (hReference - heightObsLevel) / (cosZenith * kSpeedOfLight);


  CorsikaShowerFileParticleIterator* particleIterator =
    new CorsikaShowerFileParticleIterator(*fRawStream,
                                          headerBlock.CopyBlock().AsEventHeader(),
                                          fIndex.eventHeaders[fCurrentPosition] + 1,
                                          timeShift,
                                          fObservationLevel,
                                          fIsThinned,
                                          true); // last one is keepMuProd
  fCurrentShower = CorsikaShower(header, trailer, particleIterator);

  if (fIndex.longBlocks.size() > 0)
    ReadLongBlocks<Thinning>();
  else
    ReadLongFile();

  ++fCurrentPosition;

  return eSuccess;
}


Status
CorsikaShowerFile::FindEvent(const unsigned int eventId)
{
  if (!IsOpen())
    return eEOF;
  const IdToPositionMap::const_iterator iter = fIndex.IDToPosition.find(eventId);
  if (iter == fIndex.IDToPosition.end() || iter->second >= fIndex.eventHeaders.size())
    return eFail;

  fCurrentPosition = iter->second;
  if (fIsThinned) {
    return Read<Corsika::Thinned>();
  }
  return Read<Corsika::NotThinned>();
}


int
CorsikaShowerFile::GetNEvents()
{
  if (!fRawStream) {
    const string msg = "Cannot request number of events from closed file";
    ERROR(msg);
    throw CorsikaIOException(msg);
  }

  if (!fRawStream->IsSeekable()) {
    const string msg = "Cannot request number of events for this file. It is not seekable.";
    ERROR(msg);
    throw CorsikaIOException(msg);
  }
  if (!fFileScanned) {
    fIndex = fRawStream->Scan(false);
    fFileScanned = true;
  }
  return fIndex.IDToPosition.size();
}


Status
CorsikaShowerFile::ReadLongFile()
{
  fCurrentShower.fdEdX.clear();
  fCurrentShower.fChargeProfile.clear();
  fCurrentShower.fGammaProfile.clear();
  fCurrentShower.fElectronProfile.clear();
  fCurrentShower.fMuonProfile.clear();
  fCurrentShower.fDepth_dE.clear();
  fCurrentShower.fDepth.clear();

  fCurrentShower.SetCalorimetricEnergy(0);

  corsika::GaisserHillasParameter gh;
  fCurrentShower.SetGaisserHillasParams(gh);

  static const double g = 1;
  static const double cm = 1;
  static const double cm2 = cm*cm;
  static const double deg = 3.14159/180;

  const double cosZenith = cos(GetCurrentShower().GetZenith());

  // Read CORSIKA profile if available
  ifstream longDataFile;

  longDataFile.open(fLongFile.c_str());
  if (!longDataFile.is_open()) {
    return eSuccess;
  }
  bool aux_flag = false;
  string line;
  string startstr0 = " LONGITUDINAL DISTRIBUTION IN";
  // char endstr0[256] = " DEPTH     GAMMAS   POSITRONS";
  // if (version >= 6.98)
  //   strcpy(endstr0, " DEPTH       GAMMAS   POSITRONS");
  string endstr1 = " LONGITUDINAL ENERGY DEPOSIT";
  // char endstr2[256] = " DEPTH       GAMMA    EM IONIZ     EM CUT";
  // if (version >= 6.98)
  //   strcpy(endstr2, " DEPTH         GAMMA    EM IONIZ     EM CUT");
  string endstr3 = " FIT OF THE HILLAS CURVE";
  int i = 0;
  int j = 0;
  int nBinsParticles = 0;
  int nBinsEnergyDeposit = 0;
  double energyDepositSum = 0.;
  float dX = 0; // dEdX depth step width [g/cm2]

  float nbr = 0.;
  float maxi = 0.;

  // scan for number of bins in depth of particle number profiles,
  // energy deposit profiles and dX the step-width of the dEdX profile
  bool HaveParticleProfile = false;
  bool HaveEnergyDepositProfile = false;
  bool isSlantDepthProfile = false;
  while (getline(longDataFile, line) &&
         (!HaveParticleProfile || !HaveEnergyDepositProfile)) {

    // LONGITUDINAL DISTRIBUTION IN 174 VERTICAL STEPS OF 5. G/CM**2 FOR SHOWER 25250
    if (line.find(startstr0) != string::npos) {

      HaveParticleProfile = true;
      char s1[50], s2[50], s3[50], isVertOrSlant[50];
      sscanf(line.c_str(), "%s %s %s %i %s", s1, s2, s3, &nBinsParticles, isVertOrSlant);
      string testStr(isVertOrSlant);
      if (testStr == "VERTICAL")
        isSlantDepthProfile = false;
      else if (testStr == "SLANT")
        isSlantDepthProfile = true;
      else {
        ostringstream err;
        err << "Corsika longitunal file \"" << fLongFile << "\" is invalid:\n"
          "line: \"" << line << "\" "
          "contains invalid string at                ^^^^^^^\n"
          "which is neither VERTICAL nor SLANT !";
        ERROR(err);
        return eFail;
      }
    }

    // LONGITUDINAL ENERGY DEPOSIT IN 174 VERTICAL STEPS OF 5. G/CM**2 FOR SHOWER 25250
    if (line.find(endstr1) != string::npos) {

      HaveEnergyDepositProfile = true;
      char s1[50], s2[50], s3[50], s4[50], isVertOrSlant[50], s5 [50], s6 [50];
      sscanf(line.c_str(),
             "%s %s %s %s %i %s %s %s %f",
             s1, s2, s3, s4, &nBinsEnergyDeposit, isVertOrSlant, s5, s6, &dX);
      string testStr(isVertOrSlant);
      if (testStr == "VERTICAL") {
        isSlantDepthProfile = false;
        dX /= cosZenith;
      } else if (testStr == "SLANT")
        isSlantDepthProfile = true;
      else {
        ostringstream err;
        err << "Corsika longitunal file \"" << fLongFile << "\" is invalid:\n"
          "line: \"" << line << "\" "
          "contains invalid string at                ^^^^^^^\n"
          " which is neither VERTICAL nor SLANT !";
        ERROR(err);
        return eFail;
      }
    }
  } // --  end scan file  ---

  getline(longDataFile, line);
  getline(longDataFile, line);
  sscanf(line.c_str(), "%f", &nbr);

  vector<double> auxDepth(nBinsParticles);
  vector<double> auxCharge(nBinsParticles);
  vector<double> auxMuons(nBinsParticles);
  vector<double> auxGammas(nBinsParticles);
  vector<double> auxElectrons(nBinsParticles);

  vector<double> auxDepth_dE (nBinsEnergyDeposit);
  vector<double> auxDeltaEn (nBinsEnergyDeposit);
  longDataFile.close();

  longDataFile.open(fLongFile.c_str());

  int sh = 0;
  int findShower = 0;
  findShower = fCurrentPosition;
  bool find = true;
  while (sh != findShower) {
    if (getline(longDataFile, line)) {
      if (line.find(startstr0) != string::npos)
        ++sh;
    } else {
      sh = findShower;
      find = false;
    }
  }
  if (!find) {
    ostringstream msg;
    msg << "Cannot find shower " << findShower << " in file.";
    ERROR(msg);
    return eFail;
  } /*else
      cout << "Current Position " << fCurrentPosition << endl;*/

  vector<string> tokens;
  while (getline(longDataFile, line)) {
    SplitLine(line, tokens);
    if (line.find(startstr0) != string::npos) {
      getline(longDataFile, line);
      SplitLine(line, tokens);
    }
    if (line.find(endstr1) != string::npos || aux_flag) {
      if (line.find(endstr1) != string::npos) {
        getline(longDataFile, line);
        SplitLine(line, tokens);
      }
      if (tokens.size() > 1 && tokens[0] == string("DEPTH") && tokens[1] == string("GAMMA")) {
        aux_flag = true;
        getline(longDataFile, line);
        SplitLine(line, tokens);
      }
      if (line.find(endstr3) != string::npos)
        break;
      if (aux_flag && j < nBinsParticles) {

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

        if (!isSlantDepthProfile)
          xdepth /= cosZenith;

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

        if ( j < nBinsEnergyDeposit-1 )
          energyDepositSum += auxDeltaEn[j];
        else {
          const double groundEnergy =
            (1.-hadronGroundFraction)*hadronCut
            + hadronIoniz + muonIoniz
            + emIoniz+emCut + gamma;
          energyDepositSum += groundEnergy;
        }

        auxDeltaEn[j] /= dX;

        ++j;
      }
    }

    if (line.find(endstr1) == string::npos || (tokens[0] == "DEPTH" && tokens[1] == "GAMMAS" && i < nBinsParticles)) {
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
        // if SLANT was used in CORSIKA, don't do the cosZenith correction
        if (!isSlantDepthProfile)
          xdepth /= cosZenith;
        auxDepth[i] = xdepth;
        auxCharge[i] = chargedparticles;
        auxMuons[i] = muplus + muminus;
        auxGammas[i] = gammas;
        auxElectrons[i] = electrons + positrons;
        if (chargedparticles > maxi) {
          maxi = chargedparticles;
        }
        ++i;
      }
    }
  }

  float anmax;
  float ax0;
  float axmax;
  float achi;
  while (getline(longDataFile, line)) {
    if (line.find(" PARAMETERS         = ") != string::npos) {

      float aapar, abpar, acpar;
      sscanf(line.c_str(), " PARAMETERS         =   %f %f %f %f %f %f", &anmax,
             &ax0, &axmax, &aapar, &abpar, &acpar);
      getline(longDataFile, line);
      sscanf(line.c_str(), " CHI**2/DOF         = %f", &achi);

      const bool hasValidGHfit = (achi>0) && (achi*nBinsParticles<1e15);

      double A = aapar * (g/cm2);
      double B = abpar;
      double C = acpar / (g/cm2);

      if (!isSlantDepthProfile) {
        A /= cosZenith;
        B /= cosZenith;
        C /= cosZenith;
        axmax /= cosZenith;
      }

      // corsika::GaisserHillas6Parameter gh;
      gh.SetXMax(axmax*(g/cm2), 0);
      gh.SetNMax(anmax, 0);
      gh.SetXZero(ax0*(g/cm2), 0);
      gh.SetChiSquare(nBinsParticles*achi, nBinsParticles);
      gh.SetA(A, 0.);
      gh.SetB(B, 0.);
      gh.SetC(C, 0.);

      ostringstream info;
      info << "Nmax = " << anmax << ", "
        "Xmax = " << axmax << ", "
        "X0 = " << ax0 << ", "
        "zenith = " << acos(cosZenith)/deg
           << (isSlantDepthProfile ? " (SLANT DEPTH)" : " (VERTICAL DEPTH)");
      INFO(info);

      // Adding GH information to simulated shower
      // if (!theShower.HasGHParameters() && hasValidGHfit)
      //   theShower.MakeGHParameters(gh);

      if (!hasValidGHfit) {
        ostringstream err;
        err << "CORISKA shower with invalid GH fit: Xmax=" << axmax
            << " Nmax=" << anmax << " X0=" << ax0
            << " chi2/ndf=" << achi;
        ERROR(err);
      }

      break;
    }
  }
  // end of read profile

  longDataFile.close();

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
  //const double normDepth_dedx = auxDepth_dE[nBinsEnergyDeposit-3];
  const double normN_dedx = 1;
  // const double normN_dedx =
  //   theShower.HasGHParameters() ?
  //     theShower.GetGHParameters().Eval(normDepth_dedx * g/cm2) : 1;
  const double normdEdX = auxDeltaEn[i-3];
  /*
    cout << " nBinsEnergyDeposit=" <<nBinsEnergyDeposit
    << " normDepth_dedx=" << normDepth_dedx
    << " normN_dedx=" << normN_dedx
    << " normdEdX=" << normdEdX/1e6
    << endl;
  */

  // const double normDepth_part = auxDepth[nBinsEnergyDeposit-1];
  const double normN_part = 1.;
  // const double normN_part =
  //   theShower.HasGHParameters() ?
  //     theShower.GetGHParameters().Eval(normDepth_part * g/cm2) : 1.;
  const double normCharge = auxCharge[i-1];
  const double normMuons = auxMuons[i-1];
  const double normGammas = auxGammas[i-1];
  const double normElectrons = auxElectrons[i-1];

  const double lastBinDepth_dEdX = auxDepth_dE[nBinsEnergyDeposit-1];
  const double lastBinDepth_part = auxDepth[nBinsParticles-1];

  /*
    for (int i=0; i<nBinsEnergyDeposit; i++) {
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
    const int binCorrect = nBinsEnergyDeposit - 2 + iCorrect;
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
    const double addDepth_dEdX = lastBinDepth_dEdX + dX * (iBinAdd+1);
    const double addDepth_part = lastBinDepth_part + dX * (iBinAdd+1);
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

  fCurrentShower.fdEdX = auxDeltaEn;
  fCurrentShower.fChargeProfile = auxCharge;
  fCurrentShower.fGammaProfile = auxGammas;
  fCurrentShower.fElectronProfile = auxElectrons;
  fCurrentShower.fMuonProfile = auxMuons;
  fCurrentShower.fDepth_dE = auxDepth_dE;
  fCurrentShower.fDepth = auxDepth;

  fCurrentShower.SetGaisserHillasParams(gh);
  fCurrentShower.SetCalorimetricEnergy(energyDepositSum);

  return eSuccess;
}


template <class Thinning>
Status
CorsikaShowerFile::ReadLongBlocks()
{
  fRawStream->SeekTo(fIndex.longBlocks[fCurrentPosition]);

  Corsika::Block<Thinning> block;
  if (!fRawStream->GetNextBlock(block)) {
    ostringstream err;
    err << "Cannot read CORSIKA long block at position "
        << fCurrentPosition;
    FATAL(err);
    return eFail;
  }

  if (!block.IsLongitudinal()) {
    ostringstream err;
    err << "Block at position " << fCurrentPosition
        << " is not longitudinal";
    FATAL(err);
    return eFail;
  }
  const typename Corsika::Block<Thinning>::LongitudinalBlock& longBlock = block.AsLongitudinalBlock();


  vector<double> auxDeltaEn;
  vector<double> auxCharge;
  vector<double> auxGammas;
  vector<double> auxElectrons;
  vector<double> auxMuons;
  vector<double> auxDepth_dE;
  vector<double> auxDepth;

  const int nBlocks = int(longBlock.fStepsAndBlocks)%100;
  //cout << int(longBlock.fStepsAndBlocks/100)<< " steps in " << nBlocks << " blocks" << endl;
  int i = 0;

  for (; i != Corsika::Block<Thinning>::kLongEntriesPerBlock; ++i) {
    if (i && !longBlock.fEntries[i].fDepth)
      break;
    auxDeltaEn.push_back(0);
    auxCharge.push_back(longBlock.fEntries[i].fCharged);
    auxGammas.push_back(longBlock.fEntries[i].fGamma);
    auxElectrons.push_back(longBlock.fEntries[i].fEplus + longBlock.fEntries[i].fEminus);
    auxMuons.push_back(longBlock.fEntries[i].fMuPlus + longBlock.fEntries[i].fMuMinus);
    auxDepth.push_back(longBlock.fEntries[i].fDepth);
    auxDepth_dE.push_back(longBlock.fEntries[i].fDepth);
  }

  for (int b = 1; b < nBlocks; ++b) {
    if (!fRawStream->GetNextBlock(block)) {
      ostringstream err;
      err << "Cannot read CORSIKA long block #" << b << "at position "
          << fCurrentPosition;
      FATAL(err);
      return eFail;
    }
    const typename Corsika::Block<Thinning>::LongitudinalBlock& longBlock = block.AsLongitudinalBlock();
    for (int j = 0; j != Corsika::Block<Thinning>::kLongEntriesPerBlock; ++j,++i) {
      if (i && !longBlock.fEntries[j].fDepth)
        break;
      auxDeltaEn.push_back(0);
      auxCharge.push_back(longBlock.fEntries[j].fCharged);
      auxGammas.push_back(longBlock.fEntries[j].fGamma);
      auxElectrons.push_back(longBlock.fEntries[j].fEplus + longBlock.fEntries[j].fEminus);
      auxMuons.push_back(longBlock.fEntries[j].fMuPlus + longBlock.fEntries[j].fMuMinus);
      auxDepth.push_back(longBlock.fEntries[j].fDepth);
      auxDepth_dE.push_back(longBlock.fEntries[j].fDepth);
    }
  }
  //cout << i << " entries read" << endl;

  fCurrentShower.fdEdX = auxDeltaEn;
  fCurrentShower.fChargeProfile = auxCharge;
  fCurrentShower.fGammaProfile = auxGammas;
  fCurrentShower.fElectronProfile = auxElectrons;
  fCurrentShower.fMuonProfile = auxMuons;
  fCurrentShower.fDepth_dE = auxDepth_dE;
  fCurrentShower.fDepth = auxDepth;

  // fCurrentShower.SetGaisserHillasParams(gh);
  // fCurrentShower.SetCalorimetricEnergy(energyDepositSum);

  return eSuccess;
}


bool
CorsikaShowerFile::SplitLine(string l, vector<string>& vec, string s)
{
  boost::char_separator<char> sep(s.c_str());
  mytok tok(l,sep);
  vec.clear();
  for (mytok::const_iterator titer=tok.begin();titer!=tok.end();++titer) {
    vec.push_back(*titer);
  }

  return true;
}


// Configure (x)emacs for this file ...
// Local Variables:
// mode: c++
// compile-command: "make -C .. -k"
// End:
