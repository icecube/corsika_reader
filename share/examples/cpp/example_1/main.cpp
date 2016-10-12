#include <corsika/Index.h>
#include <corsika/CorsikaUnits.h>
#include <corsika/CorsikaShowerFile.h>
#include <corsika/ShowerParticleList.h>
#include <string>
#include <vector>
#include <iostream>

using namespace std;
using namespace corsika;

int main(int argc, char* argv[])
{
  if (argc != 2) {
    cerr << "Need to give one and only one file name as argument" << endl;
    exit(1);
  }

  CorsikaShowerFile file(argv[1]);
  if(!file.IsOpen()) {
    cerr << "Failed to open file " << argv[1] << endl;
    exit(1);
  }
  corsika::Status status = file.FindEvent(1);
  if (status != corsika::eSuccess) {
    cerr << "Could not find first event in file " << argv[1] << endl;
    exit(1);
  }

  cout << "Shower " << file.GetCurrentShower().GetShowerNumber() << ":\n"
       << "  primary: " << file.GetCurrentShower().GetPrimary() << "\n"
       << "  Energy:  " << file.GetCurrentShower().GetEnergy()/PeV << " PeV\n"
       << "  zenith:  " << file.GetCurrentShower().GetZenith()/deg << ", "
       << "  azimuth: " << file.GetCurrentShower().GetAzimuth()/deg << "\n"
       << "  N_muons: " << file.GetCurrentShower().GetMuonNumber() << endl;

  ShowerParticleList particles = file.GetCurrentShower().GetParticles();
  int count = 0;
  int muons = 0;
  double muon_bundle_energy = 0;
  for (ShowerParticleList::iterator it = particles.begin();
       it != particles.end(); ++it) {
    const int pdg = it->PDGCode();
    if ((pdg == CorsikaParticle::eMuon || pdg == CorsikaParticle::eMuon)) {
      muon_bundle_energy += it->KineticEnergy();
      ++muons;
    }
    ++count;
  }

  cout << "  " << count << " particles\n"
       << "  " << muons << " muons totalling " << muon_bundle_energy/TeV << " TeV" << endl;
         
  return 0;
}
