/**
   \file
   On-disk block of raw CORSIKA files

   \author Lukas Nellen
   \version $Id$
   \date 19 Nov 2003
*/

static const char CVSId[] =
"$Id$";

#include <corsika/CorsikaBlock.h>

#include <string>
#include <sstream>
#include <iostream>
#include <exception>

#include <cstddef>
#include <cstdlib>
#include <string.h>             // the C-language header

using namespace corsika::Corsika;
using std::ostringstream;
using std::endl;
using std::string;
using std::exit;
using std::cerr;

#define ERROR(mess) cerr << mess << endl;
#define FATAL(mess) cerr << mess << endl;

void
BlockID::SetID(const char* const theID)
{
  if (strlen(theID) != kLength) {
    ostringstream msg;
    msg << "Length of string \"" << theID
        << "\" is different from " << kLength;
    FATAL(msg);
    throw std::exception();
  }

  strncpy(fID, theID, kLength);
}


bool
BlockID::Is(const char* const theID)
  const
{
  return strncmp(fID, theID, kLength) == 0;
}


template class Block<Thinned>;
template class Block<NotThinned>;

// Configure (x)emacs for this file ...
// Local Variables:
// mode: c++
// compile-command: "make -C .. -k"
// End:
