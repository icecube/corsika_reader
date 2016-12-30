#include <sstream>
#include <corsika/CorsikaShowerFileParticleIterator.h>
#include <corsika/ShowerParticleList.h>
#include <boost/python.hpp>


using namespace boost::python;
using namespace corsika;

inline object identity(object const& o) { return o; }

const CorsikaParticle& next(ShowerParticleList& s)
{
    boost::optional<CorsikaParticle> p = s.NextParticle();
    if (!p)
    {
        PyErr_SetString(PyExc_StopIteration, "No more data.");
        boost::python::throw_error_already_set();
    }
    return *p;
}


void register_ShowerParticleList()
{

  class_<ShowerParticleList>("ShowerParticleList")
    .def("__iter__", identity)
    .def("next", next, return_internal_reference<>())
    ;

}
