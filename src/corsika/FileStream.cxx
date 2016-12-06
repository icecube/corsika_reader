#include <corsika/FileStream.h>
#include <stdio.h>
#include <string.h>
#include <zlib.h>
#include <bzlib.h>
#include <sys/stat.h>

struct RawFileStream: FileStream
{
    FILE* file;
    RawFileStream(FILE* file, bool seekable): FileStream(seekable), file(file) {}
    ~RawFileStream()
    {
        fclose(file);
    }
    long read(size_t num, void* bytes)
    {
        return fread(bytes, 1, num, file);
    }
    bool seek(size_t absolute_position)
    {
        return fseek(file, absolute_position, SEEK_SET) == 0;
    }
};
struct GzFileStream: FileStream
{
    gzFile file;
    GzFileStream(gzFile file, bool seekable): FileStream(seekable), file(file) {}
    ~GzFileStream()
    {
        gzclose(file);
    }
    long read(size_t num, void* bytes)
    {
        return gzread(file, bytes, (unsigned)num);
    }
    bool seek(size_t absolute_position)
    {
        return gzseek(file, absolute_position, SEEK_SET) != -1;
    }
};
struct Bz2FileStream: FileStream
{
    BZFILE* f;
    Bz2FileStream(BZFILE* f): FileStream(false), f(f) {}
    ~Bz2FileStream()
    {
        BZ2_bzclose(f);
    }
    long read(size_t num, void* bytes)
    {
        return BZ2_bzread(f, bytes, (int)num);
    }
    bool seek(size_t absolute_position)
    {
        return false;
    }
};

static bool ends_with(const char* string, const char* postfix)
{
    size_t l0 = strlen(string);
    size_t l1 = strlen(postfix);
    if (l0 >= l1) return memcmp(string + l0 - l1, postfix, l1) == 0;
    return false;
}

FileStream* FileStream::open(const char* filename)
{
    struct stat s;
    if (stat(filename, &s)) return 0; // failed to stat file
    bool seekable = !S_ISFIFO(s.st_mode) && !S_ISSOCK(s.st_mode);
    if (ends_with(filename, ".gz"))
    {
        if (gzFile f = gzopen(filename, "r"))
            return new GzFileStream(f, seekable);
    }
    else if (ends_with(filename, ".bz2"))
    {
        if (BZFILE* f = BZ2_bzopen(filename, "r"))
            return new Bz2FileStream(f);
    }
    else
    {
        if (FILE* f = fopen(filename, "rb"))
            return new RawFileStream(f, seekable);
    }
    return 0;
}
