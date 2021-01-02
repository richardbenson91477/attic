#ifndef N1_DISPLAY_INCLUDED

class n1_disp {
public:
    class n1_app *_app;

    float eye_pos [3];
    float eye_theta, eye_phi;
    uint32_t eye_cell;

    float view_l, view_r, view_t, view_b, view_zn, view_zf;
    float view_n [4][3];

    int32_t fade_dir;
    float fade_r, fade_a, fade_a2;

    float light_pos [4], light_dif [4], light_amb [4], light_spec [4];
    float light_spot_dir [4], light_spot_exp, light_spot_cutoff;
    float light_atten_c, light_atten_l, light_atten_q;
    uint32_t light_cell;
   
    struct tex {
        char *name_s;
        uint32_t gl_texid;
        uint32_t ref_n;
    };
    blaze textures;
    uint32_t tex_white;

    bool init (bool t_);
    void tick (float t);
    void render ();
    void perspective ();
    void ortho (bool t_, float w_f, float h_f);
    void ortho (bool t_);
    void cam ();

    void mview_push ();
    void mview_pop ();
    void mview_reset ();
    void mview_trans (float x, float y, float z);
    void mview_trans (const float *v);
    void mview_rot (float x, float y, float z);

    void blend (uint32_t type);
    void fade (float alpha1, float alpha2, float speed);
    void fade_render ();
    float clip (const float *pos);
    void color3 (float r, float g, float b);
    void color4 (float r, float g, float b, float a);
    void color4v (const float *c);
    void clear ();
    void fill (const float *c);

    void light_defaults ();
    void light_apply ();
    void light_apply_pos ();

    void tex_bind (uint32_t id);
    void tex_render (uint32_t id, int32_t x, int32_t y, int32_t w, int32_t h);
    tex *tex_load (const char *name_s);
    tex *tex_get (const char *name_s);
    void tex_del (tex *_tex);
    static void tex_destroy (void *_v);
};

#define N1_DISPLAY_INCLUDED
#endif

