// $Id$
  /**
     \file Constants.h

     \brief Physical and mathematical constants
  */

#ifndef _utl_Constants_h_
#define _utl_Constants_h_

#include <corsika/CorsikaUnits.h>


namespace corsika {

  //-----------------------
  // Mathematical constants
  //-----------------------

  const double kPi      = 3.1415926535897932384626;
  const double kPiOnTwo = 0.5*kPi;
  const double kTwoPi   = 2*kPi;
  const double kE       = 2.7182818284590452353602;
  const double kLn10    = 2.3025850929940456840180;
  const double kSqrt2   = 1.4142135623730950488017;
  const double kSqrt3   = 1.7320508075688772935274;
  const double kSqrt2Pi = 2.5066282746310005024158;
  const double kSqrtPi  = 1.7724538509055160272982;

  // Euler-Mascheroni Constant
  const double kEulerGamma = 0.5772156649015328606065;

  //-----------------------
  // Physical constants
  //-----------------------

  const double kgEarth = 9.81 * m/(s*s);
  const double kEarthRadius = 6371.315*km; // average
  const double kRadiationLength = 37 * g/cm2;

  // grammage at sea level
  const double kOverburdenSeaLevel = 1033 * g/cm2;
  // refraction index sea level
  const double kRefractiveIndexSeaLevel = 1.000292;

  // A few gas and liquid properties

  const double kMolarGasConstant =  8.3145 * joule/(mole*kelvin); // R: NIST
  const double kAvogadro = 6.022142e23 / mole;                    // Na: NIST
  const double kBoltzmann = kMolarGasConstant / kAvogadro;        // kB = R/Na

  const double kDryAirMolarMass = 28.97 * gram/mole;  // M. Note: R_spec = R/M
  const double kN2MolarMass = 28.0134 * gram/mole;
  const double kO2MolarMass = 31.9989 * gram/mole;
  const double kArMolarMass = 39.9481 * gram/mole;
  const double kCO2MolarMass = 44.0096 * gram/mole;
  const double kH2OMolarMass = 18.0153 * gram/mole;

  const double kN2AirFraction = 780840 * perMillion;  // Dry air vol. fractions;
  const double kO2AirFraction = 209460 * perMillion;  // NASA Earth Fact Sheet.
  const double kArAirFraction =   9340 * perMillion;  // H2O vapor @ surface is
  const double kCO2AirFraction =   380 * perMillion;  // ~10 000 ppm.

  const double kH2OFreezingPoint = 273.15 * kelvin;

  // All taken from PDG data tables (2002)

  // Gaisser-Hillas parameter for the electron mean free path in air
  const double kLambdaGH = 70 * g/cm2;

  // Constants

  const double kSpeedOfLightSI = 299792458;
  const double kSpeedOfLight = kSpeedOfLightSI * m/s;
  const double kSpeedOfLight2 = kSpeedOfLight * kSpeedOfLight;
  const double kPlanckSI = 6.62606876e-34;
  const double kPlanckReducedSI = kPlanckSI / (2*kPi);
  const double kPlanck = kPlanckSI * joule*s;
  const double kPlanckReduced = kPlanckReducedSI * joule*s;
  const double kMuZeroSI = 4*kPi * 1e-7;
  const double kMuZero = kMuZeroSI*newton/(ampere*ampere);

  // Derived constants
  const double kMassConversionSI = eSI / (kSpeedOfLightSI*kSpeedOfLightSI);

  const double kEpsilonZeroSI = 1 / (kMuZeroSI * kSpeedOfLightSI*kSpeedOfLightSI);
  const double kAlpha = (eSI*eSI) /
    (4*kPi * kEpsilonZeroSI * kPlanckReducedSI * kSpeedOfLightSI);
  const double kElectronRadiusSI = (eSI*eSI) /
    (4*kPi * kEpsilonZeroSI * (0.510998902 * MeV * kMassConversionSI) * kSpeedOfLightSI * kSpeedOfLightSI);
  const double kThomsonCrossSectionSI =
    8*kPi * kElectronRadiusSI * kElectronRadiusSI / 3;

}

#endif
