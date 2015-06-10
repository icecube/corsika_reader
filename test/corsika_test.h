#ifdef CORSIKA_I3
#  include <I3Test.h>
#else
#  include <boost/test/unit_test.hpp>

#  define ENSURE(PRED,...) {BOOST_CHECK(PRED);}
#  define ENSURE_EQUAL(LEFT,RIGHT,...) BOOST_CHECK((LEFT == RIGHT))
#  define ENSURE_DISTANCE(LEFT,RIGHT,DISTANCE,...) BOOST_CHECK(((LEFT - RIGHT < DISTANCE) && (RIGHT - LEFT < DISTANCE)))
#  define FAIL(...) BOOST_ERROR(##__VA_ARGS__)
#  define TEST(TESTNAME) BOOST_AUTO_TEST_CASE(TESTNAME)
#  define TEST_GROUP(GROUPNAME)
#endif
