#include "tests.h"

namespace
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
}
void test_rawstream(const char* directory)
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
