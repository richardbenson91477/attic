#include "n1.hpp"

bool n1_texshader::init (bool t_) {
    if (t_) {
        dlog::debug ("n1_texshader::init (true)");
    }
    else {
        dlog::debug ("n1_texshader::init (false)");
        this->destroy_all ();
    }
    return true;
}

void n1_texshader::tick (float t) {
    texshader *_tsh;
    float frame_t;
    uint32_t key_prev, key_next;
    uint32_t frame_c = 0;
    uint32_t c, iter;
    
    for (_tsh = (texshader *) this->texshaders.iter_init (true, &iter); _tsh;
            _tsh = (texshader *) this->texshaders.iterate ()) {
        if (_tsh->paused)
            continue;

        if (_tsh->frame_n) {
            _tsh->a_t += t;
            while (_tsh->a_t >= _tsh->a_n)
                _tsh->a_t -= _tsh->a_n;

            frame_c = (uint32_t)(_tsh->a_t * _tsh->fps);
        }

        // pos keys
        if (! _tsh->p_key_n) {
            math::v_setf (_tsh->pos, 0.0f);  
        }
        else if (frame_c >= _tsh->p_keys[_tsh->p_key_n - 1].frame) {
            math::v_copy (_tsh->pos,
                _tsh->p_keys[_tsh->p_key_n - 1].pos);
        }
        else if (frame_c < _tsh->p_keys[0].frame) {
            math::v_copy (_tsh->pos, _tsh->p_keys[0].pos);
        }
        else {
            key_prev = key_next = 0;
            for (c = 0; c < _tsh->p_key_n; c ++) {
                if (_tsh->p_keys[c].frame > frame_c) {
                    key_next = c;
                    break;
                }

                key_prev = c;
            }

            frame_t = ((_tsh->a_t * _tsh->fps) -
                _tsh->p_keys[key_prev].frame) /
                (_tsh->p_keys[key_next].frame -
                    _tsh->p_keys[key_prev].frame);
 
            math::v_sub2 (_tsh->pos, _tsh->p_keys[key_next].pos,
                _tsh->p_keys[key_prev].pos);
            math::v_scalef (_tsh->pos, frame_t);
            math::v_add (_tsh->pos, _tsh->p_keys[key_prev].pos);
        }

        // scale keys
        if (! _tsh->s_key_n) {
            _tsh->scale[0] = 1.0f;
            _tsh->scale[1] = 1.0f;
        }
        else if (frame_c >= _tsh->s_keys[_tsh->s_key_n - 1].frame) {
            _tsh->scale[0] = _tsh->s_keys[_tsh->s_key_n - 1].scale[0];
            _tsh->scale[1] = _tsh->s_keys[_tsh->s_key_n - 1].scale[1];
        }
        else if (frame_c < _tsh->s_keys[0].frame) {
            _tsh->scale[0] = _tsh->s_keys[0].scale[0];
            _tsh->scale[1] = _tsh->s_keys[0].scale[1];
        }
        else {
            key_prev = key_next = 0;
            for (c = 0; c < _tsh->s_key_n; c ++) {
                if (_tsh->s_keys[c].frame > frame_c) {
                    key_next = c;
                    break;
                }
    
                key_prev = c;
            }

            frame_t = ((_tsh->a_t * _tsh->fps) -
                _tsh->s_keys[key_prev].frame) /
                (_tsh->s_keys[key_next].frame -
                    _tsh->s_keys[key_prev].frame);
 
            _tsh->scale[0] = _tsh->s_keys[key_next].scale[0] -
                _tsh->s_keys[key_prev].scale[0];
            _tsh->scale[0] *= frame_t;
            _tsh->scale[0] += _tsh->s_keys[key_prev].scale[0];

            _tsh->scale[1] = _tsh->s_keys[key_next].scale[1] -
                _tsh->s_keys[key_prev].scale[1];
            _tsh->scale[1] *= frame_t;
            _tsh->scale[1] += _tsh->s_keys[key_prev].scale[1];
        }

        // rot keys
        if (! _tsh->r_key_n) {
            _tsh->rot = 0.0f;
            _tsh->rpos[0] = 0.5f;
            _tsh->rpos[1] = 0.5f;
        }
        else if (frame_c >= _tsh->r_keys[_tsh->r_key_n - 1].frame) {
            _tsh->rot = _tsh->r_keys[_tsh->r_key_n - 1].rot;
            _tsh->rpos[0] = _tsh->r_keys[_tsh->r_key_n - 1].rpos[0];
            _tsh->rpos[1] = _tsh->r_keys[_tsh->r_key_n - 1].rpos[1];
        }
        else if (frame_c < _tsh->r_keys[0].frame) {
            _tsh->rot = _tsh->r_keys[0].rot;
            _tsh->rpos[0] = _tsh->r_keys[0].rpos[0];
            _tsh->rpos[1] = _tsh->r_keys[0].rpos[1];
        }
        else {
            key_prev = key_next = 0;
            for (c = 0; c < _tsh->r_key_n; c ++) {
                if (_tsh->r_keys[c].frame > frame_c) {
                    key_next = c;
                    break;
                }

                key_prev = c;
            }

            frame_t = ((_tsh->a_t * _tsh->fps) -
                _tsh->r_keys[key_prev].frame) /
                (_tsh->r_keys[key_next].frame -
                    _tsh->r_keys[key_prev].frame);
 
            _tsh->rot = _tsh->r_keys[key_next].rot -
                _tsh->r_keys[key_prev].rot;
            _tsh->rot *= frame_t;
            _tsh->rot += _tsh->r_keys[key_prev].rot;

            _tsh->rpos[0] = _tsh->r_keys[key_next].rpos[0] -
                _tsh->r_keys[key_prev].rpos[0];
            _tsh->rpos[0] *= frame_t;
            _tsh->rpos[0] += _tsh->r_keys[key_prev].rpos[0];

            _tsh->rpos[1] = _tsh->r_keys[key_next].rpos[1] -
                _tsh->r_keys[key_prev].rpos[1];
            _tsh->rpos[1] *= frame_t;
            _tsh->rpos[1] += _tsh->r_keys[key_prev].rpos[1];
        }

        // tex keys
        if (! _tsh->t_key_n) {
            _tsh->tex_id = _tsh->tex_def;
        }
        else if (frame_c >= _tsh->t_keys[_tsh->t_key_n - 1].frame) {
            _tsh->tex_id = _tsh->t_keys[_tsh->t_key_n - 1].tex_id;
        }
        else if (frame_c < _tsh->t_keys[0].frame) {
            _tsh->tex_id = _tsh->t_keys[0].tex_id;
        }
        else {
            key_prev = 0;
            for (c = 0; c < _tsh->t_key_n; c ++) {
                if (_tsh->t_keys[c].frame > frame_c)
                    break;
                key_prev = c;
            }
    
            _tsh->tex_id = _tsh->t_keys[key_prev].tex_id;
        }
    }
    this->texshaders.iter_init (false, &iter);
}

void n1_texshader::apply (texshader *_tsh) {
    glMatrixMode (GL_TEXTURE);
    glPushMatrix ();

    glBindTexture (GL_TEXTURE_2D, _tsh->tex_id);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glScalef (_tsh->scale[0], _tsh->scale[1], 1.0f);
    glTranslatef (_tsh->rpos[0], _tsh->rpos[1], 0.0f);
    glRotatef (-_tsh->rot, 0.0f, 0.0f, 1.0f);
    glTranslatef (-_tsh->rpos[0], -_tsh->rpos[1], 0.0f);
    glTranslatef (-_tsh->pos[0], -_tsh->pos[1], -_tsh->pos[2]);

    if (_tsh->blend_type) {
        _app->disp.blend (_tsh->blend_type);
        glDepthMask (false);
    }
}

void n1_texshader::unapply (texshader *_tsh) {
    glPopMatrix ();
    if (_tsh->blend_type) {
        glDisable (GL_BLEND);
        glDepthMask (true);             
    }
    glMatrixMode (GL_MODELVIEW);
}

n1_texshader::texshader *n1_texshader::load (const char *file_s) {
    char b_in [PATH_MAX];
    char kw [160];
    char ops [6][160];
    FILE *f_in;
    texshader *_tsh;
    n1_disp::tex *_tex;
    uint32_t frame_c = 0;
 
    dlog::debug ("n1_texshader::load", file_s);

    _tsh = this->get (file_s);
    if (_tsh) {
        _tsh->ref_c ++;
        return _tsh;
    }

    f_in = fopen (file_s, "r");
    if (! f_in)
        return NULL;

    _tsh = (texshader *) malloc (sizeof(texshader));
    if (! _tsh)
        return NULL;
    memset (_tsh, 0, sizeof(texshader));

    _tsh->ref_c = 1;
    _tsh->name_s = (char *) strdup (file_s);

    while (fgets (b_in, sizeof(b_in), f_in)) {
        if (b_in [strlen (b_in) - 1] == '\n')
            b_in [strlen (b_in) - 1] = 0;
        if (! b_in [0])
            continue;
        else if (b_in [0] == ';')
            continue;
        
        n1_var::var_start (b_in);
        if (! n1_var::var_next (kw))
            continue;

        if (! strcmp (kw, "frames")) {
            if (! n1_var::var_next (ops[0]))
                continue;
            
            _tsh->frame_n = atoi (ops[0]);
            _tsh->r_keys = (r_key *) malloc (_tsh->frame_n *
                sizeof(r_key));
            _tsh->p_keys = (p_key *) malloc (_tsh->frame_n *
                sizeof(p_key));
            _tsh->s_keys = (s_key *) malloc (_tsh->frame_n *
                sizeof(s_key));
            _tsh->t_keys = (t_key *) malloc (_tsh->frame_n *
                sizeof(t_key));
        }
        else if (! strcmp (kw, "speed")) {
            if (! n1_var::var_next (ops[0]))
                continue;
            
            _tsh->fps = (float) atof (ops[0]);
        }
        else if (! strcmp (kw, "tex")) {
            if (! n1_var::var_next (ops[0]))
                continue;

            // texture must already be loaded and must not be unloaded
            // until texshader is unloaded
            _tex = _app->disp.tex_get (ops[0]);
            if (! _tex) {
                dlog::error ("texshader tex not loaded", ops[0]);
                continue;
            }
            _tsh->tex_def = _tex->gl_texid;
        }
        else if (! strcmp (kw, "blend")) {
            if (! n1_var::var_next (ops[0]))
                continue;
            
            _tsh->blend_type = (uint32_t) atoi (ops[0]);
        }
        else if (! strcmp (kw, "frame")) {
            if (! n1_var::var_next (ops[0]))
                continue;
            
            frame_c = atoi (ops[0]) - 1;
        }
        else if (! strcmp (kw, "pos")) {
            if (! n1_var::var_next (ops[0]))
                continue;
            if (! n1_var::var_next (ops[1]))
                continue;
            if (! n1_var::var_next (ops[2]))
                continue;

            _tsh->p_keys[_tsh->p_key_n].frame = frame_c;
            _tsh->p_keys[_tsh->p_key_n].pos[0] = (float) atof (ops[0]);
            _tsh->p_keys[_tsh->p_key_n].pos[1] = (float) atof (ops[1]);
            _tsh->p_keys[_tsh->p_key_n].pos[2] = (float) atof (ops[2]);

            _tsh->p_key_n ++;
        }
        else if (! strcmp (kw, "rot")) {
            if (! n1_var::var_next (ops[0]))
                continue;

            _tsh->r_keys[_tsh->r_key_n].frame = frame_c;
            _tsh->r_keys[_tsh->r_key_n].rot = (float) atof (ops[0]);

            if (! n1_var::var_next (ops[0]))
                _tsh->r_keys[_tsh->r_key_n].rpos[0] = 0.5f;
            else
                _tsh->r_keys[_tsh->r_key_n].rpos[0] =
                    (float) atof (ops[0]);

            if (! n1_var::var_next (ops[0]))
                _tsh->r_keys[_tsh->r_key_n].rpos[1] = 0.5f;
            else
                _tsh->r_keys[_tsh->r_key_n].rpos[1] =
                    (float) atof (ops[0]);

            _tsh->r_key_n ++;
        }
        else if (! strcmp (kw, "scale")) {
            if (! n1_var::var_next (ops[0]))
                continue;
            if (! n1_var::var_next (ops[1]))
                continue;

            _tsh->s_keys[_tsh->s_key_n].frame = frame_c;
            _tsh->s_keys[_tsh->s_key_n].scale[0] = (float) atof (ops[0]);
            _tsh->s_keys[_tsh->s_key_n].scale[1] = (float) atof (ops[1]);
            _tsh->s_key_n ++;
        }
        else if (! strcmp (kw, "tex")) {
            if (! n1_var::var_next (ops[0]))
                continue;

            // texture must already be loaded somewhere
            // and not released until texshader is released
            _tex = _app->disp.tex_get (ops[0]);
            if (! _tex) {
                dlog::error ("texshader tex not loaded", ops[0]);
                continue;
            }
            
            _tsh->t_keys[_tsh->t_key_n].frame = frame_c;
            _tsh->t_keys[_tsh->t_key_n].tex_id = _tex->gl_texid;
            _tsh->t_key_n++;
        }
        else
            dlog::error ("unknown texshader keyword", kw);
    }

    fclose (f_in);

    if (_tsh->frame_n)
        _tsh->a_n = (_tsh->frame_n - 1) / _tsh->fps;

    _tsh->scale[0] = 1.0f;
    _tsh->scale[1] = 1.0f;

    _tsh->paused = false;

    this->texshaders.add (_tsh);
    return _tsh;
}

n1_texshader::texshader *n1_texshader::get (const char *file_s) {
    texshader *_tsh;
    for (_tsh = (n1_texshader::texshader *) this->texshaders.iter_init (); _tsh;
            _tsh = (n1_texshader::texshader *) this->texshaders.iterate ())
        if (! strcmp (_tsh->name_s, file_s)) {
            return _tsh;
        }

    return NULL;
}

void n1_texshader::del (n1_texshader::texshader *_tsh) {
    _tsh->ref_c --;
    if (_tsh->ref_c)
        return;

    this->texshaders.del (_tsh, 0, n1_texshader::texshader_destroy);
}

void n1_texshader::destroy_all () {
    this->texshaders.destroy (n1_texshader::texshader_destroy);
};

void n1_texshader::texshader_destroy (void *_v) {
    if (_v) {
        texshader *_tsh = (texshader *) _v;
        if (_tsh->name_s) 
            free (_tsh->name_s);
        if (_tsh->r_keys)
            free (_tsh->r_keys);
        if (_tsh->p_keys)
            free (_tsh->p_keys);
        if (_tsh->s_keys)
            free (_tsh->s_keys);
        if (_tsh->t_keys)
            free (_tsh->t_keys);
        free (_tsh);
    }
}

