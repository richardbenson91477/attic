#include "pushem.hpp"

void t_conf::defaults () {
    this->win_w = 640;
    this->win_h = 400;
    this->full_ = false;
    this->fps = 30.0f;
    this->win_s = "pushem";
    this->expl_t = 1.0f;
    this->light_y = 5.0f;
    this->rho_cutoff = 0.001f;
    this->cell_reflect = 0.92f;
    this->ent_reflect = 0.7f;
    this->ent_land_friction = 0.99f;
    this->goal_eat_t = 0.5f;
    this->goal_init_pos[0] = 0.0f;
    this->goal_init_pos[1] = -2.6f;
    this->goal_init_pos[2] = -20.0f;
    this->menu_ptr_xs = 48;
    this->menu_ptr_ys = 48;
    this->menu_fade_t = 1.0f;
    this->cam_key_r = 1.0f;
    this->cam_key_rho_r = 10.0f;
    this->cam_mouse_s = 0.65f;
    this->cam_col_dist = 1.5f;
    this->cam_phi = PI / 3.0f;
    this->cam_rho = 6.0f;
    this->player_max_rho = 20.0f;
    this->player_mov_r = 1.0f;
    this->player_weight = 50.0f;
    this->player_anim_r = 0.1f;
    this->player_shift_r = 1.0f;
    this->ball_max_rho = 20.0f;
    this->ball_weight = 20.0f;
    this->balls_start_n = 3;
    this->ball_start_t = 1.0f;
    this->ball_ptcl_freq = 2.0f;
    this->ball_ptcl_at_r = 0.5f;
}

