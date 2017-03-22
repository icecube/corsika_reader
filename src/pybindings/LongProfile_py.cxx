#include <boost/python.hpp>
#include <corsika/LongProfile.h>
#include <string>

using namespace boost::python;
using namespace corsika;

object
expose_vector_as_array(LongProfile& self, std::vector<double> LongProfile::*member) {
  object numpy = import("numpy");
  object asarray = numpy.attr("asarray");
  return asarray(object(self.*member));
}
object
expose_vector_as_array(LongProfile& self, std::vector<double> member) {
  object numpy = import("numpy");
  object asarray = numpy.attr("asarray");
  return asarray(object(member));
}

//object depth_de(LongProfile& self) { return expose_vector_as_array(self, self.fDepth_dE); }
//object de_dx(LongProfile& self) { return expose_vector_as_array(self, self.fdEdX); }
object depth(LongProfile& self) { return expose_vector_as_array(self, self.fDepth); }
object charge_profile(LongProfile& self) { return expose_vector_as_array(self, self.fChargeProfile); }
object gamma_profile(LongProfile& self) { return expose_vector_as_array(self, self.fGammaProfile); }
object electron_profile(LongProfile& self) { return expose_vector_as_array(self, self.fElectronProfile); }
object muon_profile(LongProfile& self) { return expose_vector_as_array(self, self.fMuonProfile); }

void register_LongProfile()
{
  class_<LongProfile>("LongProfile")
    .add_property("charge", charge_profile)
    .add_property("gamma", gamma_profile)
    .add_property("electron", electron_profile)
    .add_property("muon", muon_profile)
    .add_property("depth", depth)
    .add_property("calorimetric_energy", &LongProfile::fCalorimetricEnergy)
    ;
}
