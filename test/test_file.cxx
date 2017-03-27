#include "tests.h"
namespace
{
    void test_header(std::string filename)
    {
        ShowerFile file(filename);
        assert(file.IsOpen());
        
        auto& run_header = file.GetRunHeader();
        assert(run_header.fRunNumber == 2);
        
        Status status = file.FindEvent(1);
        assert(status == eSuccess);
        assert(file.GetCurrentShower().GetPrimary() == 2212);
        assert(fabs(file.GetCurrentShower().GetEnergy() - 100*TeV) < 0.01*MeV); // Primary energy
        assert(file.GetCurrentShower().GetMuonNumber() == 1335);
        assert(fabs(file.GetCurrentShower().GetZenith() - 20*deg) < 0.001*deg);
        assert(fabs(file.GetCurrentShower().GetAzimuth() - 175.381*deg) < 0.01*deg);
        assert(file.GetCurrentShower().GetMinRadiusCut() == 0);
        assert(file.GetCurrentShower().GetShowerNumber() == 1);
        assert(fabs(file.GetCurrentShower().GetEMEnergyCutoff() - 3*MeV) < 1e-4);
        assert(fabs(file.GetCurrentShower().GetMuonEnergyCutoff() - 300*MeV) < 1e-4);
    }
    
    void test_particles(std::string filename)
    {
        ShowerFile file(filename);
        file.FindEvent(1);
        auto& stream = file.GetCurrentShower().ParticleStream();
        int count = 0;
        
        auto it = stream.NextParticle();
        
        assert(it->PDGCode() == Particle::eMuon);
        assert(fabs(it->fTorZ - 183.053619385) < 1e-8);
        
        while(it)
        {
            it->GetName();
            const int pdg = it->PDGCode();
            if ((pdg == Particle::eMuon || pdg == Particle::eMuon)) {
                it->KineticEnergy();
            }
            ++count;
            it = stream.NextParticle();
        }
        assert(count == 181992);
    }
}
void test_file(const char* directory)
{
    std::string dir(directory);
    std::vector<const char*> filenames = {"/DAT000002-32", "/DAT000002-32.bz2", "/DAT000002-32.gz"};
    
    ShowerFile file;
    assert(!file.IsOpen());
    
    for (unsigned int i = 0; i != filenames.size(); ++i) {
        std::cout << "testing header " << dir << filenames[i] << std::endl;
        test_header(dir + filenames[i]);
        std::cout << "testing particles " << dir << filenames[i] << std::endl;
        test_particles(dir + filenames[i]);
    }
    printf("TestCorsikaFile Successfull!\n");
}
