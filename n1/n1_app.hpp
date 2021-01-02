#ifndef N1APP_INCLUDED

class n1_app {
public:
    n1_disp disp;
    n1_texshader texshader;
    n1_phys phys;
    n1_ptcl ptcl;
    n1_cell cell;
    n1_rgn rgn;
    n1_conf conf;

    win *_win;

    bool init (bool t_);
    void render ();
    bool tick (float t);
};

#define N1APP_INCLUDED
#endif

