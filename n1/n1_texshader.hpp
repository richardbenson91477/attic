#ifndef N1_SHADER_INCLUDED

class n1_texshader {
public:
    class n1_app *_app;

    struct r_key {
        float rot;
        float rpos [2];
        uint32_t frame;
    };
    struct p_key {
        float pos [3];
        uint32_t frame;
    };
    struct s_key {
        float scale [2];
        uint32_t frame;
    };
    struct t_key {
        uint32_t tex_id;
        uint32_t frame;
    };

    struct texshader {
        char *name_s;
        uint32_t ref_c;

        uint32_t frame_n;
        float fps;
        float a_n;
        uint32_t blend_type;
        uint32_t tex_def;

        r_key *r_keys;
        p_key *p_keys;
        s_key *s_keys;
        t_key *t_keys;
        uint32_t r_key_n, p_key_n, s_key_n, t_key_n;

        float a_t;
        float rot;
        float rpos [2];
        float pos [3];
        float scale [2];
        uint32_t tex_id;
        uint32_t paused;
    };
    blaze texshaders;
    
    bool init (bool t_);
    void tick (float f);
    void apply (texshader *_sh);
    void unapply (texshader *_sh);
    texshader *load (const char *file_s);
    texshader *get (const char *file_s);
    void del (texshader *_sh);
    void destroy_all ();
    static void texshader_destroy (void *_v);
};

#define N1_SHADER_INCLUDED
#endif

