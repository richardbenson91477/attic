
    class t_conf {
    public:
        int32_t win_w, win_h;
        bool full_;
        float fps;
        const char *win_s;
        float expl_t;
        float light_y;
        float rho_cutoff;
        float cell_reflect;
        float ent_reflect;
        float ent_land_friction;
        float goal_eat_t;
        float goal_init_pos [3];
        int32_t menu_ptr_xs;
        int32_t menu_ptr_ys;
        float menu_fade_t;
        float cam_key_r;
        float cam_key_rho_r;
        float cam_mouse_s;
        float cam_col_dist;
        float cam_phi;
        float cam_rho;
        float player_max_rho;
        float player_mov_r;
        float player_weight;
        float player_anim_r;
        float player_shift_r;
        float ball_max_rho;
        float ball_weight;
        uint32_t balls_start_n;
        float ball_start_t;
        float ball_ptcl_freq;
        float ball_ptcl_at_r;

        void defaults ();
    };

