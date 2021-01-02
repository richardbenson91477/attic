#include "mm.hpp"

int32_t main (int32_t argc, char *argv[]) {
    float t, tick_t;
    uint32_t sleep_t;

    if (! mm_init (true)) {
        dlog::error ("main", "mm_init (true)");
        return false;
    }

    tick_t = 1.0f / s._app->conf.fps;
    sleep_t = (int32_t)(tick_t * 2000.0f);
    s._app->tick (tick_t);

    t = 0.0;
    for (;;) {
        t += timer_tick ();
        if (t >= tick_t) {
            t -= tick_t;

            if (! s._app->tick (tick_t))
                break;

            if (! mm_tick (tick_t))
                break;

            if (s.menu_en)
                menu_render ();
            else 
                mm_render ();
            
            s._app->render ();
        }
        else
            usleep (sleep_t);
    }

    mm_init (false);
    return 0;
}

bool mm_init (bool t_) {
    n1_disp::tex *_tex;

    if (t_) {
        s._app = new n1_app ();

        s.conf.defaults ();
 
        s._app->conf.defaults ();
        s._app->conf.win_w = s.conf.win_w;
        s._app->conf.win_h = s.conf.win_h;
        s._app->conf.full_ = s.conf.full_;

        if (! s._app->init (true)) {
            dlog::error ("mm_init", "s._app->init (true)");
            return false;
        }

        s._snd_ponger = new snd ("ponger.raw");
        s._snd_pongerhit = new snd ("pongerhit.raw");
        s._snd_missile = new snd ("missile.raw");
        s._snd_missilehit = new snd ("missilehit.raw");
        s._snd_jump = new snd ("jump.raw");
        s._snd_pointer = new snd ("pointer.raw");
        s._snd_selection = new snd ("selection.raw");

        if (! (_tex = s._app->disp.tex_load ("crosshair.png")))
            return false;
        s.tex_misc [s.TEX_CROSSHAIR] = _tex->gl_texid;
        if (! (_tex = s._app->disp.tex_load ("logo.png")))
            return false;
        s.tex_misc [s.TEX_LOGO] = _tex->gl_texid;
        if (! (_tex = s._app->disp.tex_load ("options.png")))
            return false;
        s.tex_misc [s.TEX_OPTIONS] = _tex->gl_texid;
        if (! (_tex = s._app->disp.tex_load ("options2.png")))
            return false;
        s.tex_misc [s.TEX_OPTIONS2] = _tex->gl_texid;
        if (! (_tex = s._app->disp.tex_load ("back.png")))
            return false;
        s.tex_misc [s.TEX_BACK] = _tex->gl_texid;
        if (! (_tex = s._app->disp.tex_load ("back2.png")))
            return false;
        s.tex_misc [s.TEX_BACK2] = _tex->gl_texid;

        s.obj_pointer = new n1_obj (s._app);
        if (! s.obj_pointer->load ("pointer.mmo"))
            return false;
        s.obj_missile = new n1_obj (s._app);
        if (! s.obj_missile->load ("missile.mmo"))
            return false;
        s.obj_ponger = new n1_obj (s._app);
        if (! s.obj_ponger->load ("ponger.mmo"))
            return false;
        s.obj_smoke = new n1_obj (s._app);
        if (! s.obj_smoke->load ("smoke.mmo"))
            return false;
        s.obj_flare = new n1_obj (s._app);
        if (! s.obj_flare->load ("flare.mmo"))
            return false;
        
        s.obj_pointer->anim_init (true, 0);
 
        if (! player_init ()) {
            dlog::error ("mm_init", "player_init");
            return false;
        }

        menu_init (true);
        if (! menu_load (true)) {
            dlog::error ("mm_init", "menu_load");
            return false;
        }
    }
    else {
        if (! s.menu_en)
            map_destroy ();

        menu_init (false);

        if (s._app) {
            s._app->init (false);
            delete s._app;
            s._app = NULL;
        }
    }

    return true;
}

bool mm_tick (float t) {
    uint8_t res_u8 = 0;

    if (s.menu_en) {   
        res_u8 = menu_input ();
        if (! res_u8)
            res_u8 = menu_tick (t);
    }
    else {
        res_u8 = player_input (t);
        if (! res_u8) {
            cam_input (t);
            tool_input ();
        }

        if (! res_u8) {
            if (! items_tick (t))
                return false;
            if (! player_tick (t))
                return false;

            cam_tick ();
            tool_tick (t);
            
            res_u8 = map_tick ();
        }
    }

    if (res_u8 == s.RES_QUIT) {
        if (s.menu_en)
            return false;

        map_destroy ();
        if (! menu_load (true))
            return false;
    }
    else if (res_u8 == s.RES_WIN) {        
        if (s.menu_en)
            menu_load (false);

        if (! map_load ()) {
            dlog::error ("map_load");
            return false;
        }
    }

    return true;
}

void mm_render () {
    s._app->disp.clear ();
    s._app->disp.mview_trans (0.0f, -s.player_heady, 0.0f);
    s._app->disp.cam ();
 
    s._app->cell.render ();

    tool_render ();

    if (s.cam_rho > 0.0f)
        player_render ();
}

void mm_stop_snds () {
    s._snd_jump->stop ();
    s._snd_missile->stop ();
    s._snd_missilehit->stop ();
    s._snd_ponger->stop ();
    s._snd_pongerhit->stop ();
    s._snd_pointer->stop ();
    s._snd_selection->stop ();
}

