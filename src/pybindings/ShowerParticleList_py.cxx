#include <sstream>
#include <corsika/CorsikaShowerFileParticleIterator.h>
#include <corsika/ShowerParticleList.h>
#include <boost/python.hpp>


using namespace boost::python;
using namespace corsika;


void register_ShowerParticleList()
{

  class_<ShowerParticleList>("ShowerParticleList")
    .def("__iter__", &ShowerParticleList::begin)
    ;

}
