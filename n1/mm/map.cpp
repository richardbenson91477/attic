#include "mm.hpp"
    
bool map_load () {
    char b_in [PATH_MAX];
    char kw [160];
    char ops [5][160];
    float item_pos [3];
    FILE *f_in;

    dlog::debug ("map_load", s.conf.map_file_s);

    f_in = fopen (s.conf.map_file_s, "r");
    if (! f_in)
        return false;

    map_destroy ();

    while (fgets (b_in, sizeof(b_in), f_in)) {
        if (b_in[strlen (b_in) - 1] == '\n')
            b_in[strlen (b_in) - 1] = 0;
        if (! b_in[0])
            continue;
        else if (b_in[0] == ';')
            continue;

        n1_var::var_start (b_in);
        if (! n1_var::var_next (kw))
            continue;

        if (! strcmp (kw, "item")) {
            if (! n1_var::var_next (ops[0]))
                continue;
            if (! n1_var::var_next (ops[1]))
                continue;
            if (! n1_var::var_next (ops[2]))
                continue;
            item_pos[0] = (float) atof (ops[2]);
            if (! n1_var::var_next (ops[2]))
                continue;
            item_pos[1] = (float) atof (ops[2]);
            if (! n1_var::var_next (ops[2]))
                continue;
            item_pos[2] = (float) atof (ops[2]);

            if (! item_load (ops[0], ops[1], item_pos)) {
                dlog::debug ("item_load failed");
                return false;
            }
        }
        else if (! strcmp (kw, "cellmap")) {
            if (! n1_var::var_next (ops[0]))
                continue;

            s._app->cell.load (ops[0]);
        }
        else if (! strcmp (kw, "home")) {
            if (! n1_var::var_next (ops[0]))
                continue;
            if (! n1_var::var_next (ops[1]))
                continue;
            if (! n1_var::var_next (ops[2]))
                continue;

            s.map_home[0] = (float) atof (ops[0]);
            s.map_home[1] = (float) atof (ops[1]);
            s.map_home[2] = (float) atof (ops[2]);
        }
        else if (! strcmp (kw, "music")) {
            if (! n1_var::var_next (ops[0]))
                continue;
            s._map_snd_bg = new snd (ops[0]);
            s._map_snd_bg->play (true);
        }
    }
    fclose (f_in);

    // map init
    s.map_win_= false;
    s.map_win_tick = 0.0f;

    player_reset ();
    cam_init ();
    tool_init ();
    items_init ();
    return true;
}

void map_destroy () {
    dlog::debug ("map_destroy");

    mm_stop_snds ();
    s.items.destroy (item::destroy);
    tool_destroy_all ();
    s._app->ptcl.destroy_all ();
    s.map_win_ = false;
    s.map_win_tick = 0.0f;
    s._app->cell.destroy_all ();
    s._app->cell.destroy_all ();
    delete s._map_snd_bg;
}

uint8_t map_tick () {
    if (s.map_win_) {
        s.map_win_tick += 30.0f;
        if (s.map_win_tick > 2000.0f) {
            s.map_win_tick = 0.0f;
            return s.RES_WIN;
        }
    }

    return 0;
}

