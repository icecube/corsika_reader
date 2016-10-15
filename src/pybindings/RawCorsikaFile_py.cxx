#include <corsika/RawCorsikaFile.h>
#include <vector>
#include <boost/python.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>

using namespace boost::python;
using namespace corsika;

//BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(particle_overloads, Corsika::RawFile::GetParticleIt, 0, 1)


struct RawFile_helper
{
  static
  boost::shared_ptr<VRawParticleIterator> GetParticleIt_0(const Corsika::RawFile& self)
  {
    return self.GetVParticleIt();
  }
  static
  boost::shared_ptr<VRawParticleIterator> GetParticleIt_1(const Corsika::RawFile& self, Corsika::RawFile::PositionType start)
  {
    return self.GetVParticleIt(start);
  }

  static
  bool GetNextBlock_th(Corsika::RawFile& self, Corsika::Block<Corsika::Thinned>& theBlock)
  {
    return self.GetNextBlock(theBlock);
  }
  static
  bool GetNextBlock_notth(Corsika::RawFile& self, Corsika::Block<Corsika::NotThinned>& theBlock)
  {
    return self.GetNextBlock(theBlock);
  }
  static
  void Open_str(Corsika::RawFile& self,  const std::string& theName)
  {
    self.Open(theName);
  }

};



void
register_RawFile(std::string name)
{
  docstring_options local_docstring_options(true, true, false);

  typedef  Corsika::RawFile RawFile;
  typedef  RawFile_helper helper;
  class_<RawFile>(name.c_str())
    .def(init<std::string>())
    .def("open", &helper::Open_str)
    .def("close", &RawFile::Close)
    .def("get_next_block", &helper::GetNextBlock_notth)
    .def("get_next_block", &helper::GetNextBlock_th)
    .def("seek_to", &RawFile::SeekTo)
    .add_property("is_open", &RawFile::IsOpen)
    .add_property("is_valid", &RawFile::IsValid)
    .def("get_next_position", &RawFile::GetNextPosition)
    .def("particles", &helper::GetParticleIt_0)
    .def("particles", &helper::GetParticleIt_1)
    ;
}

void register_RawCorsikaFile()
{
  register_RawFile("RawFile");
}
