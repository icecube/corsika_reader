#include <boost/python.hpp>
#include <corsika/Constants.h>

using namespace boost::python;
using namespace corsika;

void register_MathConstants()
{
  namespace bp = boost::python;
  // map the Unit namespace to a sub-module
  // make "from mypackage.Unit import <whatever>" work
  bp::object mathModule(bp::handle<>(bp::borrowed(PyImport_AddModule("corsika.math"))));
  bp::scope().attr("math") = mathModule;
  bp::scope unit_scope = mathModule;

  bp::scope().attr("kPi")      = kPi;
  bp::scope().attr("kPiOnTwo") = kPiOnTwo;
  bp::scope().attr("kTwoPi")   = kTwoPi;
  bp::scope().attr("kE")       = kE;
  bp::scope().attr("kLn10")    = kLn10;
  bp::scope().attr("kSqrt2")   = kSqrt2;
  bp::scope().attr("kSqrt3")   = kSqrt3;
  bp::scope().attr("kSqrt2Pi") = kSqrt2Pi;
  bp::scope().attr("kSqrtPi")  = kSqrtPi;

  // Euler-Mascheroni Constant
  bp::scope().attr("kEulerGamma") = kEulerGamma;

}
