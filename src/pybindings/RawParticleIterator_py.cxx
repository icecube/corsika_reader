#include <boost/python.hpp>
#include <corsika/RawParticleIterator.h>
#include <iostream>

using namespace boost::python;
using std::cout;
using std::endl;

struct VRawParticleIterator: public corsika::VRawParticleIterator, boost::python::wrapper<corsika::VRawParticleIterator>
{
  VRawParticleIterator(){}
  VRawParticleIterator(const corsika::VRawParticleIterator& it):corsika::VRawParticleIterator(it){}

  boost::optional<corsika::CorsikaParticle> GetCorsikaParticle() const
  {
    return this->get_override("GetCorsikaParticle")();
  }
  void Rewind()
  {
    this->Rewind();
  }
  bool IsValid() const
  {
    return this->get_override("IsValid")();
  }
};

inline object identity(object const& o) { return o; }

/*
  "ranges" do not make sense for this iterator, so I am doing the minimum necessary to provide a python iterator interface.
 */

template<typename Thinning>
const typename corsika::Corsika::Block<Thinning>::ParticleData&
next_particle(corsika::RawParticleIterator<Thinning>& o)
{
  const typename corsika::Corsika::Block<Thinning>::ParticleData*
    result = o.GetOneParticle();
  if (!result) {
    PyErr_SetString(PyExc_StopIteration, "No more data.");
    boost::python::throw_error_already_set();
  }
  return *result;
}

template<typename Thinning>
const typename corsika::Corsika::Block<Thinning>::ParticleData&
next_particle_2(corsika::VRawParticleIterator& o)
{
  corsika::RawParticleIterator<Thinning>* ptr = dynamic_cast<corsika::RawParticleIterator<Thinning>*>(&o);
  if (!ptr) {
    PyErr_SetString(PyExc_StopIteration, "Could not cast particle iterator.");
    boost::python::throw_error_already_set();
  }
  const typename corsika::Corsika::Block<Thinning>::ParticleData*
    result = ptr->GetOneParticle();
  if (!result) {
    PyErr_SetString(PyExc_StopIteration, "No more data.");
    boost::python::throw_error_already_set();
  }
  return *result;
}

template<typename Thinning>
void Rewind(corsika::VRawParticleIterator& it)
{
  it.Rewind();
}

template <class Thinning>
void register_RawIterator(std::string name)
{
  typedef  corsika::RawParticleIterator<Thinning> RawParticleIterator;
  class_<RawParticleIterator>(name.c_str())
    .def("next", next_particle<Thinning>,
         return_internal_reference<>())
    .def("next", next_particle_2<Thinning>,
         return_internal_reference<>())
    .def("__iter__", identity)
    .def("rewind", &RawParticleIterator::Rewind)
    .def("rewind", Rewind<Thinning>)
    ;

  register_ptr_to_python<boost::shared_ptr<RawParticleIterator> >();
}



void register_RawParticleIterator()
{
  class_<VRawParticleIterator>("VRawIterator")
    .def("rewind", &VRawParticleIterator::Rewind)
    ;

  register_ptr_to_python<boost::shared_ptr<corsika::VRawParticleIterator> >();

  register_RawIterator<corsika::Corsika::NotThinned>("RawParticleIterator");
  register_RawIterator<corsika::Corsika::Thinned>("RawParticleIterator_thin");
}
