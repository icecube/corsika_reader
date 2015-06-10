#include <boost/python.hpp>
#include <corsika/CorsikaUnits.h>

using namespace boost::python;
using namespace corsika;

void register_CorsikaUnits()
{
  namespace bp = boost::python;
  // map the Unit namespace to a sub-module
  // make "from mypackage.Unit import <whatever>" work
  bp::object unitsModule(bp::handle<>(bp::borrowed(PyImport_AddModule("corsika.units"))));
  bp::scope().attr("units") = unitsModule;
  bp::scope unit_scope = unitsModule;

  bp::scope().attr("yocto") = yocto;
  bp::scope().attr("zepto") = zepto;
  bp::scope().attr("atto") = atto;
  bp::scope().attr("femto") = femto;
  bp::scope().attr("pico") = pico;
  bp::scope().attr("nano") = nano;
  bp::scope().attr("micro") = micro;
  bp::scope().attr("milli") = milli;
  bp::scope().attr("centi") = centi;
  bp::scope().attr("deci") = deci;
  bp::scope().attr("deka") = deka;
  bp::scope().attr("hecto") = hecto;
  bp::scope().attr("kilo") = kilo;
  bp::scope().attr("mega") = mega;
  bp::scope().attr("giga") = giga;
  bp::scope().attr("tera") = tera;
  bp::scope().attr("peta") = peta;
  bp::scope().attr("exa") = exa;
  bp::scope().attr("zetta") = zetta;
  bp::scope().attr("yotta") = yotta;

  //
  // Length [L]
  //
  bp::scope().attr("meter") = meter;
  bp::scope().attr("meter2") = meter2;
  bp::scope().attr("meter3") = meter3;

  bp::scope().attr("millimeter") = millimeter;
  bp::scope().attr("millimeter2") = millimeter2;
  bp::scope().attr("millimeter3") = millimeter3;

  bp::scope().attr("centimeter") = centimeter;
  bp::scope().attr("centimeter2") = centimeter2;
  bp::scope().attr("centimeter3") = centimeter3;

  bp::scope().attr("kilometer") = kilometer;
  bp::scope().attr("kilometer2") = kilometer2;
  bp::scope().attr("kilometer3") = kilometer3;

  bp::scope().attr("parsec") = parsec;
  bp::scope().attr("kiloParsec") = kiloParsec;
  bp::scope().attr("megaParsec") = megaParsec;

  bp::scope().attr("micrometer") = micrometer;
  bp::scope().attr("nanometer") = nanometer;
  bp::scope().attr("angstrom") = angstrom;
  bp::scope().attr("fermi") = fermi;

  bp::scope().attr("barn") = barn;
  bp::scope().attr("millibarn") = millibarn;
  bp::scope().attr("microbarn") = microbarn;
  bp::scope().attr("nanobarn") = nanobarn;
  bp::scope().attr("picobarn") = picobarn;

  // symbols
  bp::scope().attr("mm") = mm;
  bp::scope().attr("mm2") = mm2;
  bp::scope().attr("mm3") = mm3;

  bp::scope().attr("cm") = cm;
  bp::scope().attr("cm2") = cm2;
  bp::scope().attr("cm3") = cm3;

  bp::scope().attr("m") = m;
  bp::scope().attr("m2") = m2;
  bp::scope().attr("m3") = m3;

  bp::scope().attr("km") = km;
  bp::scope().attr("km2") = km2;
  bp::scope().attr("km3") = km3;

  //
  // Angle
  //
  bp::scope().attr("radian") = radian;
  bp::scope().attr("milliradian") = milliradian;
  bp::scope().attr("degree") = degree;

  bp::scope().attr("steradian") = steradian;

  // symbols
  bp::scope().attr("rad") = rad;
  bp::scope().attr("mrad") = mrad;
  bp::scope().attr("sr") = sr;
  bp::scope().attr("deg") = deg;

  //
  // Time [T]
  //
  bp::scope().attr("nanosecond") = nanosecond;
  bp::scope().attr("nanosecond2") = nanosecond2;
  bp::scope().attr("second") = second;
  bp::scope().attr("millisecond") = millisecond;
  bp::scope().attr("microsecond") = microsecond;
  bp::scope().attr("picosecond") = picosecond;
  bp::scope().attr("minute") = minute;
  bp::scope().attr("hour") = hour;
  bp::scope().attr("day") = day;

  bp::scope().attr("hertz") = hertz;
  bp::scope().attr("kilohertz") = kilohertz;
  bp::scope().attr("megahertz") = megahertz;

  bp::scope().attr("Hz") = Hz;
  bp::scope().attr("kHz") = kHz;
  bp::scope().attr("MHz") = MHz;

  // symbols
  bp::scope().attr("ns") = ns;
  bp::scope().attr("s") = s;
  bp::scope().attr("ms") = ms;
  bp::scope().attr("eplus") = eplus;
  bp::scope().attr("eSI") = eSI;
  bp::scope().attr("coulomb") = coulomb;
  bp::scope().attr("electronvolt") = electronvolt;
  bp::scope().attr("megaelectronvolt") = megaelectronvolt;
  bp::scope().attr("kiloelectronvolt") = kiloelectronvolt;
  bp::scope().attr("gigaelectronvolt") = gigaelectronvolt;
  bp::scope().attr("teraelectronvolt") = teraelectronvolt;
  bp::scope().attr("petaelectronvolt") = petaelectronvolt;
  bp::scope().attr("exaelectronvolt") = exaelectronvolt;
  bp::scope().attr("zettaelectronvolt") = zettaelectronvolt;

  bp::scope().attr("joule") = joule;
  bp::scope().attr("MeV") = MeV;
  bp::scope().attr("eV") = eV;
  bp::scope().attr("keV") = keV;
  bp::scope().attr("GeV") = GeV;
  bp::scope().attr("TeV") = TeV;
  bp::scope().attr("PeV") = PeV;
  bp::scope().attr("EeV") = EeV;
  bp::scope().attr("ZeV") = ZeV;
  bp::scope().attr("kilogram") = kilogram;
  bp::scope().attr("gram") = gram;
  bp::scope().attr("milligram") = milligram;
  bp::scope().attr("kg") = kg;
  bp::scope().attr("g") = g;
  bp::scope().attr("mg") = mg;
  bp::scope().attr("watt") = watt;
  bp::scope().attr("newton") = newton;
  bp::scope().attr("pascal") = pascal;
  bp::scope().attr("bar") = bar;
  bp::scope().attr("atmosphere") = atmosphere;
  bp::scope().attr("hPa") = hPa;
  bp::scope().attr("ampere") = ampere;
  bp::scope().attr("milliampere") = milliampere;
  bp::scope().attr("microampere") = microampere;
  bp::scope().attr("nanoampere") = nanoampere;
  bp::scope().attr("megavolt") = megavolt;
  bp::scope().attr("kilovolt") = kilovolt;
  bp::scope().attr("volt") = volt;

  bp::scope().attr("V") = V;
  bp::scope().attr("ohm") = ohm;
  bp::scope().attr("farad") = farad;
  bp::scope().attr("millifarad") = millifarad;
  bp::scope().attr("microfarad") = microfarad;
  bp::scope().attr("nanofarad") = nanofarad;
  bp::scope().attr("picofarad") = picofarad;
  bp::scope().attr("weber") = weber;
  bp::scope().attr("tesla") = tesla;
  bp::scope().attr("microtesla") = microtesla;
  bp::scope().attr("gauss") = gauss;
  bp::scope().attr("kilogauss") = kilogauss;
  bp::scope().attr("henry") = henry;
  bp::scope().attr("kelvin") = kelvin;
  bp::scope().attr("mole") = mole;
  bp::scope().attr("becquerel") = becquerel;
  bp::scope().attr("curie") = curie;
  bp::scope().attr("gray") = gray;
  bp::scope().attr("candela") = candela;
  bp::scope().attr("lumen") = lumen;
  bp::scope().attr("lux") = lux;
  bp::scope().attr("perCent") = perCent;
  bp::scope().attr("percent") = percent;
  bp::scope().attr("perThousand") = perThousand;
  bp::scope().attr("permil") = permil;
  bp::scope().attr("perMillion") = perMillion;

}
