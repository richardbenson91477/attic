#ifndef N1_REGN_INCLUDED

class n1_rgn {
public:
    struct rgn {
        // set manually
        int32_t top, left, bottom, right;
        int32_t row_s;
        int32_t col_s;

        // set by init
        int32_t w, h;
        int32_t row_n;
        int32_t col_n;
    };

    uint32_t classify (int32_t x, int32_t y, int32_t *o, rgn *_rgn,
        uint32_t n);
    void render (uint32_t texid, rgn *_texrg, int32_t texo, rgn *_destrg,
        int32_t desto);
    void init (rgn *_rgn, uint32_t n);
    bool getcell (rgn *_rgn, int32_t o, int32_t *top, int32_t *left,
        int32_t *bottom, int32_t *right);
};

#define N1_RGN_INCLUDED
#endif

