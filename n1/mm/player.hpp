    
    enum {
        SEQ_IDLE, SEQ_WALK, SEQ_JUMP, SEQ_WALKRIGHT, 
        SEQ_WALKLEFT
    };

    extern bool player_tick (float t);
    extern bool player_init ();
    extern bool player_reset ();
    extern uint8_t player_input (float t);
    extern void player_render ();
    extern void player_move_head ();
    extern void player_jump (float t);
    extern void player_walk (float mov_theta);
    extern void player_collision (const n1_phys::ecol *pcol);

