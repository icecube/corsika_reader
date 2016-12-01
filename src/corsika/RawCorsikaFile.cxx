/**
 \file
 Implement raw CORSIKA file
 
 \author Lukas Nellen
 \version $Id$
 \date 08 Dec 2003
 */

#include <corsika/RawCorsikaFile.h>

using namespace corsika;

RawFile::RawFile(): fFromFile(true){}

RawFile::RawFile(const std::string& theName): fFromFile(true)
{
    Open(theName);
}
RawFile::RawFile(const RawFile& other): fFromFile(other.fFromFile)
{
    Close();
    if (fFromFile)
    {
        Open(other.fName);
        fCorsikaStream->SeekTo(other.GetNextPosition());
    }
}

RawFile& RawFile::operator=(const RawFile& other)
{
    Close();
    if (fFromFile)
    {
        Open(other.fName);
        fCorsikaStream->SeekTo(other.GetNextPosition());
    }
    return *this;
}

void RawFile::Open(const std::string& theName)
{
    fName = theName;
    fCorsikaStream = RawStreamFactory::Create(theName);
    if (!fCorsikaStream)
        throw CorsikaIOException("failed to initialize CORSIKA stream from " + fName);
}
