 #include "pushem.hpp"

int main (int argc, char *argv[]) {
    float t, step_t;
    uint32_t sleep_t;
    
    if (! init (true)) {
        dlog::error ("pushem", "init failed");
        return false;
    }

    step_t = 1.0f / s._app->conf.fps;
    sleep_t = (int32_t)(step_t * 2000.0f);
    s._app->tick (step_t);
 
    t = 0.0;
    for (;;) {
        t += timer_tick ();
        if (t >= step_t) {
            t -= step_t;
            if (s.menu_en) {
                if (! menu_tick (step_t))
                    break;
                if (! s._app->tick (step_t))
                    break;
                menu_render ();
            }
            else {
                if (! s._app->tick (step_t))
                    break;

                if (! tick (step_t)) {
                    menu_init ();
                    continue;
                }
                render ();
            }
            s._app->render ();
        }
        else
            usleep (sleep_t);
    }
    
    init (false);
    return 0;
}

bool init (bool which) {
    if (which) {
        s._app = new n1_app ();
        if (! s._app) {
            dlog::error ("pushem", "create n1_app failed");
            return false;
        }
        
        s.conf.defaults ();
    
        s._app->conf.defaults ();
        s._app->conf.win_w = s.conf.win_w;
        s._app->conf.win_h = s.conf.win_h;
        s._app->conf.full_ = s.conf.full_;
        sprintf (s._app->conf.win_s, "%s", s.conf.win_s); 

        if (! s._app->init (true)) {
            dlog::error ("init", "_app->init");
            return false;
        }
        
        s._app->disp.light_pos[1] = s.conf.light_y;
        s._app->disp.light_apply ();

        s._obj_trail = new n1_obj (s._app);
        if (! s._obj_trail->load ("trail.mmo"))
            return false;

        s._app->cell.load ("level1.mcm");

        if (! goal_init ())
            return false;

        if (! ball_init (true))
            return false;

        menu_init ();
    }
    else {   
        ball_init (false);
        goal_destroy ();
        if (s._app) {
            s._app->init (false);
            delete s._app;
            s._app = NULL;
        }
    }

    return true;
}

bool tick (float t) {
    int res = false;

    if (s.expl_c) {
        s.expl_t += t;
        if (s.expl_t >= s.conf.expl_t) {
            s.expl_t = 0.0f;
            s.expl_c ++;
            if (s.expl_c == 5)
                s.expl_c = 0;
        }
        else {
            switch (s.expl_c) {
                case 1:
                    s.expl_x = s.expl_e0 + (int32_t)((s.expl_t /
                        s.conf.expl_t) * (s.expl_e1 - s.expl_e0));
                    break;
                case 2:
                case 3:
                    s.expl_x = s.expl_e1;
                    break;
                case 4:
                    s.expl_x = s.expl_e1 + (int32_t)((s.expl_t /
                        s.conf.expl_t) * (s.expl_e2 - s.expl_e1));
                    break;
            }
            
            s.expl_rgn.left = s.expl_x;
            s.expl_rgn.right = s.expl_x + s.expl_w;       
        }
    }

    res = cam_input (t);
    if (! res)
        return false;
    res = player_input (t);
    if (! res)
        return false;

    ball_tick (t);
    goal_tick (t);
    player_tick (t);
    cam_tick (t);
    return true;
}

void render () {
    s._app->disp.clear ();
    s._app->disp.cam ();
    s._app->cell.render ();
 
    s._app->disp.ortho (true, s._app->_win->w, s._app->_win->h);
    if (s.expl_c)
        s._app->rgn.render (s.tex_expl, NULL, 0, &s.expl_rgn, -1);
    s._app->disp.tex_bind (s.tex_chars);
    s._app->disp.ortho (false, 0.0, 0.0);
}

void play () {
    int32_t y1 = s._app->_win->h / 4;
    int32_t x1 = s._app->_win->w / 4;

    s.expl_c = 1;
    s.expl_t = 0.0f;
    s.expl_rgn.top = y1;
    s.expl_rgn.bottom = y1 * 3 - 1;
    s.expl_w = x1 * 2 + 1;
    s.expl_e0 = -s.expl_w;
    s.expl_e1 = s._app->_win->w / 2 - s.expl_w / 2;
    s.expl_e2 = s._app->_win->w;
    s.expl_x = -x1;
    s.expl_rgn.left = s.expl_x;
    s.expl_rgn.right = s.expl_x + s.expl_w;

    player_init ();
    ball_init (true);
    goal_init ();
    cam_init ();
}

void entecol_callback (const n1_phys::ecol *_col) {
    float tmp [3], tmp2 [3], rr;
    n1_phys::ent *_ent = _col->_ent, *_hit_ent = _col->_hit_ent;

    switch (_col->col_type) {
        case n1_phys::COL_GROUND:
        case n1_phys::COL_MOVE:
            if (_col->hit_type == n1_phys::TYPE_CELL) {
                math::v_reflect (tmp, _ent->mov_v, _col->pn_hit);
                math::v_copy (_ent->mov_v, tmp);

                _ent->mov_rho *= s.conf.cell_reflect;
                if (_ent->mov_rho < s.conf.rho_cutoff) {
                    _ent->mov_rho = 0.0f;
                    math::v_clear (_ent->mov_v);
                }
            }
            else if ((_col->hit_type == s.TYPE_BALL) ||
                    (_col->hit_type == s.TYPE_PLAYER)) {
                rr = _ent->mov_rho * s.conf.ent_reflect;
                math::v_scalef2 (tmp, _ent->mov_v, _ent->mov_rho);
                if (_hit_ent->mov_rho) {
                    math::v_scalef2 (tmp2, _hit_ent->mov_v,
                        _hit_ent->mov_rho);
                    math::v_add (tmp, tmp2);
                }
                math::v_norm (tmp);
                if (math::v_len (tmp) >= 0.99f) {
                    _hit_ent->mov_rho += rr;
                    if (_hit_ent->mov_rho > s.conf.ball_max_rho)
                        _hit_ent->mov_rho = s.conf.ball_max_rho;
                }
                else {
                    _hit_ent->mov_rho = 0.0f;
                    math::v_clear (_ent->mov_v);
                }

                math::v_copy (_hit_ent->mov_v, tmp);

                if (math::v_is_clear (_col->pn_hit))
                    math::v_scalef (_ent->mov_v, -1.0f);
                else {
                    math::v_reflect (tmp, _ent->mov_v, _col->pn_hit);
                    math::v_copy (_ent->mov_v, tmp);
                }
        
                _ent->mov_rho -= rr;
                if (_ent->mov_rho < s.conf.rho_cutoff) {
                    _ent->mov_rho = 0.0f;
                    math::v_clear (_ent->mov_v);
                }
            }
            break;

        case n1_phys::COL_HEAD:
            break;  

        case n1_phys::COL_FALL:
            break;
    }
}

