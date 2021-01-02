#ifndef NITRO_RAY_INCLUDED

#include <deque>
//#include <wait.h>

#include "../math/math.hpp"

#define FTOB(x) ((uint8_t)((x) * 255.0f))
#define BTOF(x) ((float)(x) / 255.0f)

class ray { public:
    uint32_t w, h;
    float eye_v [3];
    float *_rays;

    float trace_rec_n;
    float trace_rec_max; 

    struct shape {
        // 0 = sphere, 1 = poly
        uint8_t type;
        float v [3];

        float diffuse [3];
        float specular [3];
        float specular_exp;
        float reflectivity;

        float rad, rad_sq;
        uint32_t tex;
        
        float v1 [3], v2 [3], v3 [3];
        float n [3];
    };

    struct light {
        float v [3];
        float c [3];
    };
    float ambient [3];

    std::deque<shape *> shapes;
    std::deque<light *> lights;
    
    ray ();
    ~ray ();
    bool _;

    void pre_init ();
    void render (uint32_t *draw_p);
    bool calc_rays ();
    bool ray_collide (const float *pos, const float *ray,
        const struct shape *ps, float *d);
    bool trace (const struct shape *_shape_src, const float *pos,
        const float *ray, float *lsh);
};

#define NITRO_RAY_INCLUDED
#endif

