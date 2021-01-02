
class _state {
public:
    enum {
        TYPE_BALL = n1_phys::TYPE_CELL + 1,
        TYPE_PLAYER
    };
    
    n1_app *_app;
    blaze balls;
    t_conf conf;
    int32_t tex_logo, tex_opts, tex_opts2, tex_pointer, tex_expl, tex_chars;
    snd *_snd_menu, *_snd_goal_eat, *_snd_bounce;
    n1_obj *_obj_trail, *_obj_ball, *_obj_goal;
    float cam_theta, cam_phi, cam_rho, cam_rho_t, cam_col_rho, cam_t = 0.0f;
    bool cam_col;
    bool menu_en, menu_select_;
    int32_t menu_ptr_o = 1;
    n1_rgn::rgn menu_rgns [3], expl_rgn;
    int32_t logo_top, logo_bottom, logo_left, logo_right,  expl_w, expl_x,
        expl_e0, expl_e1, expl_e2, expl_c = 0;
    float expl_t, fade_t = 0.0f, logo_t = 0.0f;
    struct n1_phys::ent *_player_ent, *_player_ent2;
    n1_obj *_player_obj, *_player_obj2;
    float player_mov_theta, player_mov_rho, player_mov_v [3], player_body_theta,
        player_body_target_theta;
    n1_phys::ent *_goal_ent;
    float goal_pos [3], goal_rot [3], goal_eat_t, goal_eat_sv [3], goal_sbrad;
    bool goal_eat;
    ball *goal__ball_eat;
    uint32_t ball_started_n;
    float ball_start_t;
    const float ptcl_s [3] = { 1.5f, 1.5f, 1.5f },
        ptcl_v [3] = { 0.0f, 0.0f, 0.0f },
        ptcl_c [4] = { 0.5f, 0.5f, 0.9f, 0.1f },
        ptcl_w = 0.0f, ptcl_l = 0.7,
        ptcl_s2 [3] = { 1.0f, 1.0f, 1.0f };
};
extern class _state s;

