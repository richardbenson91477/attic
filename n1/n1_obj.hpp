#ifndef N1_OBJ_INCLUDED

class n1_obj {
public:
    class n1_app *_app;
    n1_obj (n1_app *_app) {
        this->_app = _app;
    }

    struct seq {
        char *name_s;
        uint32_t frame_n;
        float fps;
    };
    struct rkey {
        float rot [3];
        float quat [4];
        uint32_t seq_id;
        uint32_t frame_id;
    };
    struct pkey {
        float pos [3];
        uint32_t seq_id;
        uint32_t frame_id;
    };
    class joint {
    public:
        // relative to parent
        float pos [3], rot [3];
        // relative to parent
        float m_relative [3][4];
        // relative to world
        float m_final [3][4];
        // mrelative + lerp between frames
        float m_interm [3][4];
        joint *parent;
        uint32_t parent_id;
        blaze pkeys;
        uint32_t pkey_n;
        blaze rkeys;
        uint32_t rkey_n;
    
        static void destroy (void *_v) {
            joint *_joint = new joint;
            if (! _joint)
                return;
            _joint->pkeys.destroy (blaze::hook_free);
            _joint->rkeys.destroy (blaze::hook_free);
        }
    };
    struct poly {
        // vertex indices
        uint32_t vert_ids [3];
        // plane/normal
        float n [4];
        // animated vertex pos
        float jv1 [3], jv2 [3], jv3 [3];
        // animated vertex normals
        float jn1 [3], jn2 [3], jn3 [3];
        // points to either vert pos / animated pos
        float *_v3, *_v2, *_v1;
        // points to either vert n / animated n
        float *pn3, *pn2, *pn1;
        // points to vert joint during anim
        joint *_joint1, *_joint2, *_joint3;
        float tex_pos [6];
        uint32_t tex;
        uint32_t tex2;
    };
    struct vert {
        float pos [3];
        float n [4];
        uint32_t joint_id;
        uint32_t tmpi;
    };
    struct otex {
        n1_disp::tex *_tex;
        n1_texshader::texshader *psh;
        uint32_t i1, i2;
    };

    char *file_s;
    vert *_verts;
    uint32_t vert_n;
    poly *_polys;
    uint32_t poly_n;
    otex *_otexs;
    uint32_t otex_n;
    blaze seqs;
    uint32_t seq_n;
    blaze joints;
    uint32_t joint_n;
    uint32_t tex2;
    float brad;
    float pre_sbrad;
    bool anim_;
    // from current sequence
    float anim_t, anim_total, fps;
    uint32_t seq_id, frame_n;

    void render (const float *cam, const float (*mat)[4]);
    void clear ();
    bool load (const char *file_s);
    void destroy ();
    vert *vert_add (vert *_vert, float *pos);
    poly *poly_add (poly *_poly, uint32_t v1, uint32_t v2,
        uint32_t v3);
    bool otex_add (otex *_otex, char *file_s);

    seq *seq_add (char *name_s, uint32_t frame_n, float fps);
    seq *seq_get_by_name (char *name_s);
    static void seq_destroy (void *_v);

    joint *joint_add (float *pos, float *rot, uint32_t parent);

    rkey *rkey_add (joint *_joint, uint32_t seq_id, uint32_t frame_id,
        const float *rot);
    pkey *pkey_add (joint *_joint, uint32_t seq_id, uint32_t frame_id,
        const float *pos);
    rkey *rkey_get (joint *_joint, uint32_t seq_id, uint32_t frame_id);
    pkey *pkey_get (joint *_joint, uint32_t seq_id, uint32_t frame_id);
    void update ();
    void joint_update_all ();
    void normal_update_all ();
    void anim_init (bool t_, uint32_t seq);
    bool anim ();
    void anim_vn (bool normals_);
    bool anim_set_seq (uint32_t id);
};

#define N1_OBJ_INCLUDED
#endif

