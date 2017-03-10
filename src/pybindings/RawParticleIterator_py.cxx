#include <boost/python.hpp>
#include <corsika/RawParticleStream.h>
#include <iostream>

using namespace boost::python;
using std::cout;
using std::endl;

class RawParticleStream: public corsika::VRawParticleStream, public boost::python::wrapper<corsika::VRawParticleStream>
{
public:
  RawParticleStream(){}
  RawParticleStream(const corsika::VRawParticleStream& it):corsika::VRawParticleStream(it){}

  boost::optional<corsika::Particle> NextParticle()
  {
    return this->get_override("GetParticle")();
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
const corsika::ParticleData<Thinning>&
next_particle(corsika::RawParticleStream<Thinning>& o)
{
    const corsika::ParticleData<Thinning>* result = o.GetOneParticle();
  if (!result) {
    PyErr_SetString(PyExc_StopIteration, "No more data.");
    boost::python::throw_error_already_set();
  }
  return *result;
}

template<typename Thinning>
const corsika::ParticleData<Thinning>&
next_particle_2(corsika::VRawParticleStream& o)
{
  corsika::RawParticleStream<Thinning>* ptr = dynamic_cast<corsika::RawParticleStream<Thinning>*>(&o);
  if (!ptr) {
    PyErr_SetString(PyExc_StopIteration, "Could not cast particle iterator.");
    boost::python::throw_error_already_set();
  }
    const corsika::ParticleData<Thinning>* result = ptr->GetOneParticle();
  if (!result) {
    PyErr_SetString(PyExc_StopIteration, "No more data.");
    boost::python::throw_error_already_set();
  }
  return *result;
}

template<typename Thinning>
void Rewind(corsika::VRawParticleStream& it)
{
  it.Rewind();
}

template <class Thinning>
void register_RawIterator(std::string name)
{
  typedef  corsika::RawParticleStream<Thinning> RawParticleStream;
    class_<RawParticleStream>(name.c_str(), init<corsika::RawStreamPtr, size_t>())
    // .def("next", next_particle<Thinning>,
    //      return_internal_reference<>())
    .def("next", next_particle_2<Thinning>,
         return_internal_reference<>())
    .def("__iter__", identity)
    // .def("rewind", &RawParticleIterator::Rewind)
    .def("rewind", Rewind<Thinning>)
    ;

  register_ptr_to_python<boost::shared_ptr<RawParticleStream> >();
}



void register_RawParticleIterator()
{
  class_<RawParticleStream,boost::noncopyable>("VRawIterator")
    .def("rewind", &RawParticleStream::Rewind)
    ;

  register_ptr_to_python<corsika::RawParticleStreamPtr>();

  register_RawIterator<corsika::NotThinned>("RawParticleIterator");
  register_RawIterator<corsika::Thinned>("RawParticleIterator_thin");
}
