#include <corsika/Logging.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

namespace corsika
{
    void _log(unsigned level, const char* file, int line, const char* func, const char* format, ...)
    {
        const char* levels[] = {"WARN", "ERROR" ,"FATAL"};
        printf("%s: %s:%i %s: ", levels[level], file, line, func);
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
        printf("\n");
        if (level == 2) exit(1); // fatal
    }
}
