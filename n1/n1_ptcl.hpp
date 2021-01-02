#ifndef N1_PTCL_INCLUDED

class n1_ptcl {
public:
    enum {
        PTCL_POINT,
        PTCL_TRI,
        PTCL_QUAD,
        PTCL_OBJECT
    };
    struct ptcl {
        uint32_t type;
        // life span
        float ttl, ttl_range, tick;
        // weight
        float weight, fall_t;
        // pos
        float p [3];
        float p_range [3];
        // scale
        float s [3];
        float s_range [3];    
        float s1 [3];
        float s2 [3];
        int32_t s_scale;
        // direction
        float v [3];
        float v_range [3];
        // color
        float c [4];
        // orig alpha
        float ca;
        // scale color from orig->0 over ttl?
        int32_t c_scale;
        // user defined obj
        n1_obj *_obj;

        ptcl *next;
    };

    blaze *ptcls;

    bool init (bool t_);
    bool add (uint32_t n, uint32_t type, 
        float ttl, float ttl_range, 
        float weight,
        const float *p, const float *p_range, 
        const float *s, const float *s_range, const float *s2, 
        const float *v, const float *v_range, 
        const float *c, int32_t c_scale,
        n1_obj *_obj);
    void tick (float t);
    void render (n1_disp *_disp);
    void destroy_all ();
};

#define N1_PTCL_INCLUDED
#endif

