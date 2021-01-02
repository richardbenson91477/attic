#include "n1.hpp"

bool n1_disp::init (bool t_) {
    float fl [4] = {0.0, 0.0, 0.0, 1.0};
    n1_disp::tex *_tex;

    if (t_) {
        dlog::debug ("n1_disp::init (true)");
        this->fade_dir = 0;
        
        this->light_defaults ();

        this->view_l = -0.0096;
        this->view_r = -this->view_l;
        this->view_b = -0.006;
        this->view_t = -this->view_b;
        this->view_zn = 0.01;
        this->view_zf = 200.0;

        glMatrixMode (GL_PROJECTION);
        glLoadIdentity ();
        perspective ();
        
        glMatrixMode (GL_MODELVIEW);
        glLoadIdentity ();
    
        glPointSize (2.0);
        glColor4f (1.0, 1.0, 1.0, 1.0);

        glActiveTextureARB (GL_TEXTURE0_ARB);
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
        glEnable (GL_TEXTURE_2D);
 
        glEnable (GL_LIGHTING);
        glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, 1);
        fl [0] = fl [1] = fl [2] = 0.8;
        glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, fl);
        glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, fl);
        glMaterialfv (GL_FRONT_AND_BACK, GL_SHININESS, fl);
        glColorMaterial (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    
        glEnable (GL_CULL_FACE);
        glEnable (GL_DEPTH_TEST);
        glDepthFunc (GL_LESS);
        glEnable (GL_ALPHA_TEST);
        glAlphaFunc (GL_GREATER, 0.0);
    
        if (! (_tex = tex_load ("M3E_WHITE")))
            return false;
        this->tex_white = _tex->gl_texid;

        glActiveTextureARB (GL_TEXTURE1_ARB);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
            GL_LINEAR_MIPMAP_LINEAR);
        glTexGenf (GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
        glTexGenf (GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
        glEnable (GL_TEXTURE_GEN_S);
        glEnable (GL_TEXTURE_GEN_T);
        glActiveTextureARB (GL_TEXTURE0_ARB);

        glShadeModel (GL_SMOOTH);
    }
    else {
        dlog::debug ("n1_disp::init (false)");
        this->textures.destroy (n1_disp::tex_destroy);
    }
    return true;
}

void n1_disp::tick (float t) {
    if (this->fade_dir > 0) {
        this->fade_a += this->fade_r * t;
        if (this->fade_a >= this->fade_a2) { 
            this->fade_dir = 0; 
            this->fade_a = this->fade_a2;
        }
    }
    else {
        this->fade_a -= this->fade_r * t;
        if (this->fade_a <= this->fade_a2) {
            this->fade_dir = 0;
            this->fade_a = this->fade_a2;
        }
    }

    if (this->fade_a == this->fade_a2)
        this->fade_dir = 0;
}

void n1_disp::render () {
    if (this->fade_dir)
        fade_render ();
}

void n1_disp::perspective () {
    float p [3][3];
    float a [3], b [3];

    glFrustum (this->view_l, this->view_r, this->view_b, this->view_t,
        this->view_zn, this->view_zf);

    // calc view planes
    //left
    p[0][0] = 0.0; p[0][1] = 0.0; p[0][2] = 0.0;
    p[1][0] = this->view_l;
    p[1][1] = this->view_t;
    p[1][2] = this->view_zn;
    p[2][0] = this->view_l;
    p[2][1] = this->view_b;
    p[2][2] = this->view_zn;
    math::v_sub2 (a, p[0], p[1]);
    math::v_sub2 (b, p[2], p[1]);
    math::v_cross (this->view_n[0], a, b);
    math::v_norm (this->view_n[0]);

    //right
    p[0][0] = 0.0; p[0][1] = 0.0; p[0][2] = 0.0;
    p[1][0] = this->view_r;
    p[1][1] = this->view_b;
    p[1][2] = this->view_zn;
    p[2][0] = this->view_r;
    p[2][1] = this->view_t;
    p[2][2] = this->view_zn;
    math::v_sub2 (a, p[0], p[1]);
    math::v_sub2 (b, p[2], p[1]);
    math::v_cross (this->view_n[1], a, b);
    math::v_norm (this->view_n[1]);

    //bottom
    p[0][0] = 0.0; p[0][1] = 0.0; p[0][2] = 0.0;
    p[1][0] = this->view_l;
    p[1][1] = this->view_b;
    p[1][2] = this->view_zn;
    p[2][0] = this->view_r;
    p[2][1] = this->view_b;
    p[2][2] = this->view_zn;
    math::v_sub2 (a, p[0], p[1]);
    math::v_sub2 (b, p[2], p[1]);
    math::v_cross (this->view_n[2], a, b);
    math::v_norm (this->view_n[2]);

    //top
    p[0][0] = 0.0; p[0][1] = 0.0; p[0][2] = 0.0;
    p[1][0] = this->view_r;
    p[1][1] = this->view_t;
    p[1][2] = this->view_zn;
    p[2][0] = this->view_l;
    p[2][1] = this->view_t;
    p[2][2] = this->view_zn;
    math::v_sub2 (a, p[0], p[1]);
    math::v_sub2 (b, p[2], p[1]);
    math::v_cross (this->view_n[3], a, b);
    math::v_norm (this->view_n[3]);
}

void n1_disp::ortho (bool t_, float w_f, float h_f) {
    if (t_) {
        glDisable (GL_LIGHTING);
        glDisable (GL_DEPTH_TEST);
    
        glMatrixMode (GL_PROJECTION);
        glPushMatrix ();
        glLoadIdentity ();

        glMatrixMode (GL_MODELVIEW);
        glPushMatrix ();
        glLoadIdentity ();

        glOrtho (0.0, w_f, h_f, 0.0, -1.0, 1.0);
        glColor4f (1.0, 1.0, 1.0, 1.0);
    }
    else {
        glEnable (GL_LIGHTING);
        glEnable (GL_DEPTH_TEST);

        glMatrixMode (GL_PROJECTION);
        glPopMatrix ();
        glMatrixMode (GL_MODELVIEW);
        glPopMatrix ();
    }
}

void n1_disp::ortho (bool t_) {
    this->ortho (t_, 1.0, 1.0);
}

void n1_disp::cam () {
    glLoadIdentity ();
 
    glRotatef (-DEGREES(this->eye_phi - PI12), 1.0, 0.0, 0.0);
    glRotatef (-DEGREES(this->eye_theta), 0.0, 1.0, 0.0);
    glTranslatef (-this->eye_pos[0], -this->eye_pos[1], -this->eye_pos[2]);
}

void n1_disp::mview_push () {
    glPushMatrix ();        
}

void n1_disp::mview_pop () {
    glPopMatrix ();
}

void n1_disp::mview_reset () {
    glLoadIdentity();
}

void n1_disp::mview_trans (float x, float y, float z) {
    glTranslatef (x, y, z);
}

void n1_disp::mview_trans (const float *v) {
    glTranslatef (v[0], v[1], v[2]);
}

// gimbal lock hazard (and not because of lacking quaterions)
void n1_disp::mview_rot (float x, float y, float z) {
    glRotatef (DEGREES(x), 1.0, 0.0, 0.0);
    glRotatef (DEGREES(z), 0.0, 0.0, 1.0);
    glRotatef (DEGREES(y), 0.0, 1.0, 0.0);
}

void n1_disp::blend (uint32_t type) {
    switch (type) {
        case 0:
            glDisable (GL_BLEND);
            break;

        case 1:
            glEnable (GL_BLEND);
            glBlendFunc (GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
            break;

        case 2:
            glEnable (GL_BLEND);
            glBlendFunc (GL_SRC_ALPHA, GL_ONE);
            break;
    };
}

void n1_disp::fade (float alpha1, float alpha2, float speed) {
    this->fade_a = alpha1;
    this->fade_a2 = alpha2;
    this->fade_r = speed;

    if (this->fade_a > this->fade_a2)
        this->fade_dir = -1;
    else
        this->fade_dir = 1;
}

void n1_disp::fade_render () {
    this->ortho (true, 1.0, 1.0);

    glDisable (GL_TEXTURE_2D);
    this->blend (1);
    glColor4f (0.0, 0.0, 0.0, this->fade_a);

    glBegin (GL_POLYGON);
    glVertex2f (0.0, 0.0);
    glVertex2f (0.0, 1.0);
    glVertex2f (1.0, 1.0);
    glVertex2f (1.0, 0.0);
    glEnd ();

    this->blend (false);
    glEnable (GL_TEXTURE_2D);

    this->ortho (false, 0.0, 0.0);
    glColor4f (1.0, 1.0, 1.0, 1.0);
}

// return distance of point to 4 view clip planes
float n1_disp::clip (const float *pos) {
    float res [3];
    float m [3][4];
    float ec [3];
    static float t [3] = {0.0, 0.0, 0.0};
    static float p [3] = {0.0, 0.0, 0.0};
    float d;

    // convert to camera space
    t[1] = -this->eye_theta;
    p[0] = -this->eye_phi + PI12;

    math::v_sub2 (ec, pos, this->eye_pos);

    math::m_from_r (m, t);
    math::v_rotate_m2 (res, ec, m);
    math::v_copy (ec, res);

    math::m_from_r (m, p);
    math::v_rotate_m2 (res, ec, m);
    math::v_copy (ec, res);

    d = math::v_dot (ec, this->view_n[0]);
    if (d > 0)
        d = math::v_dot (ec, this->view_n[1]);
    if (d > 0)
        d = math::v_dot (ec, this->view_n[2]);
    if (d > 0)
        d = math::v_dot (ec, this->view_n[3]);

    return d;
}

void n1_disp::color4 (float r, float g, float b, float a) {
    glColor4f (r, g, b, a);
}

void n1_disp::color3 (float r, float g, float b) {
    glColor3f (r, g, b);
}

void n1_disp::color4v (const float *c) {
    glColor4fv (c);
}

void n1_disp::clear () {
    glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);        
}

void n1_disp::fill (const float *c) {
    glClearColor (c[0], c[1], c[2], 0.0);
}

void n1_disp::light_defaults () {
    this->light_pos[0] = this->light_pos[1] = this->light_pos[2] = 0.0;
    this->light_dif[0] = this->light_dif[1] = this->light_dif[2] = 0.8;
    this->light_amb[0] = this->light_amb[1] = this->light_amb[2] = 0.2;
    this->light_spec[0] = this->light_spec[1] = this->light_spec[2] = 0.5;
    this->light_amb[3] = 1.0;
    this->light_pos[3] = 1.0;
    this->light_dif[3] = 1.0;
    this->light_spec[3] = 1.0;
    this->light_spot_dir[3] = 1.0;
    this->light_spot_exp = 0.0;
    this->light_spot_cutoff = 180.0;
    this->light_atten_c = 1.0;
    this->light_atten_l = 0.0;
    this->light_atten_q = 0.0;
}

void n1_disp::light_apply () {
    glEnable (GL_LIGHT0);
    glLightfv (GL_LIGHT0, GL_POSITION, this->light_pos);
    glLightfv (GL_LIGHT0, GL_AMBIENT, this->light_amb);
    glLightfv (GL_LIGHT0, GL_DIFFUSE, this->light_dif); 
    glLightfv (GL_LIGHT0, GL_SPECULAR, this->light_spec);
    glLightfv (GL_LIGHT0, GL_SPOT_DIRECTION, this->light_spot_dir);
    glLightfv (GL_LIGHT0, GL_SPOT_EXPONENT, &this->light_spot_exp);
    glLightfv (GL_LIGHT0, GL_SPOT_CUTOFF, &this->light_spot_cutoff);
    glLightfv (GL_LIGHT0, GL_CONSTANT_ATTENUATION, &this->light_atten_c);
    glLightfv (GL_LIGHT0, GL_LINEAR_ATTENUATION, &this->light_atten_l);
    glLightfv (GL_LIGHT0, GL_QUADRATIC_ATTENUATION, &this->light_atten_q);
}

void n1_disp::light_apply_pos () {
    glEnable (GL_LIGHT0);
    glLightfv (GL_LIGHT0, GL_POSITION, this->light_pos);
}

void n1_disp::tex_bind (uint32_t tid) {
    glBindTexture (GL_TEXTURE_2D, tid);
}

void n1_disp::tex_render (uint32_t id, int32_t x, int32_t y,
        int32_t w, int32_t h) {
    float fdt, fdl, fdb, fdr;

    fdl = (float)x;
    fdr = (float)x + (float)w;
    fdb = (float)y + (float)h;
    fdt = (float)y;

    if (id)
        glBindTexture (GL_TEXTURE_2D, id);

    glBegin (GL_QUADS);

    glTexCoord2f (0.0, 1.0);
    glVertex2f (fdl, fdb);
    glTexCoord2f (1.0, 1.0);
    glVertex2f (fdr, fdb);
    glTexCoord2f (1.0, 0.0);
    glVertex2f (fdr, fdt);
    glTexCoord2f (0.0, 0.0);
    glVertex2f (fdl, fdt);

    glEnd ();
}
 
n1_disp::tex *n1_disp::tex_load (const char *file_s) {
    uint8_t rgba_white [] = {255, 255, 255, 255};
    tex *_tex;
    char *m;
    img *_img = NULL;

    _tex = tex_get (file_s);
    if (_tex) {
        _tex->ref_n ++;
        return _tex;
    }

    if (strcmp (file_s, "M3E_WHITE")) {
        for (m = (char *)file_s + strlen (file_s); m > (char *)file_s;
                m --) {
            if (m[0] == '.')
                break;
        }

        if ( (m[0] != '.') || (! m[1]) ) {
            dlog::error ("bad tex name", file_s);
            return NULL;
        }

        // texshader load falls through
        if (! strncmp (m, ".mms", 4)) {
            dlog::debug ("falling through", file_s);
            return NULL;
        }

        if (! strncmp (m, ".png", 4)) {
            _img = new img ();
            if (! _img)
                return NULL;
            if (! _img->load_png (file_s))
                return NULL;
        }
        else if (! strncmp (m, ".rwh", 4)) {
            _img = new img ();
            if (! _img)
                return NULL;
            if (! _img->load_rwh (file_s))
                return NULL;
        }
        else {
            dlog::error ("unknown texture type", file_s);
            return NULL;
        }
    }

    _tex = (tex *) malloc (sizeof(tex));
    if (! _tex)
        return NULL;

    _tex->ref_n = 1;
    _tex->name_s = (char *) strdup (file_s);

    glGenTextures (1, &_tex->gl_texid);
    glBindTexture (GL_TEXTURE_2D, _tex->gl_texid);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    if (! strcmp (file_s, "M3E_WHITE")) {
        glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA,
            GL_UNSIGNED_BYTE, rgba_white);
    }
    else {
        glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, _img->w, _img->h, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, _img->_b);

        delete _img;
    }

    this->textures.add (_tex);
    return _tex;
}

n1_disp::tex *n1_disp::tex_get (const char *file_s) {
    tex *_tex;

    for (_tex = (n1_disp::tex *) this->textures.iter_init (); _tex;
            _tex = (n1_disp::tex *) this->textures.iterate ())
        if (! strcmp (_tex->name_s, file_s))
            return _tex;

    return NULL;
}

void n1_disp::tex_del (tex *_tex) {
    _tex->ref_n --;
    if (_tex->ref_n) 
        return;
 
    this->textures.del (_tex, 0, n1_disp::tex_destroy);
}

void n1_disp::tex_destroy (void *_v) {
    tex *_tex = (tex *) _v;
    if (! _tex)
        return;

    if (_tex->name_s)
        free (_tex->name_s);

    glDeleteTextures (1, &_tex->gl_texid);
    free (_tex);
}

