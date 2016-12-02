#include <stddef.h>
struct FileStream
{
    const bool seekable;
    FileStream(bool seekable): seekable(seekable) {}
    virtual ~FileStream(){}
    virtual long read(size_t num, void* bytes) = 0; // Returns number of bytes read, <0 is returned for some errors
    virtual bool seek(size_t absolute_position) = 0; // Returns false on failure
    static FileStream* open(const char* filename); // Returns null on failure
};
