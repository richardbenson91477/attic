
    enum {
        ITEM_ACT_WIN, ITEM_ACT_DESTROY, ITEM_ACT_SET_STATE, ITEM_ACT_MOVE, 
        ITEM_ACT_PLAYER_MOVE, ITEM_ACT_PLAYER_JUMP, ITEM_ACT_SOUND, 
        ITEM_ACT_ANIM_INIT, ITEM_ACT_ANIM_SEQ, ITEM_ACT_ANIM_PLAY,
        ITEM_ACT_ANIM_PAUSE
    };
 
    struct item_action {
        int32_t action_id;
        char *source_type;
        char *source_s;
        char *target_s;
        snd *_snd;
        int32_t data1, data2;
        float fdata1;
        float v [3];
    };
    class item_state {
    public:
        float len;
        float tick;
        int32_t next;

        blaze actions;

        item_action *action_add (const char *source_type,
            const char *source_s);
        static void action_destroy (void *pv) {
            item_action *_item_action = (item_action *) pv;
            if (! _item_action)
                return;
            if (_item_action->source_s)
                free (_item_action->source_s);
            if (_item_action->source_type)
                free (_item_action->source_type);
            if (_item_action->target_s)
                free (_item_action->target_s);
            if (_item_action->_snd)
                delete _item_action->_snd;
            free (_item_action);
        }
        static void destroy (void *pv) {
            item_state *_item_state = (item_state *) pv;
            if (! _item_state)
                return;
            _item_state->actions.destroy (action_destroy);
            delete _item_state;
        }
    };
    class item {
    public:
        char *type;
        char *name_s;
        char *file_s;
        float move_len;
        float move_tick;
        float move_v [3];
        int32_t move_stalled_;

        n1_obj *_obj;
        n1_phys::ent *_ent;

        blaze states;
        int32_t state_c;
        item_state *_item_state_cur;
    
        static void destroy (void *pv);
    };

    extern bool items_init ();
    extern bool items_tick (float t);
    extern bool item_activate (item *_item, const char *source_type,
        const char *source_name);
    extern item *item_load (char *fname, char *name, float *pos);
    extern bool item_move (item *_item, float t);
    extern item *item_get (char *name, char *fname);
    extern bool item_del (item *_item);
    extern bool item_del (char *name);

