#ifndef ARC_INCLUDED

#include <cstdlib>
#include <dirent.h>
#include <sys/stat.h>

#include "../dlog/dlog.hpp"
#include "../blaze/blaze.hpp"
#include "../file/file.hpp"

class arc {
public:
    blaze l;

    struct ive {
        char *name_s;
        uint32_t name_n;

        uint8_t *_b;
        uint32_t b_n;
    };

    arc ();
    ~arc ();

    static void ive_destroy (void *_v) {
        ive *_ive = (ive *) _v;
        if (! _ive)
            return;
        if (_ive->name_s)
            free (_ive->name_s);
        if (_ive->_b)
            free (_ive->_b);
        free (_ive);
    }

    ive *add (const char *name_s);
    bool import_dir (const char *dir_s);
    bool export_dir (const char *dir_s);
    void report ();
    bool get (const char *name_s, uint8_t **__b,  uint32_t *_b_n);
    bool load_file (const char *name_s);
    bool load_ser (const uint8_t *_b, uint32_t b_n);
    bool save_file (const char *name_s);
    bool save_ser (uint8_t **__b, uint32_t *_b_n);
};

#define ARC_INCLUDED
#endif

