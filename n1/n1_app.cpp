
#include "n1.hpp"

bool n1_app::init (bool t_) {
    if (t_) {
        srand ((uint32_t) time (NULL));
    
        this->texshader._app = this;
        this->phys._app = this;
        this->cell._app = this;

        if (! snd::init (true))
            return false;

        this->_win = new win ();
        this->_win->gl_ = true;
        this->_win->full_ = this->conf.full_;
        this->_win->_s = this->conf.win_s;
        if (! this->_win->init (this->conf.win_w, this->conf.win_h))
            return false;

        if (! this->phys.init (true))
            return false;

        if (! this->disp.init (true))
            return false;

        if (! this->texshader.init (true))
            return false;

        if (! this->ptcl.init (true))
            return false;
    }
    else {
        dlog::debug ("n1_app::init (false)");

        this->phys.init (false);
        this->cell.destroy_all ();
        this->ptcl.init (false);
        this->texshader.init (false);
        this->disp.init (false);

        snd::init (false);

        delete this->_win;
    }

    return true;
}

void n1_app::render () {
    this->ptcl.render (&this->disp);
    this->disp.render ();
    this->_win->render ();
}

bool n1_app::tick (float t) {
    float v[3];

    this->ptcl.tick (t);

    if (snd::active_) {
        math::v_from_spherical (v, -1.0f,
            this->disp.eye_phi + PI12,
            this->disp.eye_theta);

        snd::orient3d (this->disp.eye_pos, v);
    }

    this->phys.tick (t);
    this->texshader.tick (t);
    this->disp.tick (t);
    
    return this->_win->tick ();
}

