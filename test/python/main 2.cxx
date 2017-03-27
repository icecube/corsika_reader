#ifdef CORSIKA_I3
#warning IceCube testing
#include <I3TestMain.ixx>
#else
#warning Stand-alone testing
#define BOOST_TEST_MAIN
#define BOOST_ALL_DYN_LINK
#define BOOST_TEST_MODULE CorsikaTest
#include <boost/test/unit_test.hpp>
#endif
