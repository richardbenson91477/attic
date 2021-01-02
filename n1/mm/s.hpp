class _state {
public:
    enum {
        RES_WIN = 2, RES_QUIT
    };
    enum {
        TYPE_ITEM = n1_phys::TYPE_CELL + 1, TYPE_PLAYER
    };
    enum {
        TEX_CROSSHAIR, TEX_LOGO, TEX_OPTIONS, TEX_OPTIONS2, TEX_BACK, TEX_BACK2
    };

    n1_app *_app = NULL;
    blaze items;
    conf_t conf;
    uint32_t tex_misc [32];
    n1_obj *obj_pointer, *obj_missile, *obj_ponger, *obj_smoke, *obj_flare;
    snd *_snd_jump, *_snd_missile, *_snd_missilehit, *_snd_ponger,
        *_snd_pongerhit, *_snd_pointer, *_snd_selection;
    n1_phys::ent *_player_ent;
    n1_obj *_player_obj;
    n1_obj::joint *_player_joint_head, *_player_joint_waist;
    uint32_t player_walk_seq_c;
    float player_heady, player_theta, player_phi, player_body_theta,
        player_mov_theta;
    item *player_pon_item;
    bool can_jump_ = true;
    float can_jump_t = 0.0f;
    float cam_pos [3], cam_theta, cam_phi, cam_rho, cam_target_rho, cam_col_rho;
    bool cam_col_;
    float map_home [3];
    bool map_win_;
    float map_win_tick;
    snd *_map_snd_bg;
    bool menu_en = true;
    float pointer_pos [3], pointer_tick;
    int32_t pointer_sel, pointer_state;
    bool pointer_chosen;
    n1_rgn::rgn menu_rgns [4];
    n1_texshader::texshader *_sh_back, *_sh_back2;
    blaze tools;
    int32_t tool_cur;
    bool can_use_ = true;
    float can_use_t = 0.0;
};
extern class _state s;

