#include <corsika/particle/ParticleProperties.h>
#include <corsika/particle/NucleusProperties.h>
#include <corsika/CorsikaParticle.h>
#include <boost/python.hpp>

using namespace boost::python;
using namespace corsika;

struct VParticlePropertiesBase
  : VParticleProperties, wrapper<VParticleProperties>
{
  int GetType() const { return this->get_override("type")(); }
  std::string GetName() const { return this->get_override("name")(); }
  double GetMass() const { return this->get_override("mass")(); }
 };

void register_ParticleProperties()
{
  class_<VParticlePropertiesBase, boost::noncopyable>("VParticleProperties");

  class_<ParticleProperties, bases<VParticleProperties> >("ParticleProperties", init<const int, const std::string&, const double>())
    .add_property("code", &ParticleProperties::GetType)
    .add_property("name", &ParticleProperties::GetName)
    .add_property("mass", &ParticleProperties::GetMass)
    ;
  class_<NucleusProperties, bases<VParticleProperties> >("NucleusProperties", init<int>())
    .add_property("code", &NucleusProperties::GetType)
    .add_property("name", &NucleusProperties::GetName)
    .add_property("mass", &NucleusProperties::GetMass)
    ;
}
