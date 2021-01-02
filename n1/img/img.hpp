#ifndef IMG_INCLUDED

#include <cstdlib>
#include <cstdint>

#include <png.h>

#include "../dlog/dlog.hpp"
#include "../arc/arc.hpp"

class img {
public:
    int32_t w, h;
    uint8_t *_b;

    bool free_;

    img ();
    ~img ();

    bool load_rwh (arc *_arc, const char *_s);
    bool load_rwh (const char *file_s);
    bool load_raw (arc *_arc, const char *file_s, int32_t w, int32_t h);
    bool load_raw (const char *file_s, int32_t w, int32_t h);
    bool load_png (const char *file_s);
    void swap_r_b ();

    static void destroy (void *_v) {
        img *_img = (img *)_v;
        if (_img)
            delete _img;
    }
};

#define IMG_INCLUDED
#endif

