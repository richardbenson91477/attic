#ifndef N1_PHYS_INCLUDED

class n1_phys {
public:
    class n1_app *_app;

    float wall_eps;

    // base ent types
    enum {
        TYPE_IGNORE = 0,
        TYPE_CELL
    };

    struct ent {
        // 0 = ignore physics
        uint32_t type;
        float pos [3];
        float mov_v [3];
        float mov_rho;

        bool rotate_;
        float rmat [3][4];

        n1_obj *_obj;

        bool jumping;
        float jump_r;
        float jump_t;

        bool falling;
        float fall_t;
        float weight;

        void *parent;
        uint32_t cell;

        bool dont_render;
        bool marlo;
        bool bound_only;
    };
    blaze ents;

    enum {
        COL_MOVE = 1,
        COL_HEAD,
        COL_GROUND,
        COL_FALL
    };

    struct ecol {
        // for ray
        float isec [3];
        // normal of poly hit or clear
        float pn_hit [3];
        // set by collide to describe target ent
        ent *_hit_ent;
        uint32_t hit_type;
        void *hit_parent;
        // set for callbacks
        ent *_ent;
        uint32_t col_type;
    };

    struct cb_table_t {
        void (*callback)(const ecol *_col);
    };
    cb_table_t cb_table [256];
    
    bool init (bool t_);
    ent *ent_add ();
    bool collide_all (const float *p, const float r, const float *ray,
        ecol *_col, ent *ent_skip);
    bool collide_ray_all (const float *p, const float *ray,
        ecol *_col, ent *ent_skip);
    bool tick (float f);
    bool move (ent *_ent, float *dir);
    void jump (ent *_ent, float t);
    void fall (ent *_ent, float t);
    void callback (uint32_t type, ecol *_col);
    void set_callback (uint32_t type, void (*callback)(const ecol *_col));
};

#define N1_PHYS_INCLUDED
#endif

