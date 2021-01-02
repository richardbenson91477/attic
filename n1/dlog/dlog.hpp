#ifndef DLOG_INCLUDED

#include <cstdio>
#include <cstdint>

// note: not a class
namespace dlog {

    bool open_log (const char *file_s);
    void close_log ();

    void debug (const char *s);
    void debug (const char *s, const char *s2);
    void debug (const char *s, const char *s2, const char *s3);
    void debug (const char *s, const char *s2, const char *s3, const char *s4);
    void debug (const char *s, float f);
    void debug (const char *s, double d);
    void debug (const char *s, uint32_t u);
    void debug (const char *s, const char *s2, uint32_t u);
    void debug (const char *s, int32_t d);

    void error (const char *s);
    void error (const char *s, const char *s2);
    void error (const char *s, float f);
    void error (const char *s, double d);
    void error (const char *s, uint32_t u);
    void error (const char *s, int32_t d);

} // namespace dlog

#define DLOG_INCLUDED
#endif

