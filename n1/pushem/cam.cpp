#include "pushem.hpp"

void cam_init () {
    s.cam_phi = s.conf.cam_phi;
    s.cam_theta = 0.0f;
    s.cam_rho = s.cam_rho_t = s.conf.cam_rho;
    s.cam_col = false;
    cam_pos ();
}

bool cam_input (float t) {
    if (s._app->_win->keys[SDL_SCANCODE_U]) {
        if (s.cam_col)
            s.cam_rho_t = s.cam_rho;
        s.cam_rho_t += s.conf.cam_key_rho_r * t;
    }
    else if (s._app->_win->keys[SDL_SCANCODE_O]) {
        if (s.cam_col)
            s.cam_rho_t = s.cam_rho;
        s.cam_rho_t -= s.conf.cam_key_rho_r * t;
        if (s.cam_rho_t < 0.0f)
            s.cam_rho_t = 0.0f;
    }
    else if (s._app->_win->keys[SDL_SCANCODE_L]) {
        s.cam_theta += s.conf.cam_key_r * t;
        math::a_bound (&s.cam_theta);
    }
    else if (s._app->_win->keys[SDL_SCANCODE_H]) {
        s.cam_theta -= s.conf.cam_key_r * t;
        math::a_bound (&s.cam_theta);
    }
    else if (s._app->_win->keys[SDL_SCANCODE_K]) {
        s.cam_phi += 0.5f * s.conf.cam_key_r * t;
        math::a_bound (&s.cam_phi);
    }
    else if (s._app->_win->keys[SDL_SCANCODE_J]) {
        s.cam_phi -= 0.5f * s.conf.cam_key_r * t;
        math::a_bound (&s.cam_phi);
    }

    if (s._app->_win->mb1_ || s._app->_win->keys[SDL_SCANCODE_LCTRL]) {
        s.cam_theta -= s._app->_win->mx_f * s.conf.cam_mouse_s;
        math::a_bound (&s.cam_theta);

        s.cam_phi -= s._app->_win->my_f * s.conf.cam_mouse_s;
        math::a_bound (&s.cam_phi);
    }

    if (s._app->_win->mb2_ || s._app->_win->keys[SDL_SCANCODE_LALT] ) {
        if (s.cam_col)
            s.cam_rho_t = s.cam_rho;

        s.cam_rho_t -= s._app->_win->my_f * s.conf.cam_mouse_s;
    }
    return true;
}

void cam_tick (float t) {
    float v [3];
    n1_phys::ecol col;
    bool res_;

    cam_pos ();

    // camera collision
    if (! s.cam_col) {
        math::v_from_spherical (v, s.cam_rho + s.conf.cam_col_dist,
            s.cam_phi, s.cam_theta);
        res_ = s._app->phys.collide_ray_all (s._player_ent->pos, v, &col,
            s._player_ent);
        if (res_ && (col.hit_type == n1_phys::TYPE_CELL)) {
            s.cam_col = true;
            s.cam_col_rho = s.cam_rho;
            math::v_sub2 (v, s._player_ent->pos, col.isec);
            s.cam_rho = math::v_len (v) - s.conf.cam_col_dist;
            if (s.cam_rho < 0.0f)
                s.cam_rho = 0.0f;
            cam_pos ();
        }
    }
    else {
        if (s.cam_rho_t < s.cam_rho) 
            s.cam_col_rho = s.cam_rho_t;

        math::v_from_spherical (v, s.cam_col_rho + s.conf.cam_col_dist,
            s.cam_phi, s.cam_theta);
        res_ = s._app->phys.collide_ray_all (s._player_ent->pos, v, &col,
            s._player_ent);
        if (res_ && (col.hit_type == n1_phys::TYPE_CELL)) {
            math::v_sub2 (v, s._player_ent->pos, col.isec);
            s.cam_rho = math::v_len (v) - s.conf.cam_col_dist;
            if (s.cam_rho < 0.0f)
                s.cam_rho = 0.0f;
            cam_pos ();
        }
        else
            s.cam_col = false;
    }

    if ((s.cam_rho != s.cam_rho_t) && (! s.cam_col))
        s.cam_rho = s.cam_rho_t;
}

void cam_pos () {
    math::v_from_spherical (s._app->disp.eye_pos, s.cam_rho, s.cam_phi,
        s.cam_theta);
    math::v_add (s._app->disp.eye_pos, s._player_ent->pos);
    s._app->disp.eye_phi = s.cam_phi;
    s._app->disp.eye_theta = s.cam_theta;
}

