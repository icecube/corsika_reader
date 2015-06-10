/**
   \file
   Implement raw CORSIKA file

   \author Lukas Nellen
   \version $Id$
   \date 08 Dec 2003
*/


#include <corsika/RawCorsikaFile.h>
#include <corsika/CorsikaBlock.h>

#include <string>
#include <sstream>
#include <iostream>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/operations.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>

using namespace std;
using std::string;
using namespace corsika::Corsika;

#define ERROR(mess) cerr << mess << endl;

RawFile::RawFile():
  fFromFile(true)
{}

RawFile::RawFile(const std::string& theName):
  fFromFile(true)
{
  Open(theName);
}


RawFile::RawFile(std::istream& in, Corsika::Compression c):
  fFromFile(false)
{
  Open(in, c);
}


RawFile::RawFile(const RawFile& other):
  fFromFile(other.fFromFile)
{
  Close();
  if (fFromFile) {
    Open(other.fName);
    fCorsikaStream->SeekTo(other.GetNextPosition());
  }
}


RawFile& RawFile::operator=(const RawFile& other)
{
  Close();
  if (fFromFile) {
    Open(other.fName);
    fCorsikaStream->SeekTo(other.GetNextPosition());
  }
  return *this;
}


void
RawFile::Open(const std::string& theName)
{
  fName = theName;
  fCorsikaStream = RawStreamFactory::Create(theName);
  if (!fCorsikaStream) {
    ostringstream msg;
    msg << "failed to initialize CORSIKA stream from " << fName;
    throw CorsikaIOException(msg.str());
  }
}


void
RawFile::Open(std::istream& in, Corsika::Compression c)
{
  fName = "stream";
  fCorsikaStream = RawStreamFactory::Create(in, false, c);
  if (!fCorsikaStream) {
    ostringstream msg;
    msg << "failed to initialize CORSIKA stream from " << fName;
    throw CorsikaIOException(msg.str());
  }
}



// Configure (x)emacs for this file ...
// Local Variables:
// mode:c++
// compile-command: "make -C .. -k"
// End:
