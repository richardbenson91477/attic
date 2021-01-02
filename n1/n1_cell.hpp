#ifndef N1_CELL_INCLUDED

class n1_cell {
public:
    class n1_app *_app;

    struct minpoly {
        float *_v1, *_v2, *_v3;
        float n [4];
    };
    struct cell {
        float min_x, max_x;
        float min_y, max_y;
        float min_z, max_z;
        uint32_t poly_n;
        float *_tnvs;
        minpoly *_mpolys;
        uint32_t otex_n;
        n1_obj::otex *_otexs;
        uint32_t block_n;
        float *bcenter;
        float *bsphere;
    };

    cell *cells;
    uint32_t cell_n;

    static const uint32_t poly_block_n = 4;
    
    n1_cell ();
    uint32_t classify (const float *v);
    uint32_t classify2 (const float *v, uint32_t cell_c);
    bool load (const char *file_s);
    void destroy_all ();
    void render ();
};

#define N1_CELL_INCLUDED
#endif

