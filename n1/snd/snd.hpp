#ifndef SND_INCLUDED

#include <cstdint>
extern "C" {
    #include <SDL2/SDL_mixer.h>
}

#include "../dlog/dlog.hpp"
#include "../arc/arc.hpp"
#include "../math/math.hpp"

class snd {
public:
    snd ();
    snd (const char *name_s);
    ~snd ();

    Mix_Chunk *sdl_buf;
    int32_t sdl_chan;

    bool load_raw (arc *_arc, const char *name_s);
    bool play (uint32_t loop_n);
    void pos3d (const float *pos);
    bool stop ();
    
    static bool active_;
    static uint32_t mix_rate;
    static uint32_t mix_chans;
    static uint32_t mix_format;
    static uint32_t mix_buffers;
    static float listener_pos [3];
    static float listener_rot [3];
    
    static bool init (bool t_);
    static void orient3d (const float *pos, const float *rot);
};

#define SND_INCLUDED
#endif

