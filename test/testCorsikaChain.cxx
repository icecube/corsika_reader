const static char CVSId[] =
  "$Id$";

#include "corsika_test.h"

#include <corsika/Index.h>
#include <corsika/CorsikaUnits.h>
#include <corsika/CorsikaShowerFile.h>
#include <corsika/ShowerParticleList.h>
#include <corsika/icecube/CorsikaChain.h>
#include <string>
#include <vector>
#include <iostream>

using namespace std;
using namespace corsika;

TEST_GROUP(CorsikaTest);


TEST(testCorsikaChain)
{
  string dir(CORSIKA_TEST_DIR);
  vector<string> filenames;
  CorsikaChain empty_chain(filenames);
  ENSURE(empty_chain.end()==empty_chain.begin());

  filenames.push_back(dir+"/DAT000002-32");
  filenames.push_back(dir+"/DAT000002-32.bz2");
  filenames.push_back(dir+"/DAT000002-32.gz");



  CorsikaChain chain(filenames);
  int n = 0;

  CorsikaChain::iterator it = chain.begin();
  ENSURE(it!=chain.end());
  for (; it != chain.end(); ++it) {
    ENSURE_EQUAL(it->GetPrimary(), 2212);
    ENSURE_DISTANCE(it->GetEnergy(), 100*TeV, 0.01*MeV, "Primary energy");
    ++n;
    int count = 0;
    const ShowerParticleList& particles = it->GetParticles();
    for (ShowerParticleList::iterator pIt = particles.begin(); pIt != particles.end(); ++pIt) {
      ++count;
    }
    ENSURE_EQUAL(count, 181992);
  }
  ENSURE(it==chain.end());
  ENSURE_EQUAL(n, 3);
}



// Configure (x)emacs for this file ...
// Local Variables:
// mode:c++
// End:
