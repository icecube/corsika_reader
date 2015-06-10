#include <corsika/Constants.h>
#include <boost/python.hpp>

using namespace boost::python;
using namespace corsika;

void register_PhysicalConstants()
{
  namespace bp = boost::python;
  // map the Unit namespace to a sub-module
  // make "from mypackage.Unit import <whatever>" work
  bp::object constantsModule(bp::handle<>(bp::borrowed(PyImport_AddModule("corsika.constants"))));
  bp::scope().attr("constants") = constantsModule;
  bp::scope unit_scope = constantsModule;

  bp::scope().attr("kgEarth") = kgEarth;
  bp::scope().attr("kEarthRadius") = kEarthRadius;
  bp::scope().attr("kRadiationLength") = kRadiationLength;

  // grammage at sea level
  bp::scope().attr("kOverburdenSeaLevel") = kOverburdenSeaLevel;
  // refraction index sea level
  bp::scope().attr("kRefractiveIndexSeaLevel") = kRefractiveIndexSeaLevel;

  // A few gas and liquid properties

  bp::scope().attr("kMolarGasConstant") = kMolarGasConstant;
  bp::scope().attr("kAvogadro") = kAvogadro;
  bp::scope().attr("kBoltzmann") = kBoltzmann;

  bp::scope().attr("kDryAirMolarMass") = kDryAirMolarMass;
  bp::scope().attr("kN2MolarMass") = kN2MolarMass;
  bp::scope().attr("kO2MolarMass") = kO2MolarMass;
  bp::scope().attr("kArMolarMass") = kArMolarMass;
  bp::scope().attr("kCO2MolarMass") = kCO2MolarMass;
  bp::scope().attr("kH2OMolarMass") = kH2OMolarMass;

  bp::scope().attr("kN2AirFraction") = kN2AirFraction;
  bp::scope().attr("kO2AirFraction") = kO2AirFraction;
  bp::scope().attr("kArAirFraction") = kArAirFraction;
  bp::scope().attr("kCO2AirFraction") = kCO2AirFraction;

  bp::scope().attr("kH2OFreezingPoint") = kH2OFreezingPoint;

  // Gaisser-Hillas parameter for the electron mean free path in air
  bp::scope().attr("kLambdaGH") = kLambdaGH;

  // Constants

  bp::scope().attr("kSpeedOfLightSI") = kSpeedOfLightSI;
  bp::scope().attr("kSpeedOfLight") = kSpeedOfLight;
  bp::scope().attr("kSpeedOfLight2") = kSpeedOfLight2;
  bp::scope().attr("kPlanckSI") = kPlanckSI;
  bp::scope().attr("kPlanckReducedSI") = kPlanckReducedSI;
  bp::scope().attr("kPlanck") = kPlanck;
  bp::scope().attr("kPlanckReduced") = kPlanckReduced;
  bp::scope().attr("kMuZeroSI") = kMuZeroSI;
  bp::scope().attr("kMuZero") = kMuZero;

  // Derived constants

  bp::scope().attr("kEpsilonZeroSI") = kEpsilonZeroSI;
  bp::scope().attr("kAlpha") = kAlpha;
  bp::scope().attr("kElectronRadiusSI") = kElectronRadiusSI;
  bp::scope().attr("kThomsonCrossSectionSI") = kThomsonCrossSectionSI;
}
