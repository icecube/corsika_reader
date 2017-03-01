#include <boost/python.hpp>
#include <corsika/LongFile.h>
#include <string>

using namespace boost::python;
using namespace corsika;

void register_CorsikaLongFile()
{
  class_<CorsikaLongFile>("CorsikaLongFile", no_init)
    .def(init<const std::string&>())
    .def(init<const std::string&, float>())
    .add_property("size", &CorsikaLongFile::size)
    .add_property("dx", &CorsikaLongFile::Dx)
    .add_property("has_particle_profile", &CorsikaLongFile::HasParticleProfile)
    .add_property("has_energy_deposit", &CorsikaLongFile::HasEnergyDeposit)
    .add_property("is_slant_depth", &CorsikaLongFile::IsSlantDepth)
    .def("get_profile", &CorsikaLongFile::GetProfile)
    ;
}
