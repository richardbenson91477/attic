#include "win.hpp"

win::win () {
    this->run_fps = 30;
    this->_s = (char *) "win";
    this->full_ = false;
    this->gl_ = false;
        
    this->sdl_win = NULL;
    this->sdl_gl = NULL;

    this->on_key = (void (*)()) NULL;
    this->on_click = (void (*)()) NULL;
    this->on_declick = (void (*)()) NULL;
    this->on_move = (void (*)()) NULL;
}

win::~win () {
    if (this->sdl_win)
        this->quit ();
}

bool win::init (int32_t w, int32_t h) {
    this->w = w;
    this->h = h;
    this->hw = w / 2;
    this->hh = h / 2;

    dlog::debug ("win::init");

    if (this->gl_) {
        SDL_GL_SetAttribute (SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute (SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute (SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute (SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute (SDL_GL_BUFFER_SIZE, 32);
        SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, 16);
    }

    if (SDL_Init (SDL_INIT_VIDEO)) {
        dlog::error ("win::init", "SDL_Init");
        return false;
    }

    this->mx = this->my = 0;
    this->mb0_ = this->mb1_ = this->mb2_ = false;
    this->keyc = SDL_SCANCODE_UNKNOWN;
    memset (this->keys, 0, sizeof(this->keys));

    this->sdl_win = SDL_CreateWindow (_s,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h,
        (this->full_ ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_RESIZABLE) |
        (this->gl_ ? SDL_WINDOW_OPENGL : 0));

    if (! this->sdl_win) {
        dlog::error ("win::init", "SDL_CreateWindow");
        return false;
    }

    this->sdl_winsurf = SDL_GetWindowSurface (this->sdl_win);
    this->_p = (uint32_t *)this->sdl_winsurf->pixels;

    if (this->gl_) {
        this->sdl_gl = SDL_GL_CreateContext (this->sdl_win);
        if (! this->sdl_gl) {
            dlog::error ("win::init", "SDL_GL_CreateContext");
            return false;
        }
    }

    this->run_frame_t = 1.0 / this->run_fps;
    this->run_frame_t_ms = (uint32_t)(this->run_frame_t * 1000.0);
    this->run_t_begin = this->run_t_end = SDL_GetTicks ();
    return true;
}

void win::quit () {
    dlog::debug ("win::quit");

    if (this->gl_) {
        SDL_GL_DeleteContext (this->sdl_gl);
        this->sdl_gl = NULL;
    }

    SDL_DestroyWindow (this->sdl_win);
    SDL_Quit ();
    this->sdl_win = NULL;
}

bool win::tick () {
    while (SDL_PollEvent (&this->event) != 0) {
        if (this->event.type == SDL_QUIT)
            return false;
        else if (this->event.type == SDL_KEYDOWN) {
            if (this->event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                return false;

            this->keyc = event.key.keysym.scancode;
            this->keys[this->keyc] = true;

            if (this->on_key)
                this->on_key ();
        }
        else if (this->event.type == SDL_KEYUP) {
            this->keyc = SDL_SCANCODE_UNKNOWN;
            this->keys[event.key.keysym.scancode] = false;
        } 
        else if (this->event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT)
                this->mb0_ = true;
            else if (event.button.button == SDL_BUTTON_MIDDLE)
                this->mb2_ = true;
            else if (event.button.button == SDL_BUTTON_RIGHT)
                this->mb1_ = true;
            this->mx = event.button.x;
            this->my = event.button.y;
            this->mx_f = ((float)this->mx / (float)this->w) - 0.5;
            this->my_f = ((float)this->my / (float)this->h) - 0.5;
 
            if (this->on_click)
                this->on_click ();
        }
        else if (this->event.type == SDL_MOUSEBUTTONUP) {
            this->mx = event.button.x;
            this->my = event.button.y;
            this->mx_f = ((float)this->mx / (float)this->w) - 0.5;
            this->my_f = ((float)this->my / (float)this->h) - 0.5;

            if (this->on_declick)
                this->on_declick ();

            if (event.button.button == SDL_BUTTON_LEFT)
                this->mb0_ = false;
            else if (event.button.button == SDL_BUTTON_MIDDLE)
                this->mb2_ = false;
            else if (event.button.button == SDL_BUTTON_RIGHT)
                this->mb1_ = false;
        }
        else if (this->event.type == SDL_MOUSEMOTION) {
            this->mx = event.motion.x;
            this->my = event.motion.y;
            this->mx_f = ((float)this->mx / (float)this->w) - 0.5;
            this->my_f = ((float)this->my / (float)this->h) - 0.5;

            if (this->on_move)
                this->on_move ();
        }
    }

    this->run_t_end = SDL_GetTicks ();
    int32_t t_i = run_t_begin - run_t_end + run_frame_t_ms;
    if (t_i > 0)
        SDL_Delay (t_i);
    this->run_t_begin = SDL_GetTicks ();
   
    return true;
}

void win::render () {
    if (! this->sdl_win)
        return;

    if (this->gl_) {
        glFlush ();
        SDL_GL_SwapWindow (this->sdl_win);
    }
    else {
        SDL_UpdateWindowSurface (this->sdl_win);
    }
}

