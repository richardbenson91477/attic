
#include "snd.hpp"

bool snd::active_ = false;
uint32_t snd::mix_rate = 48000;
uint32_t snd::mix_format = AUDIO_S16;
uint32_t snd::mix_chans = 2;
uint32_t snd::mix_buffers = 4096;
float snd::listener_pos [3];
float snd::listener_rot [3];

snd::snd () {
    this->sdl_buf = NULL;
}

snd::snd (const char *name_s) {
    this->sdl_buf = NULL;
    this->load_raw (NULL, name_s);
}
 
snd::~snd () {
    this->stop ();
    if (this->sdl_buf)
        Mix_FreeChunk (this->sdl_buf);
}

bool snd::init (bool t_) {
    if (t_) {
        dlog::debug ("snd::init (true)");
 
        if (Mix_OpenAudio (snd::mix_rate, snd::mix_format, snd::mix_chans,
                snd::mix_buffers)) {
            dlog::error ("snd::start", "Mix_OpenAudio");
            return false;
        }
        snd::active_ = true;
    }
    else {
        dlog::debug ("snd::init (false)");
        if (! snd::active_)
            return false;

        Mix_CloseAudio ();
        snd::active_ = false;
    }
    return true;
}

void snd::orient3d (const float *pos, const float *rot) {
    snd::listener_pos[0] = pos[0];
    snd::listener_pos[1] = pos[1];
    snd::listener_pos[2] = pos[2];

    snd::listener_rot[0] = rot[0];
    snd::listener_rot[1] = rot[1];
    snd::listener_rot[2] = rot[2];
}

bool snd::load_raw (arc *_arc, const char *name_s) {
    uint8_t *_b;
    uint32_t b_n;
    bool res_;
    file *_file;

    dlog::debug ("n2_snd::load", name_s);

    if (_arc) {
        if (! _arc->get (name_s, &_b, &b_n))
            return false;
    }
    else {
        _file = new file (name_s);
        if (! _file->_b)
            return false;
        _file->free_ = false;
        _b = _file->_b;
        b_n = _file->b_n;
    }

    this->sdl_chan = -1;
    this->sdl_buf = Mix_QuickLoad_RAW (_b, b_n);
 
    if (! _arc)
        delete _file;
    return true;
}

bool snd::play (uint32_t loop_n) {
    if (! snd::active_)
        return false;

    if (! this->sdl_buf)
        return false;

    this->sdl_chan = Mix_PlayChannel (-1, this->sdl_buf, loop_n);
    return true;
}

bool snd::stop () {
    if (! snd::active_)
        return false;

    if (! this->sdl_buf)
        return false;

    if (this->sdl_chan < 0)
        return false;
    
    Mix_HaltChannel (this->sdl_chan);
    this->sdl_chan = -1;
    return true;
}

void snd::pos3d (const float *pos) {
    float listener_rmat [3][4];
    float rel_pos [3], rot_pos [3];
    float d, theta;
    const float d_scale = 10.0f;

    math::v_sub2 (rel_pos, snd::listener_pos, pos);
    d = math::v_len (rel_pos);
 
    math::m_from_r (listener_rmat, snd::listener_rot);
    math::v_inv_rotate_m2 (rot_pos, rel_pos, listener_rmat);
    theta = atan2f (rot_pos[0], rot_pos[2]);
    math::a_bound (&theta);

    d *= d_scale;
    if (d > 255.0f)
        d = 255.0f;

    Mix_SetPosition (this->sdl_chan, (int16_t) DEGREES(theta),
        (int16_t) (d * d_scale));
}

