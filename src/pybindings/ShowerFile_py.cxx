#include <boost/python.hpp>
#include <corsika/ShowerFile.h>
#include <string>

using namespace boost::python;
using namespace corsika;


inline object identity(object const& o) { return o; }

class ShowerIterator {
public:
  ShowerIterator():
    fIndex(0),
    fFile(0)
  {}
  ShowerIterator(ShowerFile& f):
    fIndex(0),
    fFile(&f)
  {}

  Shower& next_shower()
  {
    ++fIndex;
    if (fFile->FindEvent(fIndex) != eSuccess) {
      PyErr_SetString(PyExc_StopIteration, "No more data.");
      boost::python::throw_error_already_set();
    }
    return fFile->GetCurrentShower();
  }
private:
  int fIndex;
  ShowerFile* fFile;
};


ShowerIterator get_shower_iterator(ShowerFile& f)
{
  return ShowerIterator(f);
}

Shower&
get_shower(ShowerFile& f, int i)
{
  f.FindEvent(i);
  return f.GetCurrentShower();
}

void register_ShowerFile()
{

  enum_<Status>("Status")
    .value("eSuccess", eSuccess)
    .value("eFail", eFail)
    .value("eEOF", eEOF)
    ;

  class_<ShowerIterator>("ShowerIterator")
    .def("__iter__", identity)
    .def("next", &ShowerIterator::next_shower, return_internal_reference<>())
    ;

  Shower& (ShowerFile::*get_current)() = &ShowerFile::GetCurrentShower;

  class_<ShowerFile, boost::noncopyable>("ShowerFile")
    .def(init<const std::string&, bool>())
    .def(init<const std::string&>())
    .def("open", &ShowerFile::Open) // would need to overload for the default second parameter (true)
    .def("close", &ShowerFile::Close)
    .def("find_event", &ShowerFile::FindEvent)
    .add_property("run_header", make_function(&ShowerFile::GetRunHeader, return_internal_reference<>()))
    .add_property("n_events", &ShowerFile::GetNEvents)
    .def("events", get_shower_iterator)
    //.staticmethod("IsValid")
    .add_property("current_shower", make_function(get_current, return_internal_reference<>()))
    .def("shower", get_shower, return_internal_reference<>())
    ;
}
