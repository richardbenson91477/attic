
    class conf_t {
    public:
        int32_t win_w, win_h;
        bool full_;
        float fps;
        float move_r;
        float turn_r;
        float cam_col_d;
        float player_weight;
        float mx_r, my_r;
        float jump_r;
        float fire_r;
        const char *player_s;
        const char *map_file_s;
        const char *player_model_file_s;

        void defaults ();
    };

