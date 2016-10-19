#include <boost/python.hpp>
#include <corsika/CorsikaLongProfile.h>
#include <string>

using namespace boost::python;
using namespace corsika;

object
expose_vector_as_array(CorsikaLongProfile& self, std::vector<double> CorsikaLongProfile::*member) {
  object numpy = import("numpy");
  object asarray = numpy.attr("asarray");
  return asarray(object(self.*member));
}
object
expose_vector_as_array(CorsikaLongProfile& self, std::vector<double> member) {
  object numpy = import("numpy");
  object asarray = numpy.attr("asarray");
  return asarray(object(member));
}

//object depth_de(CorsikaLongProfile& self) { return expose_vector_as_array(self, self.fDepth_dE); }
//object de_dx(CorsikaLongProfile& self) { return expose_vector_as_array(self, self.fdEdX); }
object depth(CorsikaLongProfile& self) { return expose_vector_as_array(self, self.fDepth); }
object charge_profile(CorsikaLongProfile& self) { return expose_vector_as_array(self, self.fChargeProfile); }
object gamma_profile(CorsikaLongProfile& self) { return expose_vector_as_array(self, self.fGammaProfile); }
object electron_profile(CorsikaLongProfile& self) { return expose_vector_as_array(self, self.fElectronProfile); }
object muon_profile(CorsikaLongProfile& self) { return expose_vector_as_array(self, self.fMuonProfile); }

void register_CorsikaLongProfile()
{
  class_<CorsikaLongProfile>("CorsikaLongProfile")
    .add_property("charge", charge_profile)
    .add_property("gamma", gamma_profile)
    .add_property("electron", electron_profile)
    .add_property("muon", muon_profile)
    .add_property("depth", depth)
    .add_property("calorimetric_energy", &CorsikaLongProfile::fCalorimetricEnergy)
    ;
}
