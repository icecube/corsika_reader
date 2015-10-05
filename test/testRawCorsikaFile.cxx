const static char CVSId[] =
  "$Id$";

#include "corsika_test.h"
#include <boost/algorithm/string.hpp>

#include <corsika/RawCorsikaFile.h>
#include <string>
#include <vector>
#include <iostream>

using namespace std;
using namespace corsika;


TEST_GROUP(CorsikaTest);

namespace testRawCorsikaFileNS {
  void test_copy(string filename)
  {
    corsika::Corsika::RawFile rawUnthinnedFile;
    rawUnthinnedFile.Open(filename.c_str());

    Corsika::Block<corsika::Corsika::NotThinned> block;
    rawUnthinnedFile.GetNextBlock(block);
    rawUnthinnedFile.GetNextBlock(block);
    corsika::Corsika::RawFile rawUnthinnedFile2(rawUnthinnedFile);
    ENSURE_EQUAL(rawUnthinnedFile2.GetNextPosition(), rawUnthinnedFile.GetNextPosition());

    rawUnthinnedFile.GetNextBlock(block);
    rawUnthinnedFile2 = rawUnthinnedFile;
    ENSURE_EQUAL(rawUnthinnedFile2.GetNextPosition(), rawUnthinnedFile.GetNextPosition());
  }

  void test_basic(string filename)
  {
    corsika::Corsika::RawFile rawUnthinnedFile;
    rawUnthinnedFile.Open(filename.c_str());
    rawUnthinnedFile.Close();

    rawUnthinnedFile.Open(filename.c_str());
    ENSURE(rawUnthinnedFile.IsValid());

    Corsika::Block<corsika::Corsika::NotThinned> block;
    rawUnthinnedFile.GetNextBlock(block);
    ENSURE(block.IsRunHeader());

    rawUnthinnedFile.GetNextBlock(block);
    ENSURE(block.IsEventHeader());

    //ENSURE(rawUnthinnedFile.DiskBlockBuffer().fPaddingBeginning[0] == corsika::Corsika::NotThinned::kBytesPerBlock);
    //ENSURE(rawUnthinnedFile.DiskBlockBuffer().fPaddingEnd[0] == corsika::Corsika::NotThinned::kBytesPerBlock);

    int i = 0;
    while (rawUnthinnedFile.GetNextBlock(block) && i < 5000) {
      i += 1;
    }
    ENSURE_EQUAL(i, 4723);
  }

  void test_basic_stream(string filename)
  {
    Corsika::Compression c = Corsika::eNone;
    if (boost::algorithm::ends_with(filename, ".bz2")) {
      c = Corsika::eBZip2;
    }
    else if (boost::algorithm::ends_with(filename, ".gz")) {
      c = Corsika::eGZip;
    }

    ifstream f(filename.c_str());
    corsika::Corsika::RawFile rawUnthinnedStream(f, c);

    Corsika::Block<corsika::Corsika::NotThinned> block;
    rawUnthinnedStream.GetNextBlock(block);
    ENSURE(block.IsRunHeader());

    rawUnthinnedStream.GetNextBlock(block);
    ENSURE(block.IsEventHeader());

    //ENSURE(rawUnthinnedStream.DiskBlockBuffer().fPaddingBeginning[0] == corsika::Corsika::NotThinned::kBytesPerBlock);
    //ENSURE(rawUnthinnedStream.DiskBlockBuffer().fPaddingEnd[0] == corsika::Corsika::NotThinned::kBytesPerBlock);

    int i = 0;
    while (rawUnthinnedStream.GetNextBlock(block) && i < 5000) {
      i += 1;
    }
    ENSURE_EQUAL(i, 4723);
  }

}

using namespace testRawCorsikaFileNS;

TEST(testRawCorsikaFile)
{
  string dir(CORSIKA_TEST_DIR);
  test_copy(dir + string("/DAT000002-32"));
  vector<string> filenames;
  filenames.push_back("/DAT000002-32.bz2");
  filenames.push_back("/DAT000002-32");
  filenames.push_back("/DAT000002-32.gz");

  for (unsigned int i = 0; i != filenames.size(); ++i) {
    std::cout << "testing raw file " << filenames[i] << std::endl;
    test_basic(dir + filenames[i]);
    cout << "testing raw file " << filenames[i] << " from stream..." << endl;
    test_basic_stream(dir + filenames[i]);
  }
  //ENSURE(Verify<CloseTo>(p.GetCoordinates(CTrans), Triple(-1,0,0)));
}


TEST(testBlock)
{
  // This test makes an explicit check on the sizes of corsika blocks
  Corsika::Block<Corsika::NotThinned> not_thin_block;
  Corsika::Block<Corsika::NotThinned>::ParticleData particle;

  // one sub-block has 273 "words" each
  ENSURE_EQUAL(sizeof(float)*273, sizeof(not_thin_block));

  // All the classes in the union must have the same size
  ENSURE_EQUAL(sizeof(not_thin_block), sizeof(not_thin_block.AsRunHeader()));
  ENSURE_EQUAL(sizeof(not_thin_block), sizeof(not_thin_block.AsRunTrailer()));
  ENSURE_EQUAL(sizeof(not_thin_block), sizeof(not_thin_block.AsEventHeader()));
  ENSURE_EQUAL(sizeof(not_thin_block), sizeof(not_thin_block.AsEventTrailer()));
  ENSURE_EQUAL(sizeof(not_thin_block), sizeof(not_thin_block.AsParticleBlock()));
  ENSURE_EQUAL(sizeof(not_thin_block), sizeof(not_thin_block.AsCherenkovBlock()));
  ENSURE_EQUAL(sizeof(not_thin_block), sizeof(not_thin_block.AsLongitudinalBlock()));

  Corsika::Block<Corsika::Thinned> thin_block;
  Corsika::Block<Corsika::Thinned>::ParticleData thin_particle;
  // comparing thinned blocks to their standard counterparts (only particle blocks change in size, the rest are zero-padded)
  ENSURE_EQUAL(sizeof(not_thin_block.AsRunHeader()), sizeof(thin_block.AsRunHeader()));
  ENSURE_EQUAL(sizeof(not_thin_block.AsRunTrailer()), sizeof(thin_block.AsRunTrailer()));
  ENSURE_EQUAL(sizeof(not_thin_block.AsEventHeader()), sizeof(thin_block.AsEventHeader()));
  ENSURE_EQUAL(sizeof(not_thin_block.AsEventTrailer()), sizeof(thin_block.AsEventTrailer()));
  ENSURE_EQUAL(sizeof(not_thin_block.AsLongitudinalBlock()), sizeof(thin_block.AsLongitudinalBlock()));
  ENSURE_EQUAL(sizeof(thin_block), sizeof(thin_block.AsParticleBlock()));
  ENSURE_EQUAL(sizeof(thin_block), sizeof(thin_block.AsCherenkovBlock()));
}



// Configure (x)emacs for this file ...
// Local Variables:
// mode:c++
// End:
