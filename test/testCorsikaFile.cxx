const static char CVSId[] =
  "$Id$";

#include "corsika_test.h"

#include <corsika/Index.h>
#include <corsika/CorsikaUnits.h>
#include <corsika/CorsikaShowerFile.h>
#include <string>
#include <vector>
#include <iostream>

using namespace std;
using namespace corsika;

TEST_GROUP(CorsikaTest);

namespace testCorsikaFileNS {

  void test_header(string filename)
  {
    CorsikaShowerFile file(filename);
    ENSURE(file.IsOpen(), "File is open");
    corsika::Status status = file.FindEvent(1);

    ENSURE_EQUAL(status, corsika::eSuccess, "Event 1 found");
    ENSURE_EQUAL(file.GetCurrentShower().GetPrimary(), 2212);
    ENSURE_DISTANCE(file.GetCurrentShower().GetEnergy(), 100*TeV, 0.01*MeV, "Primary energy");
    ENSURE_EQUAL(file.GetCurrentShower().GetMuonNumber(), 1335);
    ENSURE_DISTANCE(file.GetCurrentShower().GetZenith(), 20*deg, 0.001*deg);
    ENSURE_DISTANCE(file.GetCurrentShower().GetAzimuth(), 175.381*deg, 0.01*deg);
    ENSURE_EQUAL(file.GetCurrentShower().GetMinRadiusCut(), 0);
    ENSURE_EQUAL(file.GetCurrentShower().GetShowerNumber(), 1);
    ENSURE_DISTANCE(file.GetCurrentShower().GetEMEnergyCutoff(), 3*MeV, 1e-4);
    ENSURE_DISTANCE(file.GetCurrentShower().GetMuonEnergyCutoff(), 300*MeV, 1e-4);
  }

  void test_particles(string filename)
  {
    CorsikaShowerFile file(filename);
    file.FindEvent(1);
    CorsikaShowerFileParticleIterator& particles = file.GetCurrentShower().GetParticleIt();
    int count = 0;
      
      boost::optional<CorsikaParticle> it = particles.NextParticle();
      
    //ShowerParticleList::iterator it = particles.begin();
    ENSURE_EQUAL(it->PDGCode(), CorsikaParticle::eMuon);
    ENSURE_DISTANCE(it->fTorZ, 183.053619385, 1e-8);
    //for (; it != particles.end(); ++it) {
      while(it)
      {
          it->GetName();
          const int pdg = it->PDGCode();
          if ((pdg == CorsikaParticle::eMuon || pdg == CorsikaParticle::eMuon)) {
              it->KineticEnergy();
          }
          ++count;
          it = particles.NextParticle();
      }
    ENSURE_EQUAL(count, 181992);
  }

}

using namespace testCorsikaFileNS;

TEST(testCorsikaFile)
{
  string dir(CORSIKA_TEST_DIR);
  vector<string> filenames;
  filenames.push_back("/DAT000002-32");
  // commented out until I add dumb random access back
  filenames.push_back("/DAT000002-32.bz2");
  filenames.push_back("/DAT000002-32.gz");

  CorsikaShowerFile file;
  ENSURE(!file.IsOpen());

  for (unsigned int i = 0; i != filenames.size(); ++i) {
    std::cout << "testing header " << filenames[i] << std::endl;
    test_header(dir + filenames[i]);
    std::cout << "testing particles " << filenames[i] << std::endl;
    test_particles(dir + filenames[i]);
  }
}




// Configure (x)emacs for this file ...
// Local Variables:
// mode:c++
// End:
