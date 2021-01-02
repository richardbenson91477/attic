#include "pushem.hpp"

bool goal_init () {
    static bool first_ = true;
    n1_phys::ent *_ent;

    if (first_) {
        first_ = false;

        s._obj_goal = new n1_obj (s._app);
        if (! s._obj_goal->load ("goal.mmo"))
            return false;

        s._snd_goal_eat = new snd ("eatfx.raw");

        s.goal_sbrad = s._obj_goal->pre_sbrad;

        s._goal_ent = _ent = s._app->phys.ent_add ();
        _ent->type = n1_phys::TYPE_IGNORE;
        _ent->weight = 0.0f;
        _ent->parent = NULL;
        _ent->_obj = s._obj_goal;
        _ent->rotate_ = true;
    }

    s.goal_pos[0] = s.conf.goal_init_pos[0];
    s.goal_pos[1] = s.conf.goal_init_pos[1];
    s.goal_pos[2] = s.conf.goal_init_pos[2];

    math::v_clear (s.goal_rot);
    math::m_from_r (s._goal_ent->rmat, s.goal_rot);
        
    math::v_copy (s._goal_ent->pos, s.goal_pos);
    s._goal_ent->cell = s._app->cell.classify (s._goal_ent->pos);
    s.goal_eat = false;
    return true;
}

void goal_destroy () {
    delete s._snd_goal_eat;
}

void goal_tick (float t) {
    float pos_rel [3];
    ball *_ball;
    n1_phys::ent *_ent;
    float t2;
    uint32_t iter;

    if (! s.goal_eat) {
        s.goal_rot[1] += t;
        math::a_bound (s.goal_rot + 1);
        math::m_from_r (s._goal_ent->rmat, s.goal_rot);
        
        for (_ball = (ball *) s.balls.iter_init (true, &iter); _ball;
                _ball = (ball *) s.balls.iterate ()) {
            _ent = _ball->_ent;
            if (! _ent)
                continue;
    
            math::v_sub2 (pos_rel, s._goal_ent->pos, _ent->pos);
            if (math::pt_in_sph (pos_rel, s.goal_sbrad)) {
                s.goal_eat = true;
                s.goal_eat_t = 0.0f;
                s.goal__ball_eat = _ball;
                math::v_copy (s.goal_eat_sv, _ball->_ent->pos);
                _ball->_ent->type = n1_phys::TYPE_IGNORE;
                s._snd_goal_eat->play (false);
                break;
            }
        }
        s.balls.iter_init (false, &iter);
    }
    
    if (s.goal_eat) {
        s.goal_eat_t += t;
        if (s.goal_eat_t >= s.conf.goal_eat_t) {
            s.balls.del (s.goal__ball_eat, 0, ball_destroy);
            s.goal_eat = false;
        }
        else {
            // pull ball to goal
            t2 = s.goal_eat_t / s.conf.goal_eat_t;
            _ent = s.goal__ball_eat->_ent;
            _ent->pos[0] = (s.goal_pos[0] - s.goal_eat_sv[0]) * t2 +
                s.goal_eat_sv[0];
            _ent->pos[1] = (s.goal_pos[1] - s.goal_eat_sv[1]) * t2 +
                s.goal_eat_sv[1];
            _ent->pos[2] = (s.goal_pos[2] - s.goal_eat_sv[2]) * t2 +
                s.goal_eat_sv[2];
        }
    }
}

