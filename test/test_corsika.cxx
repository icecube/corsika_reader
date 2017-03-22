#include <corsika/RawStream.h>
#include <corsika/Block.h>
#include <corsika/ShowerFile.h>
#include <corsika/Units.h>
#include <string>
#include <vector>
using namespace corsika;

// This test compares the result of using the ShowerParticleStream and RawParticleStream classes
namespace TestLowHigh
{
    struct ParticleType
    {
        float x,y, px, py;
    };
    struct ParticleInfo
    {
        std::vector<ParticleType> particles;
        
        size_t num_particles = 0, num_muons = 0, num_electrons = 0, num_gammas = 0;
        void add(Particle& p)
        {
            num_particles++;
            int code = p.CorsikaCode();
            if (code == 5 || code == 6 || code == 95 || code == 96) num_muons++;
            else if (code == 2 || code == 3) num_electrons++;
            else if (code == 1) num_gammas++;
            
            particles.push_back({p.fX, p.fY, p.fPx, p.fPy});
        }
    };
    void run(const char* dir)
    {
        std::string filename = std::string(dir) + "/DAT000011-proton-EHISTORY-MUPROD";
        ShowerFile file(filename);
        file.FindEvent(1);
        auto shower = file.GetCurrentShower();
        auto stream = shower.ParticleStream();
        
        ParticleInfo high;
        while (auto p = stream.NextParticle())
            high.add(*p);
        
        assert(high.num_particles == 254188);
        assert(high.num_muons == 10338);
        assert(high.num_electrons == 78040);
        assert(high.num_gammas == 164973);
        
        ParticleInfo low;
        auto raw_stream = RawStream::Create(filename);
        Block<NotThinned> block;
        raw_stream->GetNextBlock(block); //RUNH
        raw_stream->GetNextBlock(block); //EVTH
        auto raw_it = VRawParticleStream::Create(raw_stream);
        while (auto p = raw_it->NextParticle())
            if (p->fDescription > 0 && p->PDGCode() != 0 && ((uint)p->fDescription)%10 == 1)
                low.add(*p);
        
        assert(high.num_particles == low.num_particles);
        assert(high.num_muons == low.num_muons);
        assert(high.num_electrons == low.num_electrons);
        assert(high.num_gammas == low.num_gammas);
        
        assert(high.particles.size() == low.particles.size());
        for (size_t i = 0; i < high.particles.size(); i++)
        {
            assert(high.particles[i].x == low.particles[i].x);
            assert(high.particles[i].y == low.particles[i].y);
            assert(high.particles[i].px == low.particles[i].px);
            assert(high.particles[i].py == low.particles[i].py);
        }
        printf("High - Low Test Successfull!\n");
    }
}

namespace TestCorsikaFile
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
    void run(const char* directory)
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
}


#define ENSURE(v) assert(v)
#define ENSURE_EQUAL(a, b) assert(a == b)

namespace TestRawStream
{
    void test_basic(std::string filename)
    {
        RawStreamPtr stream = RawStream::Create(filename);
        Block<corsika::NotThinned> block;
        stream->GetNextBlock(block);
        ENSURE(block.IsRunHeader());
        
        stream->GetNextBlock(block);
        ENSURE(block.IsEventHeader());
        ENSURE(!stream->IsThinned());
        
        
        int i = 0;
        while (stream->GetNextBlock(block) && i < 5000) {
            i += 1;
        }
        ENSURE_EQUAL(i, 4723);
        
        i = 0;
        int all = 0;
        auto particles = VRawParticleStream::Create(stream, 2);
        while (auto p = particles->NextParticle())
        {
            ++all;
            if (p->fDescription > 0 &&
                p->CorsikaCode() != 75 &&  p->CorsikaCode() != 76 &&
                p->CorsikaCode() != 85 &&  p->CorsikaCode() != 86 &&
                p->CorsikaCode() != 95 &&  p->CorsikaCode() != 96)
                i += 1;
        }
        ENSURE_EQUAL(all, 183339);
        ENSURE_EQUAL(i, 181992);
    }
    
    void run(const char* directory)
    {
        std::string dir(directory);
        std::vector<const char*> filenames = {"/DAT000002-32"};
        
        for (unsigned int i = 0; i != filenames.size(); ++i)
        {
            std::cout << "testing raw stream " << filenames[i] << std::endl;
            test_basic(dir + filenames[i]);
        }
        //ENSURE(Verify<CloseTo>(p.GetCoordinates(CTrans), Triple(-1,0,0)));
        printf("TestRawStream Successfull!\n");
    }
}

int main(int argc, const char* argv[])
{
    if (argc != 2)
    {
        printf("Usage: test_corsika path_to_test_data\n");
        return 1;
    }
    const char* dir = argv[1];
    
    TestLowHigh::run(dir);
    TestCorsikaFile::run(dir);
    TestRawStream::run(dir);
    
}
