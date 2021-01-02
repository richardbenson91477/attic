#include "n1.hpp"

#define PRAND (((float)rand() / (float)RAND_MAX) - 0.5f)

bool n1_ptcl::init (bool t_) {
    if (t_)
        this->ptcls = new blaze ();
    else
        this->destroy_all ();
    return true;
}

void n1_ptcl::tick (float t) {
    ptcl *_ptcl;
    float r;

    _ptcl = (ptcl *) this->ptcls->iter_init ();
    while (_ptcl) {
        if (_ptcl->ttl) {
            _ptcl->tick += t;
            if (_ptcl->tick > _ptcl->ttl) {
                this->ptcls->del (NULL, this->ptcls->iter_c, blaze::hook_free);
                _ptcl = (ptcl *) this->ptcls->iter_get ();
                continue;
            }
        }

        // move
        _ptcl->p[0] += _ptcl->v[0];
        _ptcl->p[1] += _ptcl->v[1];
        _ptcl->p[2] += _ptcl->v[2];

        // fall
        if (_ptcl->weight) {
            _ptcl->fall_t += _ptcl->weight;
            _ptcl->p[1] -= _ptcl->fall_t;
        }

        // scale color
        if (_ptcl->c_scale)
            _ptcl->c[3] = _ptcl->ca * (1.0f - (_ptcl->tick / _ptcl->ttl));

        // lerp scale
        if (_ptcl->s_scale) {
            if (_ptcl->ttl && _ptcl->tick)
                r = _ptcl->tick / _ptcl->ttl;
            else
                r = 0.0f;

            _ptcl->s[0] = _ptcl->s1[0] + ((_ptcl->s2[0] - _ptcl->s1[0]) * r);
            _ptcl->s[1] = _ptcl->s1[1] + ((_ptcl->s2[1] - _ptcl->s1[1]) * r);
            _ptcl->s[2] = _ptcl->s1[2] + ((_ptcl->s2[2] - _ptcl->s1[2]) * r);
        }

        _ptcl = (ptcl *) this->ptcls->iterate ();
    }
}

void n1_ptcl::render (n1_disp *_disp) {
    ptcl *_ptcl;
    bool tex_;

    if (! this->ptcls->item_n)
        return;

    glMatrixMode (GL_MODELVIEW);

    glDepthMask (false);
    _disp->blend (2);

    glDisable (GL_LIGHTING);
    glDisable (GL_TEXTURE_2D);

    tex_ = false;

    for (_ptcl = (ptcl *) this->ptcls->iter_init (); _ptcl; 
            _ptcl = (ptcl *) this->ptcls->iterate ()) {
        // clip center
        if (_disp->clip (_ptcl->p) < 0)
            continue;

        glColor4fv (_ptcl->c);

        glPushMatrix ();

        glTranslatef (_ptcl->p[0], _ptcl->p[1], _ptcl->p[2]);

        // face the eye
        glRotatef (DEGREES(_disp->eye_theta), 0.0f, 1.0f, 0.0f);
        glRotatef (DEGREES(_disp->eye_phi - PI12), 1.0f, 0.0f, 0.0f);

        switch (_ptcl->type) {
            case PTCL_POINT:
                glBegin (GL_POINTS);
                glVertex3f (0.0, 0.0, 0.0);
                glEnd ();
                break;

            case PTCL_TRI:
                glScalef (_ptcl->s[0], _ptcl->s[1], _ptcl->s[2]);

                glBegin (GL_TRIANGLES);
                glVertex2f (0.5f, -0.5f);
                glVertex2f (0.0f, 0.5f);
                glVertex2f (-0.5f, -0.5f);
                glEnd ();
                break;

            case PTCL_QUAD:
                glScalef (_ptcl->s[0], _ptcl->s[1], _ptcl->s[2]);

                glBegin (GL_QUADS);
                glVertex2f (0.5f, -0.5f);
                glVertex2f (0.5f, 0.5f);
                glVertex2f (-0.5f, 0.5f);
                glVertex2f (-0.5f, -0.5f);
                glEnd ();
                break;

            case PTCL_OBJECT:
                if (! tex_) { 
                    glEnable (GL_TEXTURE_2D); 
                    tex_ = true; 
                }

                glScalef (_ptcl->s[0], _ptcl->s[1], _ptcl->s[2]);
                _ptcl->_obj->render (NULL, NULL);
                break;
        }

        glPopMatrix ();
    }

    glEnable (GL_LIGHTING);
    glEnable (GL_TEXTURE_2D);
    
    _disp->blend (false);
    glDepthMask (true);

    glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
}

bool n1_ptcl::add (uint32_t n, uint32_t type, 
        float ttl, float ttl_range, 
        float weight,
        const float *p, const float *p_range, 
        const float *s, const float *s_range, const float *s2, 
        const float *v, const float *v_range,
        const float *c, int32_t c_scale,
        n1_obj *_obj) {
    float v2 [3];
    ptcl *_ptcl;
    uint32_t c2;

    for (c2 = 0; c2 < n; c2 ++) {
        _ptcl = (ptcl *) malloc (sizeof(ptcl));
        if (! _ptcl)
            return false;

        _ptcl->type = type;
        _ptcl->_obj = _obj;

        _ptcl->ttl = ttl + PRAND * ttl_range;
        _ptcl->tick = 0.0f;
        _ptcl->fall_t = 0.0f;

        _ptcl->weight = weight;

        if (p) {
            if (p_range) {               
                _ptcl->p[0] = p[0] + PRAND * p_range[0];               
                _ptcl->p[1] = p[1] + PRAND * p_range[1];
                _ptcl->p[2] = p[2] + PRAND * p_range[2];
            }
            else
                math::v_copy (_ptcl->p, p);
        }
        else
            math::v_clear (_ptcl->p);

        if (s) {
            if (s_range) {
                _ptcl->s[0] = s[0] + PRAND * s_range[0];
                _ptcl->s[1] = s[1] + PRAND * s_range[1];
                _ptcl->s[2] = s[2] + PRAND * s_range[2];
                _ptcl->s_scale = 0;
            }
            else if (s2) {
                math::v_copy (_ptcl->s, s);
                math::v_copy (_ptcl->s1, s);
                math::v_copy (_ptcl->s2, s2);
                _ptcl->s_scale = 1;
            }
            else {
                math::v_copy (_ptcl->s, s);
                _ptcl->s_scale = 0;
            }
        }
        else
            math::v_setf (_ptcl->s, 1.0f);

        if (v) {
            if (v_range) {
                math::spherical_from_v (v, v2, v2 + 1, v2 + 2);
                v2[0] += PRAND * v_range[0];
                v2[1] += PRAND * v_range[1];
                v2[2] += PRAND * v_range[2];
                math::v_from_spherical (_ptcl->v, v2[0], v2[1], v2[2]);
            }
            else
                math::v_copy (_ptcl->v, v);
        }
        else
            math::v_clear (_ptcl->v);

        if (c) {
            _ptcl->c[0] = c[0]; _ptcl->c[1] = c[1]; _ptcl->c[2] = c[2];
            _ptcl->c[3] = _ptcl->ca = c[3];
            _ptcl->c_scale = c_scale;
        } 
        else
            math::v_setf (_ptcl->c, 1.0f);

        this->ptcls->add (_ptcl);
    }

    return true;
}

void n1_ptcl::destroy_all () {
    this->ptcls->destroy (blaze::hook_free);
}

