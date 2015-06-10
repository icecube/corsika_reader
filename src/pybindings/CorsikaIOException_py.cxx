#include <boost/python.hpp>
#include <corsika/CorsikaIOException.h>

using namespace boost::python;
using namespace corsika;


void translate(const CorsikaIOException& e)
{
  PyErr_SetString(PyExc_IndexError, e.what());
  boost::python::throw_error_already_set();
}

void register_CorsikaIOException()
{
  register_exception_translator<CorsikaIOException>(&translate);
}
