#!/usr/bin/env python3

import sys, math, os, ctypes
import sdl2

class s:
    ''' global state '''
    win_s = 'nn_draw'
    win_w, win_h = (80, 80)

    run_fps = 25
    # run_t_end, run_t_begin
    # run_frame_t, run_frame_t_ms

    # window, windowsurface
    # mb1, mb3

    img_w, img_h = (8, 8)

    color_on = 0xffffa000
    color_off = 0xff1a1a1a

    # block_w, block_h
    # block_rect, block_rect_ref

    # img

def main ():
    init ()

    paint ()

    event = sdl2.SDL_Event ()
    event_ref = ctypes.byref (event)

    done_ = False
    while not done_:
        while sdl2.SDL_PollEvent (event_ref) != 0:
            if event.type == sdl2.SDL_QUIT:
                done_ = True
                break
            elif event.type == sdl2.SDL_KEYDOWN:
                if event.key.keysym.sym == sdl2.keycode.SDLK_ESCAPE:
                    done_ = True
                    break
            elif event.type == sdl2.SDL_MOUSEBUTTONDOWN:
                on_click (event.button.x, event.button.y, event.button.button)
            elif event.type == sdl2.SDL_MOUSEBUTTONUP:
                on_declick (event.button.x, event.button.y, event.button.button)
            elif event.type == sdl2.SDL_MOUSEMOTION:
                on_move (event.motion.x, event.motion.y)
        tick ()

    deinit ()
    dump ()

def init ():
    sdl2.SDL_Init (sdl2.SDL_INIT_VIDEO)

    s.window = sdl2.SDL_CreateWindow (bytes(s.win_s, 'utf-8'),
        sdl2.SDL_WINDOWPOS_CENTERED, sdl2.SDL_WINDOWPOS_CENTERED,
        s.win_w, s.win_h,
        sdl2.SDL_WINDOW_SHOWN)
    s.windowsurface = sdl2.SDL_GetWindowSurface (s.window)

    s.mb1 = s.mb3 = False

    s.block_w = s.win_w // s.img_w
    s.block_h = s.win_h // s.img_h
    s.block_rect = sdl2.SDL_Rect (0, 0, s.block_w, s.block_h)
    s.block_rect_ref = ctypes.byref (s.block_rect)

    s.img = [False] * s.img_w * s.img_h

    s.run_frame_t = 1.0 / s.run_fps
    s.run_frame_t_ms = int(s.run_frame_t * 1000.0)
    s.run_t_begin = s.run_t_end = sdl2.SDL_GetTicks ()

def deinit ():
    sdl2.SDL_DestroyWindow (s.window)
    sdl2.SDL_Quit ()

def tick ():
    s.run_t_end = sdl2.SDL_GetTicks ()
    _t = s.run_t_begin - s.run_t_end + s.run_frame_t_ms
    if _t > 0:
        sdl2.SDL_Delay (_t)
    s.run_t_begin = sdl2.SDL_GetTicks ()

def paint ():
    rc = s.block_rect
    rc.x = rc.y = 0
    for x in range(0, s.img_w):
        for y in range(0, s.img_h):
            if s.img[x + y * s.img_h]:
                sdl2.SDL_FillRect (s.windowsurface, s.block_rect_ref,
                    s.color_on)
            else:
                sdl2.SDL_FillRect (s.windowsurface, s.block_rect_ref,
                    s.color_off)

            rc.y += s.block_h
        rc.x += s.block_w
        rc.y = 0

    sdl2.SDL_UpdateWindowSurface (s.window)

def on_click (mx, my, mb):
    if mb == 1:
        s.mb1 = True

        x = mx // s.block_w
        y = my // s.block_h
        s.img[x + y * s.img_h] = True

    elif mb == 2:
        s.img = [False] * s.img_w * s.img_h

    elif mb == 3:
        s.mb3 = True
        x = mx // s.block_w
        y = my // s.block_h
        s.img[x + y * s.img_h] = False

    paint ()

def on_declick (mx, my, mb):
    if mb == 1:
        s.mb1 = False
    elif mb == 3:
        s.mb3 = False

def on_move (mx, my):
    if (not s.mb1) and (not s.mb3):
        return

    x = mx // s.block_w
    y = my // s.block_h

    if s.mb1:
        s.img[x + y * s.img_h] = True
    elif s.mb3:
        s.img[x + y * s.img_h] = False

    paint ()

def tostring ():
    line = ""
    for x in range(0, s.img_w * s.img_h):
        if s.img[x]:
            line += "1.0 "
        else:
            line += "0.0 "

    return line

def dump ():
    file = open ("nn_draw.out", "w")
    file.write (tostring());
    file.write ("\n");
    file.close()

if __name__ == '__main__':
    sys.exit (main ())

