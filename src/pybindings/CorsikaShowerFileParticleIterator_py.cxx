#include <boost/python.hpp>
#include <corsika/CorsikaShowerFileParticleIterator.h>


using namespace boost::python;
using namespace corsika;

inline object identity(object const& o) { return o; }

/*
  "ranges" do not make sense for this iterator, so I am doing the minimum necessary to provide a python iterator interface.
 */
CorsikaParticle&
next_particle(CorsikaShowerFileParticleIterator& o)
{
  static CorsikaShowerFileParticleIterator invalid;
  if (o == invalid) {
    PyErr_SetString(PyExc_StopIteration, "No more data.");
    boost::python::throw_error_already_set();
  }
  boost::optional<CorsikaParticle&> r(o.dereference());
  o.increment();

  // the following should not be needed. bug.
  // It happens because the particle block can have a bunch of trailing particles with all zero
  // so one can dereference and still have a valid raw iterator pointing to one of these
  // the iterator becomes invalid after incrementing then.
  if (o->PDGCode() == CorsikaParticle::eUndefined) {
    PyErr_SetString(PyExc_StopIteration, "No more data.");
    boost::python::throw_error_already_set();
  }

  return r.get();
}


void register_CorsikaShowerFileParticleIterator()
{

  class_<CorsikaShowerFileParticleIterator>("CorsikaShowerFileParticleIterator")
    .def("next", next_particle,
         return_internal_reference<>())
    .def("__iter__", identity)
    .def("rewind", &CorsikaShowerFileParticleIterator::Rewind)
    ;

}
