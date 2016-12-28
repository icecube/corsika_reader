#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <corsika/CorsikaShower.h>
#include <vector>

using namespace boost::python;
using namespace corsika;

inline object identity(object const& o) { return o; }

object
expose_vector_as_array(CorsikaShower& self, std::vector<double> CorsikaShower::*member) {
  object numpy = import("numpy");
  object asarray = numpy.attr("asarray");
  return asarray(object(self.*member));
}
object
expose_vector_as_array(CorsikaShower& self, std::vector<double> member) {
  object numpy = import("numpy");
  object asarray = numpy.attr("asarray");
  return asarray(object(member));
}

object depth_de(CorsikaShower& self) { return expose_vector_as_array(self, self.fDepth_dE); }
object de_dx(CorsikaShower& self) { return expose_vector_as_array(self, self.fdEdX); }
object depth(CorsikaShower& self) { return expose_vector_as_array(self, self.fDepth); }
object charge_profile(CorsikaShower& self) { return expose_vector_as_array(self, self.fChargeProfile); }
object gamma_profile(CorsikaShower& self) { return expose_vector_as_array(self, self.fGammaProfile); }
object electron_profile(CorsikaShower& self) { return expose_vector_as_array(self, self.fElectronProfile); }
object muon_profile(CorsikaShower& self) { return expose_vector_as_array(self, self.fMuonProfile); }


class ParticleIterator {
public:
  ParticleIterator(CorsikaShowerFileParticleIterator* f=0):
    fIterator(f)
  {
    fIterator->Rewind();
  }

  void Rewind()
  {
    fIterator->Rewind();
  }

  CorsikaParticle& next_particle()
  {
    if (!fIterator) {
      PyErr_SetString(PyExc_StopIteration, "Empty particle list.");
      boost::python::throw_error_already_set();
    }
    ++(*fIterator);
    if ((*fIterator)==fInvalid) {
      PyErr_SetString(PyExc_StopIteration, "No more particles.");
      boost::python::throw_error_already_set();
    }
    return *(*fIterator);
  }
private:
  CorsikaShowerFileParticleIterator* fIterator;
  CorsikaShowerFileParticleIterator fInvalid;
};


ParticleIterator get_particle_iterator(CorsikaShower& shower)
{
  return ParticleIterator(&shower.GetParticleIt());
}

void register_CorsikaShower()
{
  class_<std::vector<double> >("vector_double")
    .def(vector_indexing_suite<std::vector<double> >())
    ;

  class_<ParticleIterator>("ParticleIterator")
    .def("__iter__", identity)
    .def("next", &ParticleIterator::next_particle, return_internal_reference<>())
    .def("rewind", &ParticleIterator::Rewind)
    ;

  class_<CorsikaShower>("CorsikaShower")
    .add_property("primary", &CorsikaShower::GetPrimary)
    .add_property("energy", &CorsikaShower::GetEnergy)
    .add_property("muon_number", &CorsikaShower::GetMuonNumber)
    .add_property("zenith", &CorsikaShower::GetZenith)
    .add_property("azimuth", &CorsikaShower::GetAzimuth)
    .add_property("min_radius_cut", &CorsikaShower::GetMinRadiusCut)
    .add_property("shower_number", &CorsikaShower::GetShowerNumber)
    .add_property("em_energy_cutoff", &CorsikaShower::GetEMEnergyCutoff)
    .add_property("muon_energy_cutoff", &CorsikaShower::GetMuonEnergyCutoff)
    .add_property("high_energy_hadronic_model", &CorsikaShower::HighEnergyHadronicModel)
    .add_property("low_energy_hadronic_model", &CorsikaShower::LowEnergyHadronicModel)
    .add_property("array_rotation", &CorsikaShower::ArrayRotation)
    .add_property("calorimetric_energy", &CorsikaShower::GetCalorimetricEnergy, &CorsikaShower::SetCalorimetricEnergy)
    //.def("particles", &CorsikaShower::GetParticleIt, return_internal_reference<>())
    .add_property("particles", get_particle_iterator) // memory management?
    .add_property("header", make_function(&CorsikaShower::GetEventHeader, return_internal_reference<>()))
    .add_property("gaisser_hillas", &CorsikaShower::GetGaisserHillasParams)
    //evt::GaisserHillas6Parameter GetGaisserHillasParams const
    .add_property("charge_profile", charge_profile)
    .add_property("gamma_profile", gamma_profile)
    .add_property("electron_profile", electron_profile)
    .add_property("muon_profile", muon_profile)
    .add_property("depth", depth)
    .add_property("de_dx", de_dx)
    .add_property("depth_de_dx", depth_de)
    ;
}
