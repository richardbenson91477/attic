#ifndef FILE_INCLUDED

#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <climits>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "../dlog/dlog.hpp"

class file {
public:
    char file_s [PATH_MAX];
    uint8_t *_b;
    uint32_t b_n;
    
    bool free_;

    bool mapped_;
    bool readonly_;
    bool shared_;
    
    file ();
    file (const char *file_s);
    file (uint8_t *_b, uint32_t b_n);
    ~file ();
    void init ();

    bool map (const char *file_s, uint32_t b_n);
    bool unmap ();
    bool load (const char *file_s);
    bool save ();
    bool save_as (const char *file_s);
    bool _export (const char *file_s);
};

#define FILE_INCLUDED
#endif

