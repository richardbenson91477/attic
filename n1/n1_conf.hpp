#ifndef N1_CONF_INCLUDED

class n1_conf {
public:
    int32_t win_w, win_h;
    bool full_;
    char win_s [PATH_MAX];
    float fps;

    void defaults ();
};

#define N1_CONF_INCLUDED
#endif

