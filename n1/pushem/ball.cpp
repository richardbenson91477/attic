#include "pushem.hpp"

bool ball_init (bool which) {
    static bool first_ = true;

    if (which) {
        if (first_) {
            s._obj_ball = new n1_obj (s._app);
            if (! s._obj_ball->load ((char *) "ball.mmo"))
                return false;

            s._snd_bounce = new snd ("bounce.raw");

            s._app->phys.set_callback (2, ball_col_callback);

            first_ = false;
        }

        s.ball_start_t = 0.0f;
        s.ball_started_n = 0;
    }
    else {
        s.balls.destroy (ball_destroy);
        delete (s._snd_bounce);
    }
    return true;
}

ball *ball_add (float *pos) {
    ball *_ball;
    n1_phys::ent *_ent;

    _ball = (ball *) malloc (sizeof(ball));
    if (! _ball)
        return NULL;

    memset (_ball, 0, sizeof(ball));

    _ball->_ent = _ent = s._app->phys.ent_add ();
    _ent->type = s.TYPE_BALL;
    _ent->weight = s.conf.ball_weight;
    _ent->parent = (void *) _ball;
    _ent->_obj = s._obj_ball;
    _ent->marlo = true;
    _ent->rotate_ = true;
    _ent->bound_only = true;
    math::v_copy (_ent->pos, pos);
    _ent->cell = s._app->cell.classify (_ent->pos);
    _ball->_snd_bounce = s._snd_bounce;

    if (! s.balls.add (_ball))
        return NULL;

    return _ball;
}

void ball_tick (float t) {
    float m_v [3][4], m_orig [3][4], v [3];
    ball *_ball;
    n1_phys::ent *_ent;
    uint32_t iter;

    if (s.ball_started_n != s.conf.balls_start_n) {
        s.ball_start_t += t;
        if (s.ball_start_t >= s.conf.ball_start_t) {
            s.ball_start_t = 0.0;
            v[0] = (float) s.ball_started_n * 2.0f;
            v[1] = 0.0f;
            v[2] = -2.0f;
            ball_add (v);
            s.ball_started_n ++;
        }
    }
    
    for (_ball = (ball *) s.balls.iter_init (true, &iter); _ball;
            _ball = (ball *) s.balls.iterate ()) {
        _ent = _ball->_ent;
        if (! _ent)
            continue;

        // something went wrong
        if (_ent->cell == 0) {
            math::v_clear (_ent->pos);
            math::v_clear (_ent->mov_v);
            _ent->mov_rho = 0.0f;
            continue;
        }

        // ptcl tick
        if (_ent->mov_rho > s.conf.ball_ptcl_at_r) {
            _ball->ptcl_t += _ent->mov_rho;
            if (_ball->ptcl_t > s.conf.ball_ptcl_freq) {
                s._app->ptcl.add (1, n1_ptcl::PTCL_OBJECT, s.ptcl_l, 0.0f,
                    s.ptcl_w, _ent->pos, NULL, s.ptcl_s, NULL, s.ptcl_s2,
                    s.ptcl_v, NULL, s.ptcl_c, true, s._obj_trail);

                _ball->ptcl_t -= s.conf.ball_ptcl_freq;
            }
        }

        // rotation
        v[2] = -_ent->mov_v[0];
        v[1] = 0.0f;
        v[0] = _ent->mov_v[2];
        math::v_scalef (v, _ent->mov_rho * t);
        math::m_from_r (m_v, v);
        math::m_copy (m_orig, _ent->rmat);
        math::m_concat (_ent->rmat, m_v, m_orig);

        // phys
        if (_ent->mov_rho) {
            if ((! _ent->jumping) && (! _ent->falling)) {
                _ent->mov_rho *= s.conf.ent_land_friction;
            }
            if (_ent->mov_rho < s.conf.rho_cutoff)
                _ent->mov_rho = 0.0f;
        }
    }
    s.balls.iter_init (false, &iter);
}

void ball_destroy (void *_v) {
    ball *_ball = (ball *) _v;
    if (! _ball)
        return;
    s._app->phys.ents.del (_ball->_ent);
    free (_v);
}

void ball_col_callback (const n1_phys::ecol *_col) {
    ball *_ball;

    entecol_callback (_col);

    _ball = (ball *) _col->_ent->parent;
    _ball->_snd_bounce->play (false);
    _ball->_snd_bounce->pos3d (_ball->_ent->pos);
}

