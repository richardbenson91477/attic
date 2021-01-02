#include "n1.hpp"

void n1_obj::render (const float *cam, const float (*mat)[4]) {
    float v [3], n [3];
    poly *_poly;
    otex *_otex, *_otex2 = NULL;
    uint32_t tex_c, c;
    bool tex2_ = false;

    glBindTexture (GL_TEXTURE_2D, _app->disp.tex_white);

    if (this->tex2) {
        glActiveTextureARB (GL_TEXTURE1_ARB);
        glDisable (GL_TEXTURE_2D);

        _otex2 = this->_otexs + (this->tex2 - 1);
        if (_otex2->_tex) {
            glBindTexture (GL_TEXTURE_2D, _otex2->_tex->gl_texid);
        }
        else if (_otex2->psh) {
            _app->texshader.apply (_otex2->psh);
            glMatrixMode (GL_MODELVIEW);
        }

        glActiveTextureARB (GL_TEXTURE0_ARB);
    }

    _otex = this->_otexs;
    for (tex_c = 0; tex_c <= this->otex_n; tex_c ++) {
        if (tex_c) {
            if (_otex->_tex)
                glBindTexture (GL_TEXTURE_2D, _otex->_tex->gl_texid);

            if (_otex->psh)
                _app->texshader.apply (_otex->psh);
        }

        glBegin (GL_TRIANGLES);
        
        _poly = this->_polys;
        for (c = 0; c < this->poly_n; c ++) {
            if (_poly->tex != tex_c) {
                _poly ++;
                continue;
            }

            // backfacing test
            if (cam) {
                if (! mat) {
                    math::v_sub2 (v, _poly->_v1, cam);
                    if (math::v_dot (v, _poly->n) >= 0.0f) {
                        _poly ++;
                        continue;
                    }
                }
                else {
                    math::v_rotate_m2 (v, _poly->_v1, mat);
                    math::v_sub (v, cam);
                    math::v_rotate_m2 (n, _poly->n, mat);
                    if (math::v_dot (v, n) >= 0.0f) {
                        _poly ++;
                        continue;
                    }
                }
            }

            if (this->tex2) {
                if ((_poly->tex2) && (! tex2_)) {
                    glEnd ();
                    glActiveTextureARB (GL_TEXTURE1_ARB);
                    glEnable (GL_TEXTURE_2D);
                    glActiveTextureARB (GL_TEXTURE0_ARB);
                    glBegin (GL_TRIANGLES);

                    tex2_ = true;
                }
                else if ((! _poly->tex2) && (tex2_)) {
                    glEnd ();
                    glActiveTextureARB (GL_TEXTURE1_ARB);
                    glDisable (GL_TEXTURE_2D);
                    glActiveTextureARB (GL_TEXTURE0_ARB);
                    glBegin (GL_TRIANGLES);

                    tex2_ = false;
                }
            }
            
            if (! mat) {
                glTexCoord2fv (_poly->tex_pos + 4);
                glNormal3fv (_poly->pn3);
                glVertex3fv (_poly->_v3);

                glTexCoord2fv (_poly->tex_pos + 2);
                glNormal3fv (_poly->pn2);
                glVertex3fv (_poly->_v2);

                glTexCoord2fv (_poly->tex_pos);
                glNormal3fv (_poly->pn1);
                glVertex3fv (_poly->_v1);
            }
            else {
                glTexCoord2fv (_poly->tex_pos + 4);
                math::v_rotate_m2 (n, _poly->pn3, mat);
                glNormal3fv (n);
                math::v_rotate_m2 (v, _poly->_v3, mat);
                glVertex3fv (v);

                glTexCoord2fv (_poly->tex_pos + 2);
                math::v_rotate_m2 (n, _poly->pn2, mat);
                glNormal3fv (n);
                math::v_rotate_m2 (v, _poly->_v2, mat);
                glVertex3fv (v);

                glTexCoord2fv (_poly->tex_pos);
                math::v_rotate_m2 (n, _poly->pn1, mat);
                glNormal3fv (n);
                math::v_rotate_m2 (v, _poly->_v1, mat);
                glVertex3fv (v);
            }
            _poly ++;
        }
        glEnd ();

        if (tex_c) {
            if (_otex->psh) {
                _app->texshader.unapply (_otex->psh);
            }
            _otex ++;
        }
    }

    if (this->tex2) {
        glActiveTextureARB (GL_TEXTURE1_ARB);

        if (_otex2->psh) {
            glMatrixMode (GL_TEXTURE);
            _app->texshader.unapply (_otex2->psh);
        }

        glDisable (GL_TEXTURE_2D);
        glActiveTextureARB (GL_TEXTURE0_ARB);
    }
}

void n1_obj::clear () {
    this->file_s = NULL;
    this->_verts = NULL;
    this->_polys = NULL;
    this->_otexs = NULL;
    this->vert_n = 0;
    this->poly_n = 0;
    this->otex_n = 0;
    this->seq_n = 0;
    this->joint_n = 0;
    this->anim_ = false;
    this->brad = 0.0f;
    this->pre_sbrad = 0.0f;
}

bool n1_obj::load (const char *file_s) {
    char buf [PATH_MAX];
    float pos [3], rot [3], fps;
    FILE *f_in = NULL;
    poly *_poly;
    vert *_vert;
    otex *_otex;
    joint *_joint;
    uint32_t vert_ids [3];
    uint32_t vert_n, poly_n, joint_n, key_n, otex_n, seq_n, frame_n, name_len;
    uint32_t parent_id, seq_id, frame_id;
    uint32_t c, c2, type;
    char *m;

    dlog::debug ("n1_obj::load", file_s);
   
    this->clear ();

    f_in = fopen (file_s, "rb");
    if (! f_in)
        return false;

    strcpy (buf, file_s);
    m = strrchr (buf, '/');
    if (m)
        m[0] = 0;

    this->file_s = strdup (file_s);

    // header
    fread (buf, sizeof(char), 5, f_in);
    if (! strncmp (buf, "MMOF", 4)) {
        type = 7;
    }
    else if (! strncmp (buf, "MMOG", 4)) {
        type = 8;
    }
    else {
        dlog::error ("invalid obj file", this->file_s);
        return false;
    }

    if (type > 7) {
        //fread (&this->btype, 4, 1, f_in);
        fseek (f_in, 4, SEEK_CUR);
        fread (&this->brad, sizeof(float), 1, f_in);
        //fread (&this->b, sizeof(float), 3, f_in);
        fseek (f_in, sizeof(float) * 3, SEEK_CUR);
    }

    fread (&otex_n, 4, 1, f_in);
    if (otex_n) {
        this->_otexs = (otex *) malloc (otex_n * sizeof(otex));
        if (! this->_otexs) {
            dlog::error ("malloc");
            return false;
        }

        _otex = this->_otexs;
        for (c = 0; c < otex_n; c ++) {
            fread (&name_len, 4, 1, f_in);
            fread (buf, 1, name_len, f_in);
            buf[name_len] = 0;
            
            if (! otex_add (_otex, buf)) {
                dlog::error ("tex not found", buf);
                
                if (! otex_add (_otex, (char *)"M3E_WHITE")) {
                    dlog::error ("M3E_WHITE not found");
                    return false;
                }
            }
    
            _otex ++;
        }
    } // if otex_n

    fread (&this->tex2, 4, 1, f_in);
    fread (&vert_n, 4, 1, f_in);

    if (type < 8) {
        //fread (&this->btype, 4, 1, f_in);
        fseek (f_in, 4, SEEK_CUR);
        fread (&this->brad, sizeof(float), 1, f_in);
        //fread (&this->b, sizeof(float), 3, f_in);
        fseek (f_in, sizeof(float) * 3, SEEK_CUR);
    }

    if (vert_n) {
        this->_verts = (vert *) malloc (vert_n * sizeof(vert));
        if (! this->_verts) {
            dlog::error ("malloc(2)");
            return false;
        }

        for (c = 0; c < vert_n; c ++) {
            fread (pos, sizeof(float), 3, f_in);
 
            if (! (_vert = vert_add (this->_verts + c, pos)))
                return false;

            fread (&_vert->joint_id, 4, 1, f_in);
        }
    } // if vert_n

    fread (&poly_n, 4, 1, f_in);
    if (poly_n) {
        this->_polys = (poly *) malloc (poly_n * sizeof(poly));
        if (! this->_polys) {
            dlog::error ("malloc(3)");
            return false;
        }

        for (c = 0; c < poly_n; c ++) {
            fread (vert_ids, 4, 3, f_in);

            _poly = poly_add (this->_polys + c, vert_ids[0], vert_ids[1],
                vert_ids[2]);
            if (! _poly)
                return false;
    
            fread (&_poly->tex, 4, 1, f_in);
            fread (_poly->tex_pos, sizeof(float), 6, f_in);
            // these were upside-down :)
            _poly->tex_pos[1] = 1.0f - _poly->tex_pos[1];
            _poly->tex_pos[3] = 1.0f - _poly->tex_pos[3];
            _poly->tex_pos[5] = 1.0f - _poly->tex_pos[5];
            fread (&_poly->tex2, 4, 1, f_in);
        }
    } // if poly_n

    fread (&seq_n, 4, 1, f_in);
    if (seq_n) {
        for (c = 0; c < seq_n; c ++) {
            fread (&name_len, 4, 1, f_in);
            if (name_len) {
                fread (buf, 1, name_len, f_in);
                buf[name_len] = 0;
            }
            else
                buf[0] = 0;
                
            fread (&frame_n, 4, 1, f_in);
            fread (&fps, sizeof(float), 1, f_in);

            seq_add (buf, frame_n, fps);
        }
    }   // if seq_n
    
    fread (&joint_n, 4, 1, f_in);
    if (joint_n) {
        for (c = 0; c < joint_n; c ++) {
            fread (pos, sizeof(float), 3, f_in);
            fread (rot, sizeof(float), 3, f_in);
            rot[0] = RADIANS(rot[0]);
            rot[1] = RADIANS(rot[1]);
            rot[2] = RADIANS(rot[2]);

            fread (&parent_id, 4, 1, f_in);
    
            _joint = joint_add (pos, rot, parent_id);
            if (! _joint)
                return false;
    
            fread (&key_n, 4, 1, f_in);
            for (c2 = 0; c2 < key_n; c2 ++) {
                fread (rot, sizeof(float), 3, f_in);
                rot[0] = RADIANS(rot[0]);
                rot[1] = RADIANS(rot[1]);
                rot[2] = RADIANS(rot[2]);
                fread (&seq_id, 4, 1, f_in);
                fread (&frame_id, 4, 1, f_in);
    
                if (! rkey_add (_joint, seq_id, frame_id, rot))
                    return false;
            }
    
            fread (&key_n, 4, 1, f_in);
            for (c2 = 0; c2 < key_n; c2 ++) {
                fread (pos, sizeof(float), 3, f_in);
                fread (&seq_id, 4, 1, f_in);
                fread (&frame_id, 4, 1, f_in);

                if (! pkey_add (_joint, seq_id, frame_id, pos))
                    return false;
            }
        }
    } // if joint_n
    
    fclose (f_in);

    this->pre_sbrad = this->brad * this->brad;
 
    this->update ();
    this->normal_update_all ();
    this->joint_update_all ();

    return true;
}

void n1_obj::destroy () {
    otex *_otex;
    uint32_t c;
    
    dlog::debug ("n1_obj::destroy", this->file_s);

    if (this->_verts) {
        free (this->_verts);
        this->_verts = NULL;
    }
    this->vert_n = 0;

    if (this->_polys) {
        free (this->_polys);
        this->_polys = NULL;
    }
    this->poly_n = 0;

    _otex = this->_otexs;
    if (_otex) {
        for (c = 0; c < this->otex_n; c++) {
            if (_otex->_tex)
                _app->disp.tex_del (_otex->_tex);
            else if (_otex->psh)
                _app->texshader.del (_otex->psh);
            _otex ++;
        }
        free (this->_otexs);
        this->_otexs = NULL;
    }
    this->otex_n = 0;

    this->joints.destroy (n1_obj::joint::destroy);
    this->seqs.destroy (n1_obj::seq_destroy);

    if (this->file_s) {
        free (this->file_s);
        this->file_s = NULL;
    }
}

n1_obj::vert *n1_obj::vert_add (vert *_vert, float *pos) {
    _vert->n[0] = 0.0f; _vert->n[1] = 0.0f; _vert->n[2] = 0.0f;
    _vert->joint_id = 0;
    _vert->pos[0] = pos[0];
    _vert->pos[1] = pos[1];
    _vert->pos[2] = pos[2];
    this->vert_n ++;
    return _vert;
}

n1_obj::poly *n1_obj::poly_add (poly *_poly, uint32_t v1,
        uint32_t v2, uint32_t v3) {
    vert *_vert [3];

    _poly->tex = _poly->tex2 = 0;

    // NOTE: tex_pos is untouched

    _poly->vert_ids[0] = v1;
    _poly->vert_ids[1] = v2;
    _poly->vert_ids[2] = v3;
    this->poly_n ++;

    // NOTE: assuming verts valid if any verts exist
    if (! this->vert_n)
        return _poly;

    _vert[0] = this->_verts +_poly->vert_ids[0];
    _vert[1] = this->_verts +_poly->vert_ids[1];
    _vert[2] = this->_verts +_poly->vert_ids[2];

    _poly->_v1 = _vert[0]->pos;
    _poly->_v2 = _vert[1]->pos;
    _poly->_v3 = _vert[2]->pos;
    _poly->pn1 = _vert[0]->n;
    _poly->pn2 = _vert[1]->n;
    _poly->pn3 = _vert[2]->n;
    math::poly_plane (_poly->n, _poly->_v1, _poly->_v2, _poly->_v3);

    return _poly;
}

bool n1_obj::otex_add (n1_obj::otex *_otex, char *file_s) {
    _otex->_tex = _app->disp.tex_load (file_s);
    if (! _otex->_tex) {
        _otex->psh = _app->texshader.load (file_s);
        if (! _otex->psh)
            return false;
    }
    else
        _otex->psh = NULL;

    _otex->i1 = _otex->i2 = 0;
    this->otex_n ++;
    return true;
}

n1_obj::seq *n1_obj::seq_add (char *name_s, uint32_t frame_n, float fps) {
    seq *_seq = (seq *) malloc (sizeof(seq));
    if (! _seq)
        return NULL;

    if (name_s)
        _seq->name_s = (char *) strdup (name_s);
    else
        _seq->name_s = NULL;

    _seq->frame_n = frame_n;
    _seq->fps = fps;

    this->seqs.add (_seq);
    this->seq_n ++;
    return _seq;
}

n1_obj::seq *n1_obj::seq_get_by_name (char *name_s) {
    seq *_seq;
    for (_seq = (seq *) this->seqs.iter_init (); _seq;
            _seq = (seq *) this->seqs.iterate ())
        if ((_seq->name_s) && (! strcmp (_seq->name_s, name_s)))
            return _seq;
    return NULL;
}

void n1_obj::seq_destroy (void *_v) {
    seq *_seq = (seq *) _v;
    if (! _v)
        return;
    if (_seq->name_s)
        free (_seq->name_s);
    free (_seq);
}

n1_obj::joint *n1_obj::joint_add (float *pos, float *rot,
        uint32_t parent_id) {
    joint *_joint = new joint;
    if (! _joint)
        return NULL;

    _joint->parent_id = parent_id;
    if (parent_id)
        _joint->parent = (joint *) this->joints.get (parent_id - 1);
    else
        _joint->parent = NULL;

    _joint->pos[0] = pos[0];
    _joint->pos[1] = pos[1];
    _joint->pos[2] = pos[2];
    _joint->rot[0] = rot[0];
    _joint->rot[1] = rot[1];
    _joint->rot[2] = rot[2];
    _joint->pkey_n = 0;
    _joint->rkey_n = 0;

    this->joints.add (_joint);
    this->joint_n ++;
    return _joint;
}

n1_obj::rkey *n1_obj::rkey_add (joint *_joint, uint32_t seq_id,
        uint32_t frame_id, const float *rot) {
    rkey *_rkey;

    if (! _joint)
        return NULL;

    // update if key exists
    _rkey = this->rkey_get (_joint, seq_id, frame_id);
    if (_rkey) {
        math::v_copy (_rkey->rot, rot);
        return _rkey;
    }

    _rkey = (rkey *) malloc (sizeof(rkey));
    if (! _rkey)
        return NULL;

    _rkey->seq_id = seq_id;
    _rkey->frame_id = frame_id;
    math::v_copy (_rkey->rot, rot);

    _joint->rkeys.add (_rkey);
    _joint->rkey_n ++;
    return _rkey;
}

n1_obj::pkey *n1_obj::pkey_add (joint *_joint, uint32_t seq_id,
        uint32_t frame_id, const float *pos) {
    pkey *_pkey;

    if (! _joint)
        return NULL;

    // update if key exists
    _pkey = this->pkey_get (_joint, seq_id, frame_id);
    if (_pkey) {
        math::v_copy (_pkey->pos, pos);
        return _pkey;
    }

    _pkey = (pkey *) malloc (sizeof(pkey));
    if (! _pkey)
        return NULL;

    _pkey->seq_id = seq_id;
    _pkey->frame_id = frame_id;
    math::v_copy (_pkey->pos, pos);

    _joint->pkeys.add (_pkey);
    _joint->pkey_n ++;
    return _pkey;
}

n1_obj::rkey *n1_obj::rkey_get (joint *_joint, uint32_t seq_id,
        uint32_t frame_id) {
    rkey *_rkey;

    if (! _joint)
        return NULL;

    for (_rkey = (rkey *) _joint->rkeys.iter_init (); _rkey;
            _rkey = (rkey *) _joint->rkeys.iterate ())
        if ((_rkey->seq_id == seq_id) && (_rkey->frame_id == frame_id))
            return _rkey;
    return NULL;
}

n1_obj::pkey *n1_obj::pkey_get (joint *_joint, uint32_t seq_id,
        uint32_t frame_id) {
    pkey *_pkey;

    if (! _joint)
        return NULL;

    for (_pkey = (pkey *) _joint->pkeys.iter_init (); _pkey;
            _pkey = (pkey *) _joint->pkeys.iterate ())
        if ((_pkey->seq_id == seq_id) && (_pkey->frame_id == frame_id))
            return _pkey;
    return NULL;
}

void n1_obj::update () {
    poly *_poly;
    vert *_vert [3];
    uint32_t c;

    _poly = this->_polys;
    for (c = 0; c < this->poly_n; c++) {
        _vert[0] = this->_verts + _poly->vert_ids[0];
        _vert[1] = this->_verts + _poly->vert_ids[1];
        _vert[2] = this->_verts + _poly->vert_ids[2];

        _poly->_v1 = _vert[0]->pos;
        _poly->_v2 = _vert[1]->pos;
        _poly->_v3 = _vert[2]->pos;
        _poly->pn1 = _vert[0]->n;
        _poly->pn2 = _vert[1]->n;
        _poly->pn3 = _vert[2]->n;

        math::poly_plane (_poly->n, _poly->_v1, _poly->_v2, _poly->_v3);
        _poly++;
    }
}

void n1_obj::normal_update_all () {
    vert *_vert;
    poly *_poly;
    uint32_t c;
    float d;

    _vert = this->_verts;
    for (c = 0; c < this->vert_n; c ++) {
        math::v_clear (_vert->n);
        _vert->tmpi = 0;
        _vert ++;
    }

    _poly = this->_polys;
    for (c = 0; c < this->poly_n; c++) {
        _vert = this->_verts +_poly->vert_ids[0];
        math::v_add (_vert->n, _poly->n);
        _vert->tmpi ++;

        _vert = this->_verts +_poly->vert_ids[1];
        math::v_add (_vert->n, _poly->n);
        _vert->tmpi ++;

        _vert = this->_verts +_poly->vert_ids[2];
        math::v_add (_vert->n, _poly->n);
        _vert->tmpi ++;

        _poly ++;
    }

    _vert = this->_verts;
    for (c = 0; c < this->vert_n; c++) {
        if (_vert->tmpi) {
            math::v_scalef (_vert->n, 1.0f / (float)_vert->tmpi);

            d = _vert->n[0] * _vert->n[0] + _vert->n[1] * _vert->n[1] +
                _vert->n[2] * _vert->n[2];
            if (d >= 0.0f)
                math::v_divf (_vert->n, sqrtf (d));
            else
                math::v_setf (_vert->n, 0.0f);
        }
        _vert ++;
    }   
}

void n1_obj::joint_update_all () {
    static joint *_joint;
 
    // NOTE: joint functions assume parents always precede children in list
    for (_joint = (joint *) this->joints.iter_init (); _joint;
            _joint = (joint *) this->joints.iterate ()) {
        math::m_from_r_p (_joint->m_relative, _joint->rot, _joint->pos);
        if (_joint->parent)
            math::m_concat (_joint->m_final, _joint->parent->m_final,
                _joint->m_relative);
        else
            math::m_copy (_joint->m_final, _joint->m_relative);
    }
}

void n1_obj::anim_init (bool t_, uint32_t seq) {
    float v [3];
    poly *_poly;
    vert *_vert;
    joint *_joint;
    uint32_t c;

    if (t_) {
        // change vertices pos
        _vert = this->_verts;
        for (c = 0; c < this->vert_n; c ++) {
            if (! _vert->joint_id) {
                _vert ++;
                continue;
            }
            _joint = (joint *) this->joints.get (_vert->joint_id - 1);
            math::v_inv_apply_m2 (v, _vert->pos, _joint->m_final);
            math::v_copy (_vert->pos, v);
            math::v_inv_rotate_m2 (v, _vert->n, _joint->m_final);
            math::v_copy (_vert->n, v);
            _vert ++;
        }

        update ();

        _poly = this->_polys;
        for (c = 0; c < poly_n; c ++) {
            _vert = this->_verts + _poly->vert_ids[0];
            if (_vert->joint_id) {
                _poly->_joint1 = (joint *) this->joints.get (_vert->joint_id -
                    1);
                _poly->_v1 = _poly->jv1;
                _poly->pn1 = _poly->jn1;
            }

            _vert = this->_verts + _poly->vert_ids[1];
            if (_vert->joint_id) {
                _poly->_joint2 = (joint *) this->joints.get (_vert->joint_id -
                    1);
                _poly->_v2 = _poly->jv2;
                _poly->pn2 = _poly->jn2;
            }

            _vert = this->_verts + _poly->vert_ids[2];
            if (_vert->joint_id) {
                _poly->_joint3 = (joint *) this->joints.get (_vert->joint_id -
                    1);
                _poly->_v3 = _poly->jv3;
                _poly->pn3 = _poly->jn3;
            }

            _poly ++;
        }

        this->anim_ = true;
        this->anim_set_seq (seq);
    }
    else {
        // restore orig vertices pos
        joint_update_all ();
        
        _vert = this->_verts;
        for (c = 0; c < this->vert_n; c ++) {
            if (! _vert->joint_id) {
                _vert ++;
                continue;
            }
            _joint = (joint *) this->joints.get (_vert->joint_id - 1);
            math::v_apply_m2 (v, _vert->pos, _joint->m_final);
            math::v_copy (_vert->pos, v);
            math::v_rotate_m2 (v, _vert->n, _joint->m_final);
            math::v_copy (_vert->n, v);
            _vert ++;
        }

        this->anim_ = false;
        this->anim_t = 0.0f;
        update ();
    }
}

bool n1_obj::anim () {
    joint *_joint;
    rkey *_rkey, *_rkey_prev;
    pkey *_pkey, *_pkey_prev;
    float m [3][4], pos [3], q [4];
    float frame_t;
    uint32_t frame_c;

    frame_c = (uint32_t)(this->anim_t * this->fps);

    for (_joint = (joint *) joints.iter_init (); _joint;
            _joint = (joint *) joints.iterate ()) {
            
        _pkey_prev = NULL;
        for (_pkey = (pkey *) _joint->pkeys.iter_init (); _pkey;
                _pkey = (pkey *) _joint->pkeys.iterate ()) {
            if (_pkey->seq_id != this->seq_id)
                continue;
            if (_pkey->frame_id > frame_c)
                break;
            _pkey_prev = _pkey;
        }

        if (_pkey && _pkey_prev) {
            frame_t = ((this->anim_t * this->fps) - _pkey_prev->frame_id) /
                (_pkey->frame_id - _pkey_prev->frame_id);

            math::v_sub2 (pos, _pkey->pos, _pkey_prev->pos);
            math::v_scalef (pos, frame_t);
            math::v_add (pos, _pkey_prev->pos);
        }
        else if (_pkey_prev)
            math::v_copy (pos, _pkey_prev->pos);
        else if (_pkey)
            math::v_copy (pos, _pkey->pos);
        else
            math::v_setf (pos, 0.0f);

        _rkey_prev = NULL;
        for (_rkey = (rkey *) _joint->rkeys.iter_init (); _rkey; 
                _rkey = (rkey *) _joint->rkeys.iterate ()) {
            if (_rkey->seq_id != this->seq_id)
                continue;
            if (_rkey->frame_id > frame_c)
                break;
            _rkey_prev = _rkey;
        }

        if (_rkey && _rkey_prev) {   
            frame_t = ((this->anim_t * this->fps) - _rkey_prev->frame_id) /
                (_rkey->frame_id - _rkey_prev->frame_id);

            math::q_slerp (q, _rkey_prev->quat, _rkey->quat, frame_t);
            math::m_from_q (m, q);
        }
        else if (_rkey_prev) {
            math::m_ident (m);
            math::m_from_r (m, _rkey_prev->rot);
        }
        else if (_rkey) {
            math::m_ident (m);
            math::m_from_r (m, _rkey->rot);
        }
        else
            math::m_ident (m);

        m[0][3] = pos[0];
        m[1][3] = pos[1];
        m[2][3] = pos[2];

        math::m_concat (_joint->m_interm, _joint->m_relative, m);

        if (! _joint->parent)
            math::m_copy (_joint->m_final, _joint->m_interm);
        else
            math::m_concat (_joint->m_final, _joint->parent->m_final,
                _joint->m_interm);
    }

    anim_vn (true);
    return true;
}

// calculate animated vertex positions / normals and animated poly normal/D
void n1_obj::anim_vn (bool normals_) {
    poly *_poly;
    vert *_vert;
    float (*pm)[4];
    uint32_t c;
    bool has_joint;
    
    _poly = this->_polys;
    for (c = 0; c < this->poly_n; c ++) {
        has_joint = false;

        _vert = this->_verts + _poly->vert_ids[0];
        if (_vert->joint_id) {
            has_joint = true;

            pm = _poly->_joint1->m_final;
            math::v_apply_m2 (_poly->jv1, _vert->pos, pm);
            math::v_rotate_m2 (_poly->jn1, _vert->n, pm);
        }

        _vert = this->_verts + _poly->vert_ids[1];
        if (_vert->joint_id) {
            has_joint = true;

            pm = _poly->_joint2->m_final;
            math::v_apply_m2 (_poly->jv2, _vert->pos, pm);
            math::v_rotate_m2 (_poly->jn2, _vert->n, pm);
        }

        _vert = this->_verts + _poly->vert_ids[2];
        if (_vert->joint_id) {
            has_joint = true;

            pm = _poly->_joint3->m_final;
            math::v_apply_m2 (_poly->jv3, _vert->pos, pm);
            math::v_rotate_m2 (_poly->jn3, _vert->n, pm);
        }

        if (has_joint && normals_)
            math::poly_plane (_poly->n, _poly->_v1, _poly->_v2, _poly->_v3);

        _poly ++;
    }
}

bool n1_obj::anim_set_seq (uint32_t id) {
    seq *_seq;
    joint *_joint;
    rkey *_rkey;
    uint32_t iter1, iter2;

    if (! (_seq = (seq *) this->seqs.get (id)))
        return false;

    this->seq_id = id;
    this->fps = _seq->fps;
    this->frame_n = _seq->frame_n;
    this->anim_t = 0.0f;
    
    // NOTE: last frame doesn't have length; describes how animation ends
    this->anim_total = (this->frame_n - 1) / this->fps;

    for (_joint = (joint *) this->joints.iter_init (true, &iter1); _joint;
            _joint = (joint *) this->joints.iterate ()) {
        for (_rkey = (rkey *) _joint->rkeys.iter_init (true, &iter2); _rkey;
                _rkey = (rkey *) _joint->rkeys.iterate ())
            if (_rkey->seq_id == id)
                math::q_from_r (_rkey->quat, _rkey->rot);
        _joint->rkeys.iter_init (false, &iter2);
    }
    this->joints.iter_init (false, &iter1);
    return true;
}

