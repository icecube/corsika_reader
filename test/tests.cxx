#include <corsika/RawStream.h>
#include <corsika/Block.h>
#include <string>
#include <corsika/ShowerFile.h>
#include <corsika/Units.h>

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
        void add(corsika::Particle& p)
        {
            num_particles++;
            int code = p.CorsikaCode();
            if (code == 5 || code == 6 || code == 95 || code == 96) num_muons++;
            else if (code == 2 || code == 3) num_electrons++;
            else if (code == 1) num_gammas++;
            
            particles.push_back({p.fX, p.fY, p.fPx, p.fPy});
        }
    };
    void run(const char* filename)
    {
        corsika::ShowerFile file(filename);
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
        auto raw_stream = corsika::RawStream::Create(filename);
        corsika::Block<corsika::NotThinned> block;
        raw_stream->GetNextBlock(block); //RUNH
        raw_stream->GetNextBlock(block); //EVTH
        auto raw_it = corsika::VRawParticleStream::Create(raw_stream);
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


int main(int argc, const char* argv[])
{
    assert(argc == 2);
    const char* filename = argv[1];
    
    TestLowHigh::run(filename);
    
}
