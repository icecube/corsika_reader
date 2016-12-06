#include <corsika/RawStream.h>
#include <vector>
#include <map>
namespace corsika
{
    struct FileIndex
    {
        int runNumber;
        std::vector<unsigned long int> eventHeaders;
        std::vector<unsigned long int> eventTrailers;
        std::vector<unsigned long int> longBlocks;
        std::map<unsigned int, unsigned int> IDToPosition;
        std::string String();
        
        void Scan(VRawStream& stream, bool force);
    };
}
