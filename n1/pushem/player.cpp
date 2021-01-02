#include "pushem.hpp"

bool player_init () {
    static bool first_ = true;
    n1_phys::ent *_ent;

    if (first_) {
        first_ = false;

        s._player_obj = new n1_obj (s._app);
        if (! s._player_obj->load ("player.mmo"))
            return false;

        s._player_obj2 = new n1_obj (s._app);
        if (! s._player_obj2->load ("player2.mmo"))
            return false;

        s._player_obj2->anim_init (true, 0);

        s._app->phys.set_callback (3, entecol_callback);

        s._player_ent = _ent = s._app->phys.ent_add ();
        _ent->type = s.TYPE_PLAYER;
        _ent->weight = s.conf.player_weight;
        _ent->parent = NULL;
        _ent->_obj = s._player_obj;
        _ent->mov_rho = 0.0f;
        _ent->rotate_ = true;
        _ent->bound_only = true;
        _ent->marlo = true;
        math::v_clear (_ent->pos);
        _ent->cell = s._app->cell.classify (_ent->pos);

        s._player_ent2 = _ent = s._app->phys.ent_add ();
        _ent->type = n1_phys::TYPE_IGNORE;
        _ent->parent = NULL;
        _ent->_obj = s._player_obj2;
        _ent->mov_rho = 0.0f;
        _ent->rotate_ = true;
        math::v_clear (_ent->pos);
        _ent->cell = s._app->cell.classify (_ent->pos);
    }

    s._player_ent->mov_rho = 0.0f;
    s.player_mov_theta = 1000.0f; // somewhere out-of-bounds
    s.player_mov_rho = 0.0f;
    math::v_clear (s.player_mov_v);

    math::v_clear (s._player_ent->pos);
    math::v_clear (s._player_ent2->pos);
    s._player_ent->pos[1] = s._player_ent2->pos[1] = -4.0f;
    return true;
}

bool player_input (float t) {
    float ball_pos [3];
    float theta, mx_f, my_f;

    if (s._app->_win->keys[SDL_SCANCODE_ESCAPE]) {
        s._app->_win->keys[SDL_SCANCODE_ESCAPE] = false;
        return false;
    }

    if (s._app->_win->keys[SDL_SCANCODE_SPACE]) {
        s._app->_win->keys[SDL_SCANCODE_SPACE] = false;

        ball_pos[0] = math::randf (-2.0, 2.0);
        ball_pos[1] = math::randf (-2.0, 0.0);
        ball_pos[2] = math::randf (-2.0, 2.0);
        ball_add (ball_pos);
    }

    if (s._app->_win->keys[SDL_SCANCODE_W] &&
        s._app->_win->keys[SDL_SCANCODE_S]) {
    }
    else if (s._app->_win->keys[SDL_SCANCODE_A] &&
        s._app->_win->keys[SDL_SCANCODE_D]) {
    }
    else if (s._app->_win->keys[SDL_SCANCODE_W] &&
            s._app->_win->keys[SDL_SCANCODE_A]) {
        s.player_mov_rho = 1.0f;
        player_walk (s.cam_theta + PI14);
    }
    else if (s._app->_win->keys[SDL_SCANCODE_W] &&
            s._app->_win->keys[SDL_SCANCODE_D]) {
        s.player_mov_rho = 1.0f;
        player_walk (s.cam_theta - PI14);
    }
    else if (s._app->_win->keys[SDL_SCANCODE_S] &&
            s._app->_win->keys[SDL_SCANCODE_A]) {
        s.player_mov_rho = 1.0f;
        player_walk (s.cam_theta - PI - PI14);
    }
    else if (s._app->_win->keys[SDL_SCANCODE_S] &&
            s._app->_win->keys[SDL_SCANCODE_D]) {
        s.player_mov_rho = 1.0f;
        player_walk (s.cam_theta - PI + PI14);
    }
    else if (s._app->_win->keys[SDL_SCANCODE_W]) {
        s.player_mov_rho = 1.0f;
        player_walk (s.cam_theta);
    }
    else if (s._app->_win->keys[SDL_SCANCODE_S]) {
        s.player_mov_rho = 1.0f;
        player_walk (s.cam_theta - PI);
    }
    else if (s._app->_win->keys[SDL_SCANCODE_A]) {
        s.player_mov_rho = 1.0f;
        player_walk (s.cam_theta + PI12);
    }
    else if (s._app->_win->keys[SDL_SCANCODE_D]) {
        s.player_mov_rho = 1.0f;
        player_walk (s.cam_theta - PI12);
    }

    // mouse
    if (s._app->_win->mb0_) {
        // convert to polar
        mx_f = s._app->_win->mx_f;
        my_f = s._app->_win->my_f;

        s.player_mov_rho = sqrtf (mx_f * mx_f + my_f * my_f) * 4.0f;
        if (s.player_mov_rho > s.conf.player_max_rho)
            s.player_mov_rho = s.conf.player_max_rho;

        theta = atan2f (-mx_f, -my_f);
        player_walk (s.cam_theta + theta);
    }
    return true;
}

void player_tick (float t) {
    float m_v [3][4], m_orig [3][4], v [3];
    n1_phys::ent *_ent;
 
    math::v_copy (s._player_ent2->pos, s._player_ent->pos);
    s._player_ent2->cell = s._player_ent->cell;

    s._player_obj2->anim_t += s._player_ent->mov_rho * t *
        s.conf.player_anim_r;
    if (s._player_obj2->anim_t >= s._player_obj2->anim_total)
        s._player_obj2->anim_t -= s._player_obj2->anim_total;
    s._player_obj2->anim ();

    _ent = s._player_ent;

    // rotation
    v[0] = _ent->mov_v[2];
    v[1] = 0.0f;
    v[2] = -_ent->mov_v[0];
    math::v_scalef (v, _ent->mov_rho * t);
    math::m_from_r (m_v, v);
    math::m_copy (m_orig, _ent->rmat);
    math::m_concat (_ent->rmat, m_v, m_orig);

    // rotation 2
    if (_ent->mov_rho > 0.0f) {
        v[0] = _ent->mov_v[0];
        v[1] = _ent->mov_v[1];
        v[2] = _ent->mov_v[2];
        math::v_norm (v);
        math::spherical_from_v (v, NULL, NULL, &s.player_body_target_theta);
        s.player_body_theta = math::a_shift (s.player_body_theta, 
            s.player_body_target_theta, t * PI2 * s.conf.player_shift_r);
    }

    v[0] = 0.0f;
    v[1] = s.player_body_theta;
    v[2] = 0.0f;
    math::m_from_r (s._player_ent2->rmat, v);

    // phys
    if (_ent->mov_rho) {
        if ((! _ent->jumping) && (! _ent->falling)) {
            _ent->mov_rho *= s.conf.ent_land_friction;
        }

        if (_ent->mov_rho < s.conf.rho_cutoff)
            _ent->mov_rho = 0.0f;
    }
}

void player_walk (float mov_theta) {
    float v1 [3], v2 [3];
    n1_phys::ent *_ent;

    _ent = s._player_ent;

    if ((_ent->jumping) || (_ent->falling))
        return;

    math::a_bound (&mov_theta);

    if (s.player_mov_theta != mov_theta) {
        s.player_mov_theta = mov_theta;
        s.player_mov_v[0] = -sin (s.player_mov_theta);
        s.player_mov_v[1] = 0.0f;
        s.player_mov_v[2] = -cos (s.player_mov_theta);
        math::v_norm (s.player_mov_v);
    }

    math::v_scalef2 (v1, _ent->mov_v, _ent->mov_rho);
    math::v_scalef2 (v2, s.player_mov_v, s.player_mov_rho *
        s.conf.player_mov_r);
    math::v_add (v1, v2);
    _ent->mov_rho = math::v_len (v1);
    math::v_norm2 (_ent->mov_v, v1);
    if (_ent->mov_rho > s.conf.player_max_rho)
        _ent->mov_rho = s.conf.player_max_rho;
}

