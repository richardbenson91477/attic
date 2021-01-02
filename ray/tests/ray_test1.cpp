#include "../ray.hpp"
#include "../../timer/timer.hpp"
#include "../../dlog/dlog.hpp"
#include "../../win/win.hpp"

const int32_t win_w {600}, win_h {480};
const char *win_s {"ray_test1"};
 
class ray ray1;
const float key_mov_r {5.0};
float frame_t {0.0};
ray::light *_light1;
ray::shape *_shape1, *_shape2, *_shape3, *puser_shape1, *puser_shape2;

bool scene_init ();
bool tick (win *_win);

int main (int argc, char **argv) {
    auto frame_n = 0;
    if (argc > 1)
        frame_n = atoi (argv [1]);

    ray1.w = win_w;
    ray1.h = win_h;
    if (! ray1.calc_rays ())
        return 1;

    scene_init ();

    if (frame_n) {
        dlog::debug ("frame_n: ", frame_n);
        frame_t = 1.0 / 30.0;
        auto frame_p = (uint32_t *) malloc (ray1.w * ray1.h * 4);
        if (! frame_p) {
            dlog::error ("malloc: ", ray1.w * ray1.h * 4);
            return 2;
        }

        timer::tick ();
        for (auto frame_c = 0; frame_c < frame_n; frame_c ++) {
            if (! tick (nullptr))
                break;
            ray1.render (frame_p);
        }
        auto t = timer::tick ();
        dlog::debug ("t = ", t);
        dlog::debug ("fps = ", (float)frame_n / t);
    }
    else {
        auto _win = new win ();
        _win->_s = win_s;
        _win->run_fps = 60.0;
        if (! _win->create (win_w, win_h))
            return 3;
    
        frame_t = _win->run_frame_t;

        draw_init (_win->_p, _win->w, _win->h);
 
        while (_win->tick ()) {
            if (! tick (_win))
                break;
            ray1.render ((uint32_t *) _win->_p);
            _win->render ();
        }
 
        _win->quit ();
        delete _win;
    }

    return 0;
}

bool tick (win *_win) {
    static float t = 0.0f, t2 = 0.0f, t3 = 0.0f;

    if (_win) {
        if (_win->keys [SDL_SCANCODE_ESCAPE])
            return false;

        if (_win->keys [SDL_SCANCODE_H])
            puser_shape2->v [0] -= key_mov_r;
        else if (_win->keys [SDL_SCANCODE_L])
            puser_shape2->v [0] += key_mov_r;

        if (_win->keys [SDL_SCANCODE_K])
            puser_shape2->v [1] -= key_mov_r;
        else if (_win->keys [SDL_SCANCODE_J])
            puser_shape2->v [1] += key_mov_r;

        if (_win->keys [SDL_SCANCODE_U])
            puser_shape2->v [2] -= key_mov_r;
        else if (_win->keys [SDL_SCANCODE_O])
            puser_shape2->v [2] += key_mov_r;

        if (_win->keys [SDL_SCANCODE_W])
            puser_shape1->v [1] -= key_mov_r;
        else if (_win->keys [SDL_SCANCODE_S])
            puser_shape1->v [1] += key_mov_r;

        if (_win->keys [SDL_SCANCODE_A])
            puser_shape1->v [0] -= key_mov_r;
        else if (_win->keys [SDL_SCANCODE_D])
            puser_shape1->v [0] += key_mov_r;

        if (_win->keys [SDL_SCANCODE_Q])
            puser_shape1->v [2] += key_mov_r;
        else if (_win->keys [SDL_SCANCODE_E])
            puser_shape1->v [2] -= key_mov_r;
    }

    t += frame_t;
    if (t > PI2)
        t -= PI2;

    _light1->v [0] = cosf(t) * 200.0f;
    _light1->v [2] = sinf(t) * 200.0f * sinf(t);
    _shape3->v [0] = cosf(t) * 4000.0f;

    t2 += frame_t * 2.0f;
    if (t2 > PI2)
        t2 -= PI2;
    _shape2->v [1] = sinf(t2) * 20.0f;

    t3 += frame_t * 4.0f;
    if (t3 > PI2)
        t3 -= PI2;
    _shape1->v [0] = -100.0f + sinf(t3) * 20.0f;

    return true;
}

bool scene_init () {
    struct ray::shape *_shape;

    ray1.ambient [0] = ray1.ambient [1] = ray1.ambient [2] = 0.35f;

    _shape = new ray::shape { 0, // type
        {0.0, 0.0, -50.0}, // v
        {1.0, 0.3, 0.3}, // diffuse
        {0.3, 1.0, 1.0}, // specular
        37.0, // specular_exp
        0.2, // reflectivity
        35.0, 35.0 * 35.0, // rad, rad_sq
        1 // tex
    };
    ray1.shapes.push_back (_shape);
    puser_shape1 = _shape;

    _shape = new ray::shape { 0, {-100.0, 0.0, -50.0},
        {0.3, 0.3, 0.9}, {0.2, 0.2, 0.2},
        17.0, 0.0, 25.0, 25.0 * 25.0, 0};
    ray1.shapes.push_back (_shape);
    _shape1 = _shape;

    _shape = new ray::shape {0, {100.0, 0.0, -50.0},
        {0.5, 1.0, 0.5}, {0.0, 0.0, 0.0},
        0.0, 0.7, 35.0, 35.0 * 35.0, 0};
    ray1.shapes.push_back (_shape);
    _shape2 = _shape;

    _shape = new ray::shape {0, {0.0, 0.0, 1000.0},
        {1.0, 1.0, 1.0}, {0.0, 0.0, 0.0},
        0.0, 0.2, 500.0, 500.0 * 500.0, 0};
    ray1.shapes.push_back (_shape);
    _shape3 = _shape;

    float sz = 200.0f;
    _shape = new ray::shape {1, {0.0, 0.0, -250.0},
        {1.0, 0.2, 1.0}, {0.0, 0.0, 0.0},
        0.0, 0.5f, 0.0, 0.0, 0,
        {-sz, -sz, 0.0}, {0.0, sz, 0.0}, {sz, -sz, 0.0}};
    ray1.shapes.push_back (_shape);
    puser_shape2 = _shape;
    math::poly_normal (_shape->n, _shape->v1, _shape->v2, _shape->v3);
    _shape->tex = 0;
   
    _light1 = new ray::light { {300.0, 0.0, 100.0}, {0.6, 0.7, 0.8} };
    ray1.lights.push_back (_light1);
    return true;
}

