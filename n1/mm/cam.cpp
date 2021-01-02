#include "mm.hpp"

void cam_init () {
    math::v_clear (s.cam_pos);
    s.cam_theta = 0.0f;
    s.cam_phi = PI12;
    s.cam_rho = 3.0f;
    s.cam_target_rho = s.cam_rho;

    s.cam_col_ = false;
    s.cam_col_rho = 0.0f;
}

void cam_tick () {
    n1_phys::ecol col;
    float ray [3];
    float pos_rel [3];
    int32_t res;

    cam_get_pos ();
    math::v_copy (s._app->disp.eye_pos, s.cam_pos);
    s._app->disp.eye_phi = s.cam_phi;
    s._app->disp.eye_theta = s.cam_theta;

    // cam collision
    if (! s.cam_col_) {
        math::v_from_spherical (ray, s.cam_rho + s.conf.cam_col_d, s.cam_phi,
            s.cam_theta);

        res = s._app->phys.collide_ray_all (s._player_ent->pos, ray, &col,
            s._player_ent);
        if ((res) && (col.hit_type == n1_phys::TYPE_CELL)) {
            s.cam_col_ = true;
            s.cam_col_rho = s.cam_rho;

            math::v_sub2 (pos_rel, s._player_ent->pos, col.isec);
            s.cam_rho = math::v_len (pos_rel) - s.conf.cam_col_d;

            if (s.cam_rho < 0.0f)
                s.cam_rho = 0.0f;

            cam_get_pos ();
            math::v_copy (s._app->disp.eye_pos, s.cam_pos);
        }
    }
    else {
        if (s.cam_target_rho < s.cam_rho) 
            s.cam_col_rho = s.cam_target_rho;

        math::v_from_spherical (ray, s.cam_col_rho + s.conf.cam_col_d,
            s.cam_phi, s.cam_theta);

        res = s._app->phys.collide_ray_all (s._player_ent->pos, ray, &col,
            s._player_ent);
        if (res && (col.hit_type == n1_phys::TYPE_CELL)) {
            math::v_sub2 (pos_rel, s._player_ent->pos, col.isec);
            s.cam_rho = math::v_len (pos_rel) - s.conf.cam_col_d;

            if (s.cam_rho < 0.0f)
                s.cam_rho = 0.0f;

            cam_get_pos ();
            math::v_copy (s._app->disp.eye_pos, s.cam_pos);
        }
        else
            s.cam_col_ = false;
    }

    if ((s.cam_rho != s.cam_target_rho) && (! s.cam_col_))
        s.cam_rho = s.cam_target_rho;
}

void cam_input (float t) {
    if (s._app->_win->keys [SDL_SCANCODE_LSHIFT]) {
        if (s._app->_win->keys [SDL_SCANCODE_U]) {
            if (s.cam_col_)
                s.cam_target_rho = s.cam_rho;

            s.cam_target_rho += s.conf.turn_r * t;
            if (s.cam_target_rho < 0.0f)
                s.cam_target_rho = 0.0f;
        }

        if (s._app->_win->keys [SDL_SCANCODE_O]) {
            if (s.cam_col_)
                s.cam_target_rho = s.cam_rho;

            s.cam_target_rho += -s.conf.turn_r * t;
            if (s.cam_target_rho < 0.0f)
                s.cam_target_rho = 0.0f;
        }
        if (s._app->_win->keys [SDL_SCANCODE_L]) {
            s.cam_theta += -s.conf.turn_r * t;
            math::a_bound (&s.cam_theta);
        }
        else if (s._app->_win->keys [SDL_SCANCODE_H]) {
            s.cam_theta += s.conf.turn_r * t;
            math::a_bound (&s.cam_theta);
        }
        if (s._app->_win->keys [SDL_SCANCODE_K]) {
            s.cam_phi += s.conf.turn_r * t;
            math::a_bound (&s.cam_phi);
        }
        else if (s._app->_win->keys [SDL_SCANCODE_J]) {
            s.cam_phi += -s.conf.turn_r * t;
            math::a_bound (&s.cam_theta);
        }
    }
}

void cam_to_head () {
    s.cam_phi = s.player_phi + PI12;
    s.cam_theta = s.player_theta;
}

void cam_get_pos () {
    math::v_from_spherical (s.cam_pos, s.cam_rho, s.cam_phi, s.cam_theta);
    math::v_add (s.cam_pos, s._player_ent->pos);
    s.cam_pos[1] += s.player_heady;
}

void cam_set_pos (float pos[3]) {
    float theta_orig = s.cam_theta;

    pos[1] += -s.player_heady;
    math::v_sub (pos, s._player_ent->pos);

    math::spherical_from_v (pos, &s.cam_rho, &s.cam_phi, &s.cam_theta);

    if ((s.cam_phi < 0.0f) || (s.cam_phi > PI))
        s.cam_theta = theta_orig;

    cam_get_pos ();
}

