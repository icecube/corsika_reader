#include <corsika/RawStream.h>
#include <corsika/Block.h>
#include <corsika/ShowerFile.h>
#include <corsika/Units.h>
#include <corsika/Index.h>
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


#define ENSURE_EQUAL(a, b, ...) assert(a == b)

namespace TestRawStream
{
    void test_basic(std::string filename)
    {
        RawStreamPtr stream = RawStream::Create(filename);
        Block<corsika::NotThinned> block;
        stream->GetNextBlock(block);
        assert(block.IsRunHeader());
        
        stream->GetNextBlock(block);
        assert(block.IsEventHeader());
        assert(!stream->IsThinned());
        
        
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
        //assert(Verify<CloseTo>(p.GetCoordinates(CTrans), Triple(-1,0,0)));
        printf("TestRawStream Successfull!\n");
    }
}

namespace TestIndex
{
    using corsika::km;
    using corsika::m;
    using corsika::cm;
    
    struct pointee
    {
        double x_,y_,dx_,dy_;
        pointee(double x, double y, double dx, double dy): x_(x),y_(y),dx_(dx),dy_(dy) {}
        double x() const { return x_; }
        double dx() const { return dx_; }
        double y() const { return y_; }
        double dy() const { return dy_; }
        bool operator==(const pointee& other) const { return x_==other.x_ && dx_==other.dx_ && y_==other.y_ && dy_==other.dy_; }
    };
    
    void test_details()
    {
        double a[11] = {-10., -8., -6., -4., -2., 0., 2., 4., 6., 8., 10.};
        std::vector<double> edges;
        edges.assign(a, a + 11);
        ENSURE_EQUAL(edges.size(), (unsigned)11);
        ENSURE_EQUAL(corsika::detail::digitize(edges, -11.), (unsigned)0);
        ENSURE_EQUAL(corsika::detail::digitize(edges, 11.), (unsigned)11);
        ENSURE_EQUAL(corsika::detail::digitize(edges, -10.), (unsigned)1);
        ENSURE_EQUAL(corsika::detail::digitize(edges, -9.5), (unsigned)1);
        ENSURE_EQUAL(corsika::detail::digitize(edges, -7.5), (unsigned)2);
        ENSURE_EQUAL(corsika::detail::digitize(edges, 2.), (unsigned)7);
        
        // these fail with std::upper_bound(arr.begin(), arr.end(), x, less);
        ENSURE_EQUAL(corsika::detail::digitize(edges, -6.5), (unsigned)2);
        ENSURE_EQUAL(corsika::detail::digitize(edges, -4.5), (unsigned)3);
        ENSURE_EQUAL(corsika::detail::digitize(edges, -2.5), (unsigned)4);
        ENSURE_EQUAL(corsika::detail::digitize(edges, -0.5), (unsigned)5);
        
        
        double a2[21] = {-10., -9., -8., -7., -6., -5., -4., -3., -2., -1., 0.,
            1., 2., 3., 4., 5., 6., 7., 8., 9., 10.};
        std::vector<double> edges2;
        edges2.assign(a2, a2 + 21);
        ENSURE_EQUAL(edges2.size(), (unsigned)21);
        
        ENSURE_EQUAL(*std::upper_bound(edges2.begin(), edges2.end(), 0.5, corsika::detail::less), 1.);
        ENSURE_EQUAL(*std::upper_bound(edges2.begin(), edges2.end(), -0.5, corsika::detail::less), 0.);
        
        ENSURE_EQUAL(corsika::detail::digitize(edges2, 0.5), (unsigned)11);
        ENSURE_EQUAL(corsika::detail::digitize(edges2, -0.5), (unsigned)10);
        ENSURE_EQUAL(corsika::detail::digitize(edges2, -9.5), (unsigned)1);
        ENSURE_EQUAL(corsika::detail::digitize(edges2, 10.1), (unsigned)21);
        
        double a3[11] = {-10., -8., -8., -8., -2., 0., 0., 4., 6., 8., 10.};
        std::vector<double> edges3;
        edges3.assign(a3, a3 + 11);
        ENSURE_EQUAL(edges3.size(), (unsigned)11);
        ENSURE_EQUAL(corsika::detail::digitize(edges3, 0.5), (unsigned)7);
        ENSURE_EQUAL(corsika::detail::digitize(edges3, -7.5), (unsigned)4);
    }
    
    void test_square_policy()
    {
        double a[21] = {-10., -9., -8., -7., -6., -5., -4., -3., -2., -1., 0.,
            1., 2., 3., 4., 5., 6., 7., 8., 9., 10.};
        std::vector<double> edges(21);
        edges.assign(a, a + 21);
        
        corsika::SquareCollision<pointee> policy;
        
        typedef std::pair<unsigned int,unsigned int> IndexPair;
        pointee p(5.5, -9.5, 0.01, 0.01);
        std::vector<std::pair<unsigned int,unsigned int> > bins = policy(edges, edges, p);
        ENSURE_EQUAL(bins.size(), (unsigned)1, "Bins. Tiny area centered in a bin");
        ENSURE_EQUAL(bins[0].first, (unsigned)16, "bins[0].first. Tiny area centered in a bin");
        ENSURE_EQUAL(bins[0].second, (unsigned)1, "bins[0].second. Tiny area centered in a bin");
        
        std::cout << std::endl;
        pointee p2(5., -2., 0.01, 0.01);
        std::vector<std::pair<unsigned int,unsigned int> > bins2 = policy(edges, edges, p2);
        for (unsigned int i = 0; i != bins2.size(); ++i) {
            std::cout << "test_square_policy: (" << bins2[i].first << ", " << bins2[i].second << ")" << std::endl;
        }
        ENSURE_EQUAL(bins2.size(), (unsigned)4, "Square collision, tiny area right at the edge of a bin");
        
        pointee p3(5.5, -2.5, 3.5, 1.5);
        std::vector<std::pair<unsigned int,unsigned int> > bins3 = policy(edges, edges, p3);
        ENSURE_EQUAL(bins3.size(), (unsigned)15, "Square collision, bin-centered, size 3.5 and 1.5 bin size");
        
        pointee p4(5.5, -2.5, 3., 3.);
        std::vector<std::pair<unsigned int,unsigned int> > bins4 = policy(edges, edges, p4);
        ENSURE_EQUAL(bins4.size(), (unsigned)16, "Square collision, bin-centered, size 3. bin size (clipping the upper bin)");
        
    }
    
    void test_square_index()
    {
        corsika::PositionIndex<pointee> index(4*corsika::km, 50*corsika::m);
        pointee p(1*km, -1.5*km, 500*m, 100*m);
        index.Add(p);
        
        ENSURE_EQUAL(index.GetSize(0), 4*corsika::km, "PositionIndex grid size mismatch");
        ENSURE_EQUAL(index.GetSpacing(0), 50*corsika::m, "PositionIndex grid spacing mismatch");
        ENSURE_EQUAL(index.GetSize(1), 4*corsika::km, "PositionIndex grid size mismatch");
        ENSURE_EQUAL(index.GetSpacing(1), 50*corsika::m, "PositionIndex grid spacing mismatch");
        
        corsika::PositionIndex<pointee>::pointer_list_type v = index.Get(0,0);
        ENSURE_EQUAL(v.size(), (unsigned)0, "I know there is no pointee there!");
        
        v = index.Get(p.x(), p.y());
        ENSURE_EQUAL(v.size(), (unsigned)1, "There should be only one pointee.");
        
        // these work because dx/2 and dy/2 are multiples of the grid separation
        v = index.Get(p.x() - p.dx()/2, p.y());
        ENSURE_EQUAL(v.size(), (unsigned)1, "x-axis lower edge: There should be exactly one pointee.");
        v = index.Get(p.x() + p.dx()/2, p.y());
        ENSURE_EQUAL(v.size(), (unsigned)1, "x-axis upper edge: There should be exactly one pointee.");
        v = index.Get(p.x() - p.dx()/2 - 0.0001*cm, p.y());
        ENSURE_EQUAL(v.size(), (unsigned)0, "x-axis lower edge-: I know there is no pointee there!");
        v = index.Get(p.x() - p.dx()/2 + 0.0001*cm, p.y());
        ENSURE_EQUAL(v.size(), (unsigned)1, "x-axis lower edge+: There should be exactly one pointee.");
        // binning policy interval is closed below, the upper edge clips the bin right above
        v = index.Get(p.x() + p.dx()/2 + index.GetSpacing(0) - 0.0001*cm, p.y());
        ENSURE_EQUAL(v.size(), (unsigned)1, "x-axis upper edge-: There should be exactly one pointee.");
        v = index.Get(p.x() + p.dx()/2 + index.GetSpacing(0) + 0.0001*cm, p.y());
        ENSURE_EQUAL(v.size(), (unsigned)0, "x-axis upper edge+: I know there is no pointee there!");
        
        v = index.Get(p.x(), p.y() - p.dy()/2);
        ENSURE_EQUAL(v.size(), (unsigned)1, "y-axis lower edge: There should be exactly one pointee.");
        v = index.Get(p.x(), p.y() + p.dy()/2);
        ENSURE_EQUAL(v.size(), (unsigned)1, "y-axis upper edge: There should be exactly one pointee.");
        v = index.Get(p.x(), p.y() - p.dy()/2 - 0.0001*cm);
        ENSURE_EQUAL(v.size(), (unsigned)0, "y-axis lower edge-: I know there is no pointee there!");
        v = index.Get(p.x(), p.y() - p.dy()/2 + 0.0001*cm);
        ENSURE_EQUAL(v.size(), (unsigned)1, "y-axis lower edge+: There should be exactly one pointee.");
        // binning policy interval is closed below, the upper edge clips the bin right above
        v = index.Get(p.x(), p.y() + p.dy()/2 + index.GetSpacing(0) - 0.0001*cm);
        ENSURE_EQUAL(v.size(), (unsigned)1, "y-axis upper edge-: There should be exactly one pointee.");
        v = index.Get(p.x(), p.y() + p.dy()/2 + index.GetSpacing(0) + 0.0001*cm);
        ENSURE_EQUAL(v.size(), (unsigned)0, "y-axis upper edge+: I know there is no pointee there!");
        
        index.Add(pointee(1*km, -3*km, 500*m, 100*m));
        ENSURE_EQUAL(index.Get(1*km, -10*km).size(), (unsigned)1, "y-axis underflow bin");
        index.Add(pointee(3*km, -3*km, 500*m, 100*m));
        ENSURE_EQUAL(index.Get(10*km, -10*km).size(), (unsigned)1, "(x/y)-axis (over/under)flow bin");
        
        corsika::PositionIndex<pointee> index2(40*corsika::km, 45*corsika::m);
        ENSURE_EQUAL(index2.GetBins(0), 888, "PositionIndex number of bins round mismatch.");
        ENSURE_EQUAL(index2.GetSpacing(0), index2.GetSize(0)/index2.GetBins(0), "PositionIndex grid spacing mismatch");
        assert(index2.GetSpacing(0) != 45*m); // see? it is not 45 m
        ENSURE_EQUAL(index2.GetBins(1), 888, "PositionIndex number of bins round mismatch.");
        ENSURE_EQUAL(index2.GetSpacing(1), index2.GetSize(1)/index2.GetBins(1), "PositionIndex grid spacing mismatch");
        assert(index2.GetSpacing(1) != 45*m); // see? it is not 45 m
        
    }
    
    void test_rectangular_index()
    {
        corsika::PositionIndex<pointee> index(0., 4*corsika::km, 80, 1.*corsika::km, 3.*corsika::km, 20);
        pointee p(1*km, 1.5*km, 500*m, 100*m);
        index.Add(p);
        
        ENSURE_EQUAL(index.GetSize(0), 4*corsika::km, "PositionIndex grid size mismatch");
        ENSURE_EQUAL(index.GetSpacing(0), 50*corsika::m, "PositionIndex grid spacing mismatch");
        ENSURE_EQUAL(index.GetSize(1), 2*corsika::km, "PositionIndex grid size mismatch");
        ENSURE_EQUAL(index.GetSpacing(1), 100*corsika::m, "PositionIndex grid spacing mismatch");
        ENSURE_EQUAL(index.GetBins(0), 80);
        ENSURE_EQUAL(index.GetBins(1), 20);
        
        corsika::PositionIndex<pointee>::pointer_list_type v = index.Get(0,0);
        ENSURE_EQUAL(v.size(), (unsigned)0, "I know there is no pointee there!");
        
        v = index.Get(p.x(), p.y());
        ENSURE_EQUAL(v.size(), (unsigned)1, "There should be only one pointee.");
    }
    void run()
    {
        test_details();
        test_square_policy();
        test_square_index();
        test_rectangular_index();
        printf("TestIndex Successfull!\n");
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
    TestIndex::run();
    printf("All Tests Were Successfull!\n");
}
