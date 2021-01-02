#include "mm.hpp"

void conf_t::defaults () {
    this->win_w = 960;
    this->win_h = 600;
    this->full_ = false;
    this->fps = 30.0f;
    this->move_r = 5.0f;
    this->turn_r = 2.0f;
    this->cam_col_d = 0.2f;
    this->player_weight = 30.0f;
    this->mx_r = 2.0f;
    this->my_r = 1.0f;
    this->jump_r = 10.0f;
    this->fire_r = 0.05f;
    this->player_s = "player";
    this->map_file_s = "test1.map";
    this->player_model_file_s = "player.mmo";
}

