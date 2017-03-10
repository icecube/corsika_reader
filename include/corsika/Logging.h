#pragma once

namespace corsika
{
    void _log(unsigned level, const char* file, int line, const char* func, const char* format, ...);
}

// Placeholder logging system. These macros can be later changed to use I3Logging
#define corsika_log_info(format, ...) corsika::_log(0, __FILE__, __LINE__, __PRETTY_FUNCTION__, format, __VA_ARGS__)
#define corsika_log_error(format, ...) corsika::_log(1, __FILE__, __LINE__, __PRETTY_FUNCTION__, format, __VA_ARGS__)
#define corsika_log_fatal(format, ...) corsika::_log(2, __FILE__, __LINE__, __PRETTY_FUNCTION__, format, __VA_ARGS__)
