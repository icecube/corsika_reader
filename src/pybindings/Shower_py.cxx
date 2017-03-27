#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <corsika/Shower.h>
#include <vector>

using namespace boost::python;
using namespace corsika;

inline object identity(object const& o) { return o; }

object
expose_vector_as_array(Shower& self, std::vector<double> Shower::*member) {
  object numpy = import("numpy");
  object asarray = numpy.attr("asarray");
  return asarray(object(self.*member));
}
object
expose_vector_as_array(Shower& self, std::vector<double> member) {
  object numpy = import("numpy");
  object asarray = numpy.attr("asarray");
  return asarray(object(member));
}

object depth_de(Shower& self) { return expose_vector_as_array(self, self.fDepth_dE); }
object de_dx(Shower& self) { return expose_vector_as_array(self, self.fdEdX); }
object depth(Shower& self) { return expose_vector_as_array(self, self.fDepth); }
object charge_profile(Shower& self) { return expose_vector_as_array(self, self.fChargeProfile); }
object gamma_profile(Shower& self) { return expose_vector_as_array(self, self.fGammaProfile); }
object electron_profile(Shower& self) { return expose_vector_as_array(self, self.fElectronProfile); }
object positron_profile(Shower& self) { return expose_vector_as_array(self, self.fPositronProfile); }
object muon_profile(Shower& self) { return expose_vector_as_array(self, self.fMuonProfile); }
object anti_muon_profile(Shower& self) { return expose_vector_as_array(self, self.fAntiMuonProfile); }
object hadron_profile(Shower& self) { return expose_vector_as_array(self, self.fHadronProfile); }
object nuclei_profile(Shower& self) { return expose_vector_as_array(self, self.fNucleiProfile); }
object cherenkov_profile(Shower& self) { return expose_vector_as_array(self, self.fCherenkovProfile); }

class ParticleIterator {
public:
  ParticleIterator(ShowerParticleStream* f=0):
    fIterator(f)
  {
    fIterator->Rewind();
  }

  void Rewind()
  {
    fIterator->Rewind();
  }

  Particle next_particle()
  {
    if (!fIterator) {
      PyErr_SetString(PyExc_StopIteration, "Empty particle list.");
      boost::python::throw_error_already_set();
    }
      boost::optional<Particle> p = fIterator->NextParticle();
    if (!p) {
      PyErr_SetString(PyExc_StopIteration, "No more particles.");
      boost::python::throw_error_already_set();
    }
    return *p;
  }
private:
  ShowerParticleStream* fIterator;
  ShowerParticleStream fInvalid;
};


ParticleIterator get_particle_iterator(Shower& shower)
{
  return ParticleIterator(&shower.ParticleStream());
}

void register_Shower()
{
  class_<std::vector<double> >("vector_double")
    .def(vector_indexing_suite<std::vector<double> >())
    ;

  class_<ParticleIterator>("ParticleIterator")
    .def("__iter__", identity)
    .def("next", &ParticleIterator::next_particle)
    .def("rewind", &ParticleIterator::Rewind)
    ;

  class_<Shower>("Shower")
    .add_property("primary", &Shower::GetPrimary)
    .add_property("energy", &Shower::GetEnergy)
    .add_property("muon_number", &Shower::GetMuonNumber)
    .add_property("zenith", &Shower::GetZenith)
    .add_property("azimuth", &Shower::GetAzimuth)
    .add_property("min_radius_cut", &Shower::GetMinRadiusCut)
    .add_property("shower_number", &Shower::GetShowerNumber)
    .add_property("em_energy_cutoff", &Shower::GetEMEnergyCutoff)
    .add_property("muon_energy_cutoff", &Shower::GetMuonEnergyCutoff)
    .add_property("high_energy_hadronic_model", &Shower::HighEnergyHadronicModel)
    .add_property("low_energy_hadronic_model", &Shower::LowEnergyHadronicModel)
    .add_property("array_rotation", &Shower::ArrayRotation)
    .add_property("calorimetric_energy", &Shower::GetCalorimetricEnergy, &Shower::SetCalorimetricEnergy)
    //.def("particles", &Shower::GetParticleIt, return_internal_reference<>())
    .add_property("particles", get_particle_iterator) // memory management?
    .add_property("header", make_function(&Shower::GetEventHeader, return_internal_reference<>()))
    .add_property("gaisser_hillas", &Shower::GetGaisserHillasParams)
    //evt::GaisserHillas6Parameter GetGaisserHillasParams const
    .add_property("charge_profile", charge_profile)
    .add_property("gamma_profile", gamma_profile)
    .add_property("electron_profile", electron_profile)
    .add_property("positron_profile",  positron_profile)
    .add_property("muon_profile", muon_profile)
    .add_property("anti_muon_profile", anti_muon_profile)
    .add_property("hadron_profile",    hadron_profile)
    .add_property("nuclei_profile",    nuclei_profile)
    .add_property("cherenkov_profile", cherenkov_profile)
    .add_property("depth", depth)
    .add_property("de_dx", de_dx)
    .add_property("depth_de_dx", depth_de)
    ;
}
