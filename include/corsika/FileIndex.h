#pragma once
#include <corsika/RawStream.h>
#include <vector>
#include <map>
namespace corsika
{
    struct FileIndex
    {
        int runNumber;
        std::vector<size_t> eventHeaders;
        std::vector<size_t> eventTrailers;
        std::vector<size_t> longBlocks;
        std::map<unsigned int, unsigned int> IDToPosition;
        std::string String();
        
        void Scan(RawStream& stream, bool force);
    };
}
