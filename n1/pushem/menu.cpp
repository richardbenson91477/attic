#include "pushem.hpp"

int32_t menu_tick (float t) {
    int32_t res = false;
    uint32_t logo_s;

    res = menu_input ();
    switch (res) {
        case 1:
            return 0;
            break;

        case 2:
            s._app->disp.fade (0.0f, 1.0f, s.conf.menu_fade_t);
            s.menu_select_ = true;
            break;
    }

    s.cam_t += t;
    while (s.cam_t > PI2)
        s.cam_t -= PI2;
    
    s.logo_t += t;
    if (s.logo_t >= 1.0f)
        s.logo_t = 1.0f;

    logo_s = (int32_t)(pow (s.logo_t, -5) * 50.0f);
    s.menu_rgns[0].top = s.logo_top - logo_s;
    s.menu_rgns[0].bottom = s.logo_bottom + logo_s;
    s.menu_rgns[0].left = s.logo_left - logo_s;
    s.menu_rgns[0].right = s.logo_right + logo_s;

    s.cam_theta = PI + (float) sin (s.cam_t) * PI;
    math::a_bound (&s.cam_theta);
    s.cam_phi = PI + (float) sin (s.cam_t) * PI;
    math::a_bound (&s.cam_phi);

    math::v_from_spherical (s._app->disp.eye_pos, s.cam_rho, s.cam_phi,
        s.cam_theta);
    s._app->disp.eye_phi = s.cam_phi;
    s._app->disp.eye_theta = s.cam_theta;

    if (s.menu_select_) {
        s.fade_t += t;
        if (s.fade_t >= s.conf.menu_fade_t) {
            s.fade_t = 0.0f;
            s.cam_t = 0.0f;
            s.menu_select_ = false;
            s.menu_en = false;
            play ();
        }
    }

    return 1;
}

void menu_render () {
    s._app->disp.clear ();
    s._app->disp.cam ();
    s._app->cell.render ();
    s._app->disp.ortho (true, s._app->_win->w, s._app->_win->h);
    s._app->rgn.render (s.tex_logo, NULL, 0, s.menu_rgns, -1);
    if (s.menu_ptr_o != 1)
        s._app->rgn.render (s.tex_opts, s.menu_rgns + 1, 0,
            s.menu_rgns + 2, 0);
    else
        s._app->rgn.render (s.tex_opts2, s.menu_rgns + 1, 0,
            s.menu_rgns + 2, 0);
    if (s.menu_ptr_o != 2)
        s._app->rgn.render (s.tex_opts, s.menu_rgns + 1, 1,
            s.menu_rgns + 2, 1);
    else
        s._app->rgn.render (s.tex_opts2, s.menu_rgns + 1, 1,
            s.menu_rgns + 2, 1);
    s._app->disp.tex_render (s.tex_pointer, s._app->_win->mx,
        s._app->_win->my, s.conf.menu_ptr_xs, s.conf.menu_ptr_ys);
    s._app->disp.ortho (false, 0.0, 0.0);
}

int32_t menu_input () {
    int32_t o;
    static int32_t last_mx = 0, last_my = 0;

    if (s._app->_win->keys[SDL_SCANCODE_ESCAPE]) {
        s._app->_win->keys[SDL_SCANCODE_ESCAPE] = 0;
        return 1;
    }

    if ((s._app->_win->keys [SDL_SCANCODE_RETURN]) || (s._app->_win->mb0_)) {
        if (! s.menu_select_) {
            if (s.menu_ptr_o == 1)
                return 2;
            else if (s.menu_ptr_o == 2)
                return 1;
        }
    }

    if ((s._app->_win->mx != last_mx) || (s._app->_win->my != last_my)) {
        if (s._app->rgn.classify (s._app->_win->mx, s._app->_win->my,
                &o, s.menu_rgns + 2, 1)) {
            if ((o + 1) != s.menu_ptr_o) {
                s._snd_menu->play (false);
                s.menu_ptr_o = o + 1;
            }
        }
        else
            s.menu_ptr_o = 0;
    }
    
    last_mx = s._app->_win->mx; 
    last_my = s._app->_win->my;
    
    if (s._app->_win->keys[SDL_SCANCODE_W]) {
        s._app->_win->keys[SDL_SCANCODE_W] = 0;
        s.menu_ptr_o --;
        if (s.menu_ptr_o < 0)
            s.menu_ptr_o = 2;
    }
    else if (s._app->_win->keys[SDL_SCANCODE_S]) {
        s._app->_win->keys[SDL_SCANCODE_S] = 0;
        s.menu_ptr_o ++;
        if (s.menu_ptr_o > 2)
            s.menu_ptr_o = 0;
    }

    return 0;
}

bool menu_init () {
    static bool first_ = true;
    n1_disp::tex *_tex;
    int32_t x1, x2, y1;

    if (first_) {
        first_ = false;

        if (! (_tex = s._app->disp.tex_load ("pushem.png")) )
            return false;
        s.tex_logo = _tex->gl_texid;

        if (! (_tex = s._app->disp.tex_load ("options.png")) )
            return false;
        s.tex_opts = _tex->gl_texid;

        if (! (_tex = s._app->disp.tex_load ("options2.png")) )
            return false;
        s.tex_opts2 = _tex->gl_texid;

        if (! (_tex = s._app->disp.tex_load ("pointer.png")) )
            return false;
        s.tex_pointer = _tex->gl_texid;

        if (! (_tex = s._app->disp.tex_load ("explain.png")) )
            return false;
        s.tex_expl = _tex->gl_texid;
        
        if (! (_tex = s._app->disp.tex_load ("chars.png")) )
            return false;
        s.tex_chars = _tex->gl_texid;

        s._snd_menu = new snd ("menu.raw");

        x1 = s._app->_win->w / 2;
        y1 = s._app->_win->h / 4;
        x2 = s._app->_win->w / 5;

        // logo rgn      
        s.menu_rgns[0].top = s.logo_top = y1 - x2;
        s.menu_rgns[0].bottom = s.logo_bottom = y1 + x2 - 1;
        s.menu_rgns[0].left = s.logo_left = x1 - x2;
        s.menu_rgns[0].right = s.logo_right = x1 + x2 - 1;
        s.menu_rgns[0].col_s = s.menu_rgns[0].right -
            s.menu_rgns[0].left + 1;
        s.menu_rgns[0].row_s = s.menu_rgns[0].bottom -
            s.menu_rgns[0].top + 1;

        // options tex rgn
        s.menu_rgns[1].top = 255;
        s.menu_rgns[1].bottom = 0;
        s.menu_rgns[1].left = 0;
        s.menu_rgns[1].right = 255;
        s.menu_rgns[1].row_s = 128;
        s.menu_rgns[1].col_s = 256;

        // options rgn
        x2 = s._app->_win->w / 10;
        y1 = s._app->_win->h / 2 + s._app->_win->h / 6;

        s.menu_rgns[2].top = y1 - x2;
        s.menu_rgns[2].bottom = y1 +x2 - 1;
        s.menu_rgns[2].left = x1 -x2;
        s.menu_rgns[2].right = x1 +x2 - 1;
        s.menu_rgns[2].col_s = s.menu_rgns[2].right -
            s.menu_rgns[2].left + 1;
        s.menu_rgns[2].row_s = (s.menu_rgns[2].bottom -
            s.menu_rgns[2].top + 1) / 2;
        s._app->rgn.init (s.menu_rgns, 3); 
    }

    s.balls.destroy (ball_destroy);

    s.cam_rho = 0.1f;

    s.menu_ptr_o = 1;
    s.menu_select_ = false;
    s.menu_en = true;
    return true;
}

