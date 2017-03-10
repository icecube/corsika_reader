#include <boost/python.hpp>
#include <corsika/ShowerParticleStream.h>


using namespace boost::python;
using namespace corsika;

inline object identity(object const& o) { return o; }

/*
  "ranges" do not make sense for this iterator, so I am doing the minimum necessary to provide a python iterator interface.
 */
Particle& next_particle(ShowerParticleStream& o)
{
    boost::optional<Particle> p = o.NextParticle();

  // the following should not be needed. bug.
  // It happens because the particle block can have a bunch of trailing particles with all zero
  // so one can dereference and still have a valid raw iterator pointing to one of these
  // the iterator becomes invalid after incrementing then.
  if (!p || p->PDGCode() == Particle::eUndefined) {
    PyErr_SetString(PyExc_StopIteration, "No more data.");
    boost::python::throw_error_already_set();
  }
  return *p;
}


void register_CorsikaShowerFileParticleIterator()
{

  class_<ShowerParticleStream>("ShowerParticleStream")
    .def("next", next_particle, return_internal_reference<>())
    .def("__iter__", identity)
    .def("rewind", &ShowerParticleStream::Rewind)
    ;

}
