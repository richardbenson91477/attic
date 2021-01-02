#include "mm.hpp"

bool player_tick (float t) {
    // anim
    s._player_obj->anim_t += t;
    if (s._player_obj->anim_t >= s._player_obj->anim_total)
        s._player_obj->anim_t += -s._player_obj->anim_total;
    s._player_obj->anim ();

    // phys
    if (s._player_ent->mov_rho) {
        s._player_ent->mov_v[0] = -(float) sin (s.player_mov_theta);
        s._player_ent->mov_v[1] = 0.0f;
        s._player_ent->mov_v[2] = -(float) cos (s.player_mov_theta);

        if ((! s._player_ent->jumping) && (! s._player_ent->falling)) {
            if (s._player_ent->mov_rho < s.conf.move_r) {
                if (s._player_obj->seq_id != SEQ_IDLE) 
                    s._player_obj->anim_set_seq (SEQ_IDLE);
            }

            // dry land friction
            s._player_ent->mov_rho *= 0.8f;

        }
        else {
            // air friction
            s._player_ent->mov_rho *= 0.95f;
        }

        // cutoff
        if (s._player_ent->mov_rho < 0.01f)
            s._player_ent->mov_rho = 0.0f;
    }

    if (! s.can_jump_) {
        s.can_jump_t += t;
        if (s.can_jump_t >= 0.3f) {
            s.can_jump_ = true;
            s.can_jump_t = 0.0f;
        }
    }

    return true;
}

bool player_init () {
    s._player_obj = new n1_obj (s._app);
    if (! s._player_obj->load (s.conf.player_model_file_s))
        return false;

    s._player_ent = s._app->phys.ent_add ();
    s._player_ent->type = s.TYPE_PLAYER;
    s._player_ent->weight = s.conf.player_weight;
    s._player_ent->jump_r = s.conf.jump_r;
    s._player_ent->_obj = s._player_obj;
    s._player_ent->dont_render = 1;
    s._player_ent->marlo = 1;
    s._player_ent->bound_only = 1;

    s._app->phys.set_callback (s.TYPE_PLAYER, player_collision);

    s._player_joint_head = (n1_obj::joint *) s._player_obj->joints.get (19);
    if (! s._player_joint_head)
        return false;

    s._player_joint_waist = (n1_obj::joint *)
        s._player_obj->joints.get (15);
    if (! s._player_joint_waist)
        return false;

    s._player_obj->anim_init (true, SEQ_IDLE);

    s.player_heady = s._player_joint_head->m_final[1][3];
    return true;
}

bool player_reset () {
    s._app->disp.light_apply ();

    s.player_pon_item = NULL;

    math::v_copy (s._player_ent->pos, s.map_home);
    s._player_ent->mov_rho = 0.0f;
    s._player_ent->jumping = false;
    s._player_ent->falling = false;
    s._player_ent->cell = s._app->cell.classify (s._player_ent->pos);

    s.player_phi = 0.0f;
    s.player_theta = 0.0f;
    s.player_mov_theta = 0.0f;
    s.player_body_theta = 0.0f;

    s.player_walk_seq_c = SEQ_WALK;
    s._player_obj->anim_set_seq (SEQ_IDLE);
    
    player_move_head ();
    return true;
}

uint8_t player_input (float t) {
    float theta;
    static int32_t last_mx = 0, last_my = 0;

    if (last_my != s._app->_win->my) {
        s.player_phi = -s._app->_win->my_f * PI * s.conf.my_r;
        if (last_mx != s._app->_win->mx)
            player_move_head ();
    }

    if (last_mx != s._app->_win->mx) {
        s.player_theta = -s._app->_win->mx_f * PI2 * s.conf.mx_r;
        
        theta = s.player_theta - s.player_body_theta; 
        math::a_bound (&theta);
        if ((theta > PI14) && (theta < PI12))
            s.player_body_theta = s.player_theta - PI14;
        else if ((theta < PI2_78) && (theta > PI2_34))
            s.player_body_theta = s.player_theta + PI14;
        math::a_bound (&s.player_body_theta);

        player_move_head ();
    }

    last_mx = s._app->_win->mx;
    last_my = s._app->_win->my;

    if (s._app->_win->keys [SDL_SCANCODE_W] &&
        s._app->_win->keys [SDL_SCANCODE_S]) {
    }
    else if (s._app->_win->keys [SDL_SCANCODE_A] &&
        s._app->_win->keys [SDL_SCANCODE_D]) {
    }
    else if (s._app->_win->keys [SDL_SCANCODE_W] &&
            s._app->_win->keys [SDL_SCANCODE_A]) {
        s.player_walk_seq_c = SEQ_WALKLEFT;
        player_walk (s.player_theta + PI14);
    }
    else if (s._app->_win->keys [SDL_SCANCODE_W] &&
            s._app->_win->keys [SDL_SCANCODE_D]) {
        s.player_walk_seq_c = SEQ_WALKRIGHT;
        player_walk (s.player_theta - PI14);
    }
    else if (s._app->_win->keys [SDL_SCANCODE_S] &&
            s._app->_win->keys [SDL_SCANCODE_A]) {
        s.player_walk_seq_c = SEQ_WALKLEFT;
        player_walk (s.player_theta - PI - PI14);
    }
    else if (s._app->_win->keys [SDL_SCANCODE_S] &&
            s._app->_win->keys [SDL_SCANCODE_D]) {
        s.player_walk_seq_c = SEQ_WALKRIGHT;
        player_walk (s.player_theta - PI + PI14);
    }
    else if (s._app->_win->keys [SDL_SCANCODE_W]) {
        s.player_walk_seq_c = SEQ_WALK;
        player_walk (s.player_theta);
    }
    else if (s._app->_win->keys [SDL_SCANCODE_S]) {
        s.player_walk_seq_c = SEQ_WALK;
        player_walk (s.player_theta - PI);
    }
    else if (s._app->_win->keys [SDL_SCANCODE_A]) {
        s.player_walk_seq_c = SEQ_WALKLEFT;
        player_walk (s.player_theta + PI12);
    }
    else if (s._app->_win->keys [SDL_SCANCODE_D]) {
        s.player_walk_seq_c = SEQ_WALKRIGHT;
        player_walk (s.player_theta - PI12);
    }

    if (! s._app->_win->keys [SDL_SCANCODE_LSHIFT]) {
        if (s._app->_win->keys [SDL_SCANCODE_H]) {
            s.player_theta += s.conf.turn_r * t;
            math::a_bound (&s.player_theta);

            theta = s.player_theta - s.player_body_theta;
            math::a_bound (&theta);

            if ((theta > PI14) && (theta < PI12)) {   
                s.player_body_theta = s.player_theta - PI14;
                math::a_bound (&s.player_body_theta);
            }

            player_move_head ();
        }
        else if (s._app->_win->keys [SDL_SCANCODE_L]) {
            s.player_theta += -s.conf.turn_r * t;
            math::a_bound (&s.player_theta);

            theta = s.player_body_theta - s.player_theta;
            math::a_bound (&theta);

            if ((theta > PI14) && (theta < PI12)) {
                s.player_body_theta = s.player_theta + PI14;
                math::a_bound (&s.player_body_theta);
            }
            player_move_head ();
        }

        if (s._app->_win->keys [SDL_SCANCODE_J]) {
            s.player_phi += -s.conf.turn_r * 0.5f * t;
            if (s.player_phi > PI14)
                s.player_phi = PI14;
            if (s.player_phi < -PI14)
                s.player_phi = -PI14;
            player_move_head ();
        }
        else if (s._app->_win->keys [SDL_SCANCODE_K]) {           
            s.player_phi += s.conf.turn_r * 0.5f * t;
            if (s.player_phi > PI14)
                s.player_phi = PI14;
            if (s.player_phi < -PI14)
                s.player_phi = -PI14;
            player_move_head ();
        }
        else if (s._app->_win->keys [SDL_SCANCODE_U]) {           
            s.player_phi = 0.0f;
            player_move_head ();
        }
    }
        
    if ((s._app->_win->mb2_) || (s._app->_win->keys [SDL_SCANCODE_SPACE])) {
        player_jump (t);
    }
    if (s._app->_win->keys [SDL_SCANCODE_ESCAPE]) {
        s._app->_win->keys [SDL_SCANCODE_ESCAPE] = 0;
        return s.RES_QUIT;
    }

    return 0;
}

void player_move_head () {
    n1_obj::joint *_joint;

    cam_to_head ();

    _joint = s._player_joint_head;

    _joint->rot[2] = -s.player_phi * .8f;
    _joint->rot[0] = s.player_theta - s.player_body_theta;

    math::m_from_r_p (_joint->m_relative, _joint->rot, _joint->pos);
    if (_joint->parent)
        math::m_concat (_joint->m_final, _joint->parent->m_final,
            _joint->m_relative);
    else
        math::m_copy (_joint->m_final, _joint->m_relative);

    _joint = s._player_joint_waist;
    _joint->rot[2] = -s.player_phi * .4f;

    math::m_from_r_p (_joint->m_relative, _joint->rot, _joint->pos);
    if (_joint->parent)
        math::m_concat (_joint->m_final, _joint->parent->m_final,
            _joint->m_relative);
    else
        math::m_copy (_joint->m_final, _joint->m_relative);
}

void player_render () {
    s._app->disp.mview_push ();
    s._app->disp.mview_trans (s._player_ent->pos);
    s._app->disp.light_apply_pos ();
    s._app->disp.mview_rot (0.0, s.player_body_theta + PI, 0.0);
    s._app->disp.color4 (1.0f, 1.0f, 1.0f, 1.0f);
    s._player_obj->render (NULL, NULL);
    s._app->disp.mview_pop ();
}

void player_walk (float mov_theta) {
    n1_phys::ent *_ent;

    _ent = s._player_ent;

    if ((_ent->jumping) || (_ent->falling))
        return;

    if (s.player_mov_theta != mov_theta) {
        s.player_mov_theta = mov_theta;
        math::a_bound (&s.player_mov_theta);

        s.player_body_theta = s.player_theta;
        player_move_head ();
    }

    if (s._player_obj->seq_id != s.player_walk_seq_c)
        s._player_obj->anim_set_seq (s.player_walk_seq_c);

    _ent->mov_rho = s.conf.move_r;
}

void player_jump (float t) {
    if ((! s.can_jump_) || (s._player_ent->falling) ||
            (s._player_ent->jumping))
        return;

    s.can_jump_ = false;
    s.can_jump_t = 0;

    if (s._player_obj->seq_id != SEQ_JUMP) 
        s._player_obj->anim_set_seq (SEQ_JUMP);

    s._snd_jump->play (false);
    s._snd_jump->pos3d (s.cam_pos);

    s.player_pon_item = NULL;
    
    s._app->phys.jump (s._player_ent, t);
}

void player_collision (const n1_phys::ecol *pcol) {
    item *_item = NULL;

    if (pcol->hit_type == s.TYPE_ITEM)
        _item = (item *)pcol->hit_parent;

    switch (pcol->col_type) {
        case n1_phys::COL_MOVE:
            if (_item)
                item_activate (_item, "player", "body");
            break;
    
        case n1_phys::COL_HEAD:
            if (s._player_obj->seq_id != SEQ_IDLE) 
                s._player_obj->anim_set_seq (SEQ_IDLE);
    
            if (_item)
                item_activate (_item, "player", "body");
            break;  
    
        case n1_phys::COL_GROUND:
            if (s._player_obj->seq_id == SEQ_JUMP)
                s._player_obj->anim_set_seq (SEQ_IDLE);
    
            if (_item) {
                if (_item != s.player_pon_item) {
                    item_activate (_item, "player", "feet");
                    s.player_pon_item = _item;
                }
            }
       
            break;
    
        case n1_phys::COL_FALL:
            s.player_pon_item = NULL;
            break;
    }
}

