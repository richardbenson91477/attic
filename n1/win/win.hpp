#ifndef WIN_INCLUDED
#define WIN_INCLUDED

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>

#ifdef NITRO_ANDROID
    #include <SDL2/SDL_opengles.h>
#else
    #include <SDL2/SDL_opengl.h>
    #define glFrustumf glFrustum
    #define glOrthof glOrtho
#endif

#include "../ndef.hpp"
#include "../dlog/dlog.hpp"
#include "../draw/draw.hpp"

class win {
public:
    int32_t w, h, hw, hh;
    bool full_;
    bool gl_;
    const char *_s;
    uint32_t *_p;
    
    float run_fps, run_frame_t;
    uint32_t run_t_begin, run_t_end, run_frame_t_ms;

    SDL_Window *sdl_win;
    SDL_Surface *sdl_winsurf;
    SDL_GLContext sdl_gl;
    SDL_Event event;
    
    SDL_Scancode keyc;
    bool keys [SDL_NUM_SCANCODES];
    int32_t mx, my;
    float mx_f, my_f;
    bool mb0_, mb1_, mb2_;

    win ();
    ~win ();

    bool init (int32_t w, int32_t h);
    bool tick ();
    void render ();
    void quit ();

    void (* on_key)();
    void (* on_click)();
    void (* on_declick)();
    void (* on_move)();
};

#endif

