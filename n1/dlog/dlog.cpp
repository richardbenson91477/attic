#include "dlog.hpp"

namespace dlog {
    FILE *f_log = stderr;
}

bool dlog::open_log (const char *file_s) {
    f_log = fopen (file_s, "w");
    if (! f_log)
        return false;

    return true;
}

void dlog::close_log () {
    fclose (f_log);
    f_log = stderr;
}

void dlog::debug (const char *s) {
    #ifndef NO_DEBUG_MSGS
        fprintf (f_log, "> %s\n", s);
        fflush (f_log);
    #endif
}
void dlog::debug (const char *s, const char *s2) {
    #ifndef NO_DEBUG_MSGS
        fprintf (f_log, "> %s: %s\n", s, s2);
        fflush (f_log);
    #endif
}
void dlog::debug (const char *s, const char *s2, const char *s3) {
    #ifndef NO_DEBUG_MSGS
        fprintf (f_log, "> %s: %s: %s\n", s, s2, s3);
        fflush (f_log);
    #endif
}
void dlog::debug (const char *s, const char *s2, const char *s3,
        const char *s4) {
    #ifndef NO_DEBUG_MSGS
        fprintf (f_log, "> %s: %s: %s: %s\n", s, s2, s3, s4);
        fflush (f_log);
    #endif
}
void dlog::debug (const char *s, float f) {
    #ifndef NO_DEBUG_MSGS
        fprintf (f_log, "> %s: %f\n", s, f);
        fflush (f_log);
    #endif
}
void dlog::debug (const char *s, double d) {
    #ifndef NO_DEBUG_MSGS
        fprintf (f_log, "> %s: %lf\n", s, d);
        fflush (f_log);
    #endif
}
void dlog::debug (const char *s, uint32_t u) {
    #ifndef NO_DEBUG_MSGS
        fprintf (f_log, "> %s: %u\n", s, u);
        fflush (f_log);
    #endif
}
void dlog::debug (const char *s, const char *s2, uint32_t u) {
    #ifndef NO_DEBUG_MSGS
        fprintf (f_log, "> %s: %s: %u\n", s, s2, u);
        fflush (f_log);
    #endif
}
void dlog::debug (const char *s, int32_t d) {
    #ifndef NO_DEBUG_MSGS
        fprintf (f_log, "> %s: %d\n", s, d);
        fflush (f_log);
    #endif
}
void dlog::error (const char *s) {
    fprintf (f_log, "ERROR: %s\n", s);
    fflush (f_log);
}
void dlog::error (const char *s, const char *s2) {
    fprintf (f_log, "ERROR: %s: %s\n", s, s2);
    fflush (f_log);
}
void dlog::error (const char *s, float f) {
    fprintf (f_log, "ERROR: %s: %f\n", s, f);
    fflush (f_log);
}
void dlog::error (const char *s, double d) {
    fprintf (f_log, "ERROR: %s: %f\n", s, d);
    fflush (f_log);
}
void dlog::error (const char *s, uint32_t u) {
    fprintf (f_log, "ERROR: %s: %u\n", s, u);
    fflush (f_log);
}
void dlog::error (const char *s, int32_t d) {
    fprintf (f_log, "ERROR: %s: %d\n", s, d);
    fflush (f_log);
}

