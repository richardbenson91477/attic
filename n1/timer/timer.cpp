
#include "timer.hpp"

float timer_tick () {   
    timeval tv;
    float now_t, _t;
    static time_t init_secs = 0;
    static float before_t;

    gettimeofday (&tv, NULL);

    if (init_secs == 0) {
        init_secs = tv.tv_sec;
        now_t = (float)tv.tv_usec / 1000000.0f +
            (float)(tv.tv_sec - init_secs);
        before_t = now_t;
        return 0.0f;
    }

    now_t = (float)tv.tv_usec / 1000000.0f +
        (float)(tv.tv_sec - init_secs);

    _t = now_t - before_t;
    before_t = now_t;
    return (float)_t;
}

