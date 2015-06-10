#include <boost/python.hpp>
#include <corsika/CorsikaShowerFile.h>
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
  ShowerIterator(CorsikaShowerFile& f):
    fIndex(0),
    fFile(&f)
  {}

  CorsikaShower& next_shower()
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
  CorsikaShowerFile* fFile;
};


ShowerIterator get_shower_iterator(CorsikaShowerFile& f)
{
  return ShowerIterator(f);
}

CorsikaShower&
get_shower(CorsikaShowerFile& f, int i)
{
  f.FindEvent(i);
  return f.GetCurrentShower();
}

void register_CorsikaShowerFile()
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

  CorsikaShower& (CorsikaShowerFile::*get_current)() = &CorsikaShowerFile::GetCurrentShower;
  bool (CorsikaShowerFile::*is_valid)() = &CorsikaShowerFile::IsValid;

  class_<CorsikaShowerFile, boost::noncopyable>("CorsikaShowerFile")
    .def(init<const std::string&, bool>())
    .def(init<const std::string&>())
    .def("open", &CorsikaShowerFile::Open) // would need to overload for the default second parameter (true)
    .def("close", &CorsikaShowerFile::Close)
    .def("find_event", &CorsikaShowerFile::FindEvent)
    .add_property("n_events", &CorsikaShowerFile::GetNEvents)
    .def("events", get_shower_iterator)
    .add_property("is_valid", is_valid)
    //.staticmethod("IsValid")
    .add_property("current_shower", make_function(get_current, return_internal_reference<>()))
    .def("shower", get_shower, return_internal_reference<>())
    ;
}
