#include "mm.hpp"

void menu_init (bool t_) {
    int32_t x1, x2, y1, y2;

    if (t_) {
        s._app->disp.light_apply ();

        // logo rgn
        x1 = s._app->_win->w / 2;
        y1 = (int32_t)((float)s._app->_win->h * 0.3);
        y2 = (int32_t)((float)s._app->_win->h * 0.5);
        x2 = (int32_t)((float)s._app->_win->w * 0.33);

        s.menu_rgns[0].top = y1 - y2;
        s.menu_rgns[0].bottom = y1 + y2 - 1;
        s.menu_rgns[0].left = x1 - x2;
        s.menu_rgns[0].right = x1 + x2 - 1;
        s.menu_rgns[0].col_s = s.menu_rgns[0].right -
            s.menu_rgns[0].left + 1;
        s.menu_rgns[0].row_s = s.menu_rgns[0].bottom -
            s.menu_rgns[0].top + 1;

        // options tex rgn
        s.menu_rgns[1].top = 255;
        s.menu_rgns[1].bottom = 0;
        s.menu_rgns[1].left = 0;
        s.menu_rgns[1].right = 255;
        s.menu_rgns[1].row_s = 80;
        s.menu_rgns[1].col_s = 256;

        // options rgn
        x2 = (s._app->_win->w / 5) / 2;
        x1 = s._app->_win->w / 2;
        y1 = s._app->_win->h / 2 + x2 / 2;

        s.menu_rgns[2].top = y1 - x2;
        s.menu_rgns[2].bottom = y1 + x2 - 1;
        s.menu_rgns[2].left = x1 - x2;
        s.menu_rgns[2].right = x1 + x2 - 1;
        s.menu_rgns[2].col_s = (s.menu_rgns[2].right -
            s.menu_rgns[2].left + 1);
        s.menu_rgns[2].row_s = (s.menu_rgns[2].bottom -
            s.menu_rgns[2].top + 1) / 3;

        s._app->rgn.init (s.menu_rgns, 3);
            
        s._sh_back = s._app->texshader.load ("back.mms");
        s._sh_back2 = s._app->texshader.load ("back2.mms");
    }
    else {
        s._app->texshader.del (s._sh_back);
        s._app->texshader.del (s._sh_back2);
    }
}

uint8_t menu_tick (float t) {
    if (s.pointer_chosen) {       
        if ((s.pointer_sel == 0) && (s.pointer_state == 0)) {
            s.pointer_state ++;
        }
        
        s.pointer_tick += t;
        if (s.pointer_tick > 0.3) {
            s.pointer_tick -= 0.3;
            s.pointer_state ++;

            if (s.pointer_state == 2) {
                s.obj_pointer->anim_set_seq (0);

                if (s.pointer_sel == 0)
                    return s.RES_WIN;
                else if (s.pointer_sel == 1) {
                    s.pointer_state = 0;
                    s.pointer_chosen = false;
                }
                else
                    return s.RES_QUIT;
            }
        }
    }

    s.obj_pointer->anim_t += t;
    if (s.obj_pointer->anim_t >= s.obj_pointer->anim_total)
        s.obj_pointer->anim_t -= s.obj_pointer->anim_total;
    s.obj_pointer->anim ();
    
    return 0;
}

uint8_t menu_input () {
    int32_t o;
    static int32_t last_mx = 0, last_my = 0;

    if (! s.pointer_chosen) {
        if (s._app->_win->keys [SDL_SCANCODE_S]) {
            s._app->_win->keys [SDL_SCANCODE_S] = 0;
            if (s.pointer_sel < 2) {
                s.pointer_sel ++;
                s._snd_pointer->play (false);
            }
        }
        else if (s._app->_win->keys [SDL_SCANCODE_W]) {
            s._app->_win->keys [SDL_SCANCODE_W] = 0;
            if (s.pointer_sel > 0) {
                s.pointer_sel --;
                s._snd_pointer->play (false);
            }
        }
        else if (s._app->_win->keys [SDL_SCANCODE_RETURN]) {
            s.pointer_chosen = true;
            s._snd_selection->play (false);
            s.obj_pointer->anim_set_seq (1);
        }
    }
    
    if (s._app->_win->keys [SDL_SCANCODE_ESCAPE]) {
        s._app->_win->keys [SDL_SCANCODE_ESCAPE] = 0;
        return s.RES_QUIT;
    }

    if ((last_mx != s._app->_win->mx) || (last_my != s._app->_win->my)) {
        s.pointer_pos[0] = s._app->_win->mx;
        s.pointer_pos[1] = s._app->_win->h - s._app->_win->my;

        if (! s.pointer_chosen) {
            if (s._app->rgn.classify (
                    (int32_t)s.pointer_pos[0] - 1, 
                    s._app->_win->h - (int32_t)s.pointer_pos[1] - 1,
                    &o, s.menu_rgns + 2, 1)) {
                if ((o < 3) && (s.pointer_sel != o)) {
                    s.pointer_sel = o;
                    s._snd_pointer->play (false);
                }
            }
        }
    }

    last_mx = s._app->_win->mx;
    last_my = s._app->_win->my;

    if ((s._app->_win->mb0_) && (! s.pointer_chosen)) {
        s.pointer_chosen = true;
        s.obj_pointer->anim_set_seq (1);
        s._snd_selection->play (false);
    }

    return 0;
}

void menu_render () {
    s._app->disp.clear ();
    s._app->disp.ortho (true, s._app->_win->w, s._app->_win->h);

    // back
    s._app->texshader.apply (s._sh_back);
    s._app->disp.tex_render (0, 0, 0, s._app->_win->w, s._app->_win->h);
    s._app->texshader.unapply (s._sh_back);
    s._app->texshader.apply (s._sh_back2);
    s._app->disp.tex_render (0, 0, 0, s._app->_win->w, s._app->_win->h);
    s._app->texshader.unapply (s._sh_back2);

    // logo
    s._app->rgn.render (s.tex_misc[s.TEX_LOGO], NULL, 0,
        s.menu_rgns, - 1);

    // options
    if (s.pointer_sel != 0)
        s._app->rgn.render (s.tex_misc[s.TEX_OPTIONS],
            s.menu_rgns + 1, 0, s.menu_rgns + 2, 0);
    else 
        s._app->rgn.render (s.tex_misc[s.TEX_OPTIONS2],
            s.menu_rgns + 1, 0, s.menu_rgns + 2, 0);

    if (s.pointer_sel != 1)
        s._app->rgn.render (s.tex_misc[s.TEX_OPTIONS], 
            s.menu_rgns + 1, 1, s.menu_rgns + 2, 1);
    else 
        s._app->rgn.render (s.tex_misc[s.TEX_OPTIONS2], 
            s.menu_rgns + 1, 1, s.menu_rgns + 2, 1);

    if (s.pointer_sel != 2)
        s._app->rgn.render (s.tex_misc[s.TEX_OPTIONS], 
            s.menu_rgns + 1, 2, s.menu_rgns + 2, 2);
    else 
        s._app->rgn.render (s.tex_misc[s.TEX_OPTIONS2], 
            s.menu_rgns + 1, 2, s.menu_rgns + 2, 2);

    s._app->disp.mview_push ();
    s._app->disp.mview_reset ();

    s._app->disp.mview_trans (
        (s.pointer_pos[0] / (float)s._app->_win->w * 2.0) - 1.0,
        (s.pointer_pos[1] / (float)s._app->_win->h * 2.0) - 1.0,
        0.0);

    s._app->disp.color4 (1.0, 1.0, 1.0, 1.0);
    s.obj_pointer->render (NULL, NULL);
    s._app->disp.mview_pop ();
    s._app->disp.ortho (false, 0.0, 0.0);
}

bool menu_load (bool t_) {
    if (t_) {
        s.menu_en = true;

        s.pointer_pos[1] = (float)(s._app->_win->h / 2);
        s.pointer_pos[0] = (float)(s._app->_win->w / 2);
        s.pointer_sel = 0;
        s.pointer_chosen = false;
        s.pointer_state = 0;
        s.pointer_tick = 0.0f;
    
        math::v_clear (s._app->disp.eye_pos);
        s._app->disp.eye_phi = PI12;
        s._app->disp.eye_theta = 0.0f;
    }
    else {
        s.menu_en = false;
    }

    s._app->disp.mview_reset ();
    s._app->disp.light_apply ();
    return true;
}

