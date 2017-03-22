#include <boost/python.hpp>
#include <corsika/LongFile.h>
#include <string>

using namespace boost::python;
using namespace corsika;

void register_LongFile()
{
  class_<LongFile>("LongFile", no_init)
    .def(init<const std::string&>())
    .def(init<const std::string&, float>())
    .add_property("size", &LongFile::size)
    .add_property("dx", &LongFile::Dx)
    .add_property("has_particle_profile", &LongFile::HasParticleProfile)
    .add_property("has_energy_deposit", &LongFile::HasEnergyDeposit)
    .add_property("is_slant_depth", &LongFile::IsSlantDepth)
    .def("get_profile", &LongFile::GetProfile)
    ;
}
