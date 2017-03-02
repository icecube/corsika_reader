/**
   \file
   Implementation of the Corsika shower reader

   \author Lukas Nellen
   \version $Id$
   \date 29 Jan 2004
*/

#include <corsika/Constants.h>
#include <corsika/ShowerFile.h>
#include <corsika/ShowerParticleStream.h>
#include <corsika/IOException.h>
#include <corsika/Block.h>
#include <corsika/Shower.h>
#include <corsika/LongFile.h>
#include <corsika/LongProfile.h>
#include <corsika/particle/ParticleList.h>

#include <sstream>
#include <string>
#include <cmath>
#include <iostream>

#include <boost/tokenizer.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace corsika;

namespace bfs = boost::filesystem;

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


  bfs::path p(theFileName);
  p.replace_extension(".long");
  fLongFile = p.string();

  p = bfs::path(fLongFile);
  if ( !(bfs::exists(p) && bfs::is_regular_file(p)) ) {
    fLongFile = "";
  }

  fRawStream = RawStream::Create(theFileName);
  fIsThinned = fRawStream->IsThinned();
  if (scan && fRawStream->IsSeekable()) {
      fIndex.Scan(*fRawStream, false);
    fFileScanned = true;
  }
  else if (!fRawStream->IsSeekable()){
    ostringstream msg;
    msg << "File " << theFileName << " does not support random access. This class can not handle it";
    throw CorsikaIOException(msg.str());
  }
  if (fIsThinned) {
    ReadRunHeader<Thinned>();
  }
  else {
    ReadRunHeader<NotThinned>();
  }
}

void
CorsikaShowerFile::Close()
{
  fRunHeader = corsika::RunHeader();
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

  Block<Thinning> headerBlock;
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
  const EventHeader& header = headerBlock.AsEventHeader;

  fRawStream->SeekTo(fIndex.eventTrailers[fCurrentPosition]);

  Block<Thinning> trailerBlock;
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

  const EventTrailer& trailer = trailerBlock.AsEventTrailer;

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


  ShowerParticleStream* particleIterator =
    new ShowerParticleStream(fRawStream,
                                          fIndex.eventHeaders[fCurrentPosition] + 1,
                                          timeShift,
                                          fObservationLevel,
                                          true); // last one is keepMuProd
  fCurrentShower = CorsikaShower(header, trailer, particleIterator);

  if (fIndex.longBlocks.size() > 0)
    ReadLongBlocks<Thinning>();
  else if ( fLongFile != "" )
    ReadLongFile();

  ++fCurrentPosition;

  return eSuccess;
}


template <class Thinning>
Status
CorsikaShowerFile::ReadRunHeader()
{
  Block<Thinning> runHeaderBlock;
  if (!fRawStream->GetNextBlock(runHeaderBlock)) {
    FATAL("Cannot read CORSIKA run header");
    return eFail;
  }
  if (!runHeaderBlock.IsRunHeader()) {
    FATAL("First block is not a run header");
    return eFail;
  }
  fRunHeader = runHeaderBlock.AsRunHeader;
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
    return Read<Thinned>();
  }
  return Read<NotThinned>();
}



size_t CorsikaShowerFile::GetNEvents()
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
      fIndex.Scan(*fRawStream, false);
    fFileScanned = true;
  }
  return fIndex.IDToPosition.size();
}


Status
CorsikaShowerFile::ReadLongFile()
{
  //cout << "reading long file " << fLongFile << endl;
  if (!fCorsikaLongFile)
    fCorsikaLongFile.reset(new CorsikaLongFile(fLongFile, GetCurrentShower().GetZenith()));

  if (fCorsikaLongFile->size() >= fCurrentPosition) {
    CorsikaLongProfile p = fCorsikaLongFile->GetProfile(fCurrentPosition);
    fCurrentShower.fdEdX = p.fdEdX;
    fCurrentShower.fChargeProfile = p.fChargeProfile;
    fCurrentShower.fGammaProfile = p.fGammaProfile;
    fCurrentShower.fElectronProfile = p.fElectronProfile;
    fCurrentShower.fPositronProfile = p.fPositronProfile;
    fCurrentShower.fMuonProfile = p.fMuonProfile;
    fCurrentShower.fAntiMuonProfile = p.fAntiMuonProfile;
    fCurrentShower.fHadronProfile = p.fHadronProfile;
    fCurrentShower.fNucleiProfile = p.fNucleiProfile;
    fCurrentShower.fCherenkovProfile = p.fCherenkovProfile;
    fCurrentShower.fDepth_dE = p.fDepth_dE;
    fCurrentShower.fDepth = p.fDepth;

    fCurrentShower.SetGaisserHillasParams(p.fGaisserHillas);
    fCurrentShower.SetCalorimetricEnergy(p.fCalorimetricEnergy);
  }
  else {
    fCurrentShower.fdEdX.clear();
    fCurrentShower.fChargeProfile.clear();
    fCurrentShower.fGammaProfile.clear();
    fCurrentShower.fElectronProfile.clear();
    fCurrentShower.fPositronProfile.clear();
    fCurrentShower.fMuonProfile.clear();
    fCurrentShower.fAntiMuonProfile.clear();
    fCurrentShower.fHadronProfile.clear();
    fCurrentShower.fNucleiProfile.clear();
    fCurrentShower.fCherenkovProfile.clear();
    fCurrentShower.fDepth_dE.clear();
    fCurrentShower.fDepth.clear();

    fCurrentShower.SetCalorimetricEnergy(0);

    GaisserHillasParameter gh;
    fCurrentShower.SetGaisserHillasParams(gh);
  }
  return eSuccess;
}


template <class Thinning>
Status
CorsikaShowerFile::ReadLongBlocks()
{
  fRawStream->SeekTo(fIndex.longBlocks[fCurrentPosition]);

  Block<Thinning> block;
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
  const LongitudinalBlock& longBlock = block.AsLongitudinalBlock;


  vector<double> auxDeltaEn;
  vector<double> auxCharge;
  vector<double> auxGammas;
  vector<double> auxElectrons;
  vector<double> auxPositrons;
  vector<double> auxMuons;
  vector<double> auxAntiMuons;
  vector<double> auxHadrons;
  vector<double> auxNuclei;
  vector<double> auxCherenkov;
  vector<double> auxDepth_dE;
  vector<double> auxDepth;

  const int nBlocks = int(longBlock.fStepsAndBlocks)%100;
  //cout << int(longBlock.fStepsAndBlocks/100)<< " steps in " << nBlocks << " blocks" << endl;
  int i = 0;

  for (; i != kLongEntriesPerBlock; ++i) {
    if (i && !longBlock.fEntries[i].fDepth)
      break;
    auxDeltaEn.push_back(0);
    auxCharge.push_back(longBlock.fEntries[i].fCharged);
    auxGammas.push_back(longBlock.fEntries[i].fGamma);
    auxElectrons.push_back(longBlock.fEntries[i].fEminus);
    auxPositrons.push_back(longBlock.fEntries[i].fEplus);
    auxMuons.push_back(longBlock.fEntries[i].fMuMinus);
    auxAntiMuons.push_back(longBlock.fEntries[i].fMuPlus);
    auxHadrons.push_back(longBlock.fEntries[i].fNuclei);
    auxNuclei.push_back(longBlock.fEntries[i].fHadron);
    auxCherenkov.push_back(longBlock.fEntries[i].fCherenkov);
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
    const LongitudinalBlock& longBlock = block.AsLongitudinalBlock;
    for (int j = 0; j != kLongEntriesPerBlock; ++j,++i) {
      if (i && !longBlock.fEntries[j].fDepth)
        break;
      auxDeltaEn.push_back(0);
      auxCharge.push_back(longBlock.fEntries[j].fCharged);
      auxGammas.push_back(longBlock.fEntries[j].fGamma);
      auxElectrons.push_back(longBlock.fEntries[j].fEminus);
      auxPositrons.push_back(longBlock.fEntries[i].fEplus);
      auxMuons.push_back(longBlock.fEntries[i].fMuMinus);
      auxAntiMuons.push_back(longBlock.fEntries[i].fMuPlus);
      auxHadrons.push_back(longBlock.fEntries[i].fNuclei);
      auxNuclei.push_back(longBlock.fEntries[i].fHadron);
      auxCherenkov.push_back(longBlock.fEntries[i].fCherenkov);
      auxDepth.push_back(longBlock.fEntries[j].fDepth);
      auxDepth_dE.push_back(longBlock.fEntries[j].fDepth);
    }
  }
  //cout << i << " entries read" << endl;

  fCurrentShower.fdEdX = auxDeltaEn;
  fCurrentShower.fChargeProfile = auxCharge;
  fCurrentShower.fGammaProfile = auxGammas;
  fCurrentShower.fElectronProfile = auxElectrons;
  fCurrentShower.fPositronProfile = auxPositrons;
  fCurrentShower.fMuonProfile = auxMuons;
  fCurrentShower.fAntiMuonProfile = auxAntiMuons;
  fCurrentShower.fHadronProfile = auxHadrons;
  fCurrentShower.fNucleiProfile = auxNuclei;
  fCurrentShower.fCherenkovProfile = auxCherenkov;
  fCurrentShower.fDepth_dE = auxDepth_dE;
  fCurrentShower.fDepth = auxDepth;

  // fCurrentShower.SetGaisserHillasParams(gh);
  // fCurrentShower.SetCalorimetricEnergy(energyDepositSum);

  return eSuccess;
}
