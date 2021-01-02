#include "mm.hpp"

void tool_init () {
    tool_destroy_all ();
    s.tool_cur = TOOL_MISSILE;
}
 
void tool_tick (float t) {
    float v [3];
    n1_phys::ecol col;
    tool *_tool;
    bool res_;

    if (! s.can_use_) {
        s.can_use_t += t;
        if (s.can_use_t >= 0.25f) {
            s.can_use_ = true;
            s.can_use_t = 0.0f;
        }
    }

    _tool = (tool *) s.tools.iter_init ();
    while (_tool) {
        if (_tool->type == TOOL_MISSILE) {
            if (_tool->state == 1) {
                _tool->tick += t;

                _tool->particle_t += 1.0;
                if (_tool->particle_t > 0.0f) {
                    tool_smoke (_tool->pos);
                    _tool->particle_t = 0.0f;
                }

                // out of bound
                if (_tool->tick > 5.0f) {
                    _tool->fire_t = 0.0f;
                    _tool->state = 2;

                    _tool->_snd_fx1->stop ();
                    _tool->_snd_fx2->play (false);
                    _tool->_snd_fx2->pos3d (_tool->pos);
                    _tool = (tool *) s.tools.iterate ();
                    continue;
                }

                // calculate new position
                math::v_scalef2 (v, _tool->dir, _tool->speed);

                s.obj_missile->render (NULL, NULL);
                res_ = s._app->phys.collide_all (_tool->pos,
                    s.obj_missile->brad, v, &col, NULL);
                if (res_ && (col.hit_type != s.TYPE_PLAYER)) {
                    _tool->fire_t = 0.0f;
                    _tool->state = 2;
       
                    _tool->_snd_fx1->stop ();
                    _tool->_snd_fx2->play (false);
                    _tool->_snd_fx2->pos3d (_tool->pos);

                    if (col.hit_type == s.TYPE_ITEM) {
                        item_activate ((item *) col.hit_parent, "tool",
                            "missile");
                    }
                }
                else {
                    math::v_add (_tool->pos, v);
                    // NOTE: too glitchy sounding
                    // snd::buf_pos3d (_tool->snd_fx1, _tool->pos);
                }
            }
            if (_tool->state == 2) {
                if (_tool->fire_t == 0.0f)
                    tool_explosion (_tool->pos);

                _tool->fire_t += s.conf.fire_r;

                _tool->particle_t += 1.0f;
                if (_tool->particle_t > 0.1f) {
                    tool_fire (_tool->pos, _tool->fire_t);
                    _tool->particle_t = 0.0f;
                }
                
                if (_tool->fire_t > 0.005f) {
                    s.tools.del (_tool, 0, blaze::hook_free);
                    _tool = (tool *) s.tools.iter_get ();
                    continue;
                }
            }
        }
        else if (_tool->type == TOOL_PONGER) {
            if (_tool->tick > 5.0f) {
                s.tools.del (_tool, 0, blaze::hook_free);
                _tool = (tool *) s.tools.iter_get ();
                continue;
            }

            _tool->tick += t;

            math::v_scalef2 (v, _tool->dir, _tool->speed);

            res_ = s._app->phys.collide_all (_tool->pos, s.obj_ponger->brad, v,
                &col, NULL);
            if (res_ && (col.hit_type != s.TYPE_PLAYER)) {
                _tool->_snd_fx2->play (false);
                _tool->_snd_fx2->pos3d (_tool->pos);

                if (! math::v_is_clear (col.pn_hit)) {
                    math::v_reflect (v, _tool->dir, col.pn_hit);
                    math::v_copy (_tool->dir, v);
                }
                else
                    math::v_scalef (_tool->dir, -1.0f);

                if (col.hit_type == s.TYPE_ITEM) {
                    item_activate ((item *) col.hit_parent, "tool", "ponger");
                }       
            }
            else {
                math::v_add (_tool->pos, v);
            }
        }
        _tool = (tool *) s.tools.iterate ();
    }
}

void tool_input () {
    if ((s._app->_win->mb0_) || (s._app->_win->keys [SDL_SCANCODE_F])) {
        tool_use (s.tool_cur);
    }

    if (s._app->_win->keys [SDL_SCANCODE_1]) {
        s._app->_win->keys [SDL_SCANCODE_1] = 0;
        s.tool_cur = TOOL_MISSILE;
    }
    else if (s._app->_win->keys [SDL_SCANCODE_2]) {
        s._app->_win->keys [SDL_SCANCODE_2] = 0;
        s.tool_cur = TOOL_PONGER;
    }
}

void tool_render () {
    tool *_tool;

    for (_tool = (tool *) s.tools.iter_init (); _tool;
            _tool = (tool *) s.tools.iterate ()) {
        if (_tool->type == TOOL_MISSILE) {
            if (_tool->state == 1) {
                s._app->disp.mview_push ();
                s._app->disp.mview_trans (_tool->pos);
                s._app->disp.mview_rot (0.0, _tool->theta, 0.0);
                s._app->disp.mview_rot (_tool->phi -PI12, 0.0, 0.0);

                s.obj_missile->render (NULL, NULL);
                s._app->disp.mview_pop();
            }
        }
        else if (_tool->type == TOOL_PONGER) {
            s._app->disp.mview_push();
            s._app->disp.mview_trans (_tool->pos);

            s.obj_ponger->render (NULL, NULL);
            s._app->disp.mview_pop();
        }
    }
}

bool tool_use (int32_t type) {
    tool *_tool;

    if (! s.can_use_)
        return false;

    s.can_use_ = false;
    s.can_use_t = 0.0f;

    _tool = (tool *) malloc (sizeof(tool));
    if (! _tool)
        return false;
    memset (_tool, 0, sizeof(tool));
    _tool->type = type;

    if (type == TOOL_MISSILE) {
        math::v_copy (_tool->pos, s._player_ent->pos);
        math::v_copy (_tool->orig, _tool->pos);

        _tool->speed = 0.2f;
        _tool->phi = s.cam_phi;
        _tool->theta = s.cam_theta;
        _tool->rho = -_tool->speed;

        math::v_from_spherical (_tool->dir, _tool->rho, _tool->phi,
            _tool->theta);
        math::v_divf (_tool->dir, math::v_len (_tool->dir));

        _tool->state = 1;
        _tool->tick = 0;
        _tool->particle_t = 0;

        _tool->_snd_fx1 = s._snd_missile;
        _tool->_snd_fx2 = s._snd_missilehit;
        _tool->_snd_fx1->play (false);
        _tool->_snd_fx1->pos3d (_tool->pos);
    }
    else if (type == TOOL_PONGER) {
        math::v_copy (_tool->pos, s._player_ent->pos);
        math::v_copy (_tool->orig, _tool->pos);

        _tool->speed = 0.5f;
        _tool->phi = s.cam_phi;
        _tool->theta = s.player_theta;
        _tool->rho = -_tool->speed;

        math::v_from_spherical (_tool->dir, _tool->rho, _tool->phi,
            _tool->theta);
        math::v_divf (_tool->dir, math::v_len (_tool->dir));

        _tool->tick = 0;
        _tool->particle_t = 0;

        _tool->_snd_fx1 = s._snd_ponger;
        _tool->_snd_fx2 = s._snd_pongerhit;
        _tool->_snd_fx1->play (false);
        _tool->_snd_fx1->pos3d (_tool->pos);
    }
    else {
        return false;
    }

    s.tools.add (_tool);
    return true;
}

void tool_destroy_all () {
    s.tools.destroy (blaze::hook_free);
}

void tool_smoke (const float *pos) {
    static const float spc [4] = { 0.2f, 0.25f, 0.3f, 0.8f };
    static const float spc2 [4] = { 1.0f, 0.25f, 0.3f, 0.8f };
    static const float sppr [3] = { 0.1f, 0.1f, 0.1f };
    static const float sps [3] = { 0.25f, 0.25f, 0.25f };
    static const float sps2 [3] = { 0.7f, 0.7f, 0.7f };
    static const float sps3 [3] = { 0.5f, 0.5f, 0.5f };
    static const float spv [3] = { 0.0f, 0.03f, 0.0f };
    static const float spv2 [3] = { 0.0f, 0.05f, 0.0f };
    static const float spvr [3] = { 0.0f, PI12, PI2 };
    static const float spl1 = 0.55f;
    static const float spl2 = 0.10f;

    s._app->ptcl.add (1, n1_ptcl::PTCL_OBJECT, spl1, 0, 0, pos, sppr, sps,
        NULL, sps2, spv, spvr, spc, true, s.obj_smoke);

    s._app->ptcl.add (1, n1_ptcl::PTCL_OBJECT, spl2, 0, 0, pos, sppr, sps,
        NULL, sps3, spv2, spvr, spc2, true, s.obj_smoke);
}

void tool_fire (const float *pos, float fire_t) {
    static const float fps [3] = { 0.2f, 0.2f, 0.2f };
    static const float fps2 [3] = { 0.01f, 0.01f, 0.01f };
    static const float fpvr [3] = { PI14, PI, PI };
    static const float fpl = 0.8f;
    float fpc [4] = { 1.0f, 0.0f, 0.0f, 1.0f };
    float fpv [3] = { 0.03f, 0.0f, 0.0f };
    float fpw;

    fpw = math::randf (0.0f, 0.02f);
    fpv[1] = -0.25f + fire_t / 2.0f;
    fpc[0] = math::randf (0.1f, 0.7f);
    fpc[1] = math::randf (0.1f, 0.7f);
    fpc[2] = math::randf (0.1f, 0.7f);

    s._app->ptcl.add (50, n1_ptcl::PTCL_OBJECT, fpl, 0, fpw, pos, NULL, fps,
        fps2, NULL, fpv, fpvr, fpc, true, s.obj_flare);
}

void tool_explosion (const float *pos) {
    static const float exs [3] = { 0.5f, 0.5f, 0.5f };
    static const float exs2 [3] = { 3.0f, 3.0f, 3.0f };
    static const float exc [4] = { 0.0f, 0.0f, 1.0f, 1.0f };
    static const float exl = 0.3f;

    s._app->ptcl.add (1, n1_ptcl::PTCL_OBJECT, exl, 0, 0, pos, NULL, exs, NULL,
        exs2, NULL, NULL, exc, true, s.obj_flare);
}

