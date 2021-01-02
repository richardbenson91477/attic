    
    enum {
        TOOL_MISSILE = 1, TOOL_PONGER = 2
    };
    struct tool {
        int32_t type;
        float state;
        float pos [3];
        float orig [3];
        float dir [3];
        float theta, phi, rho;
        float speed;
        float tick;
        float fire_t;
        float particle_t;
        snd *_snd_fx1, *_snd_fx2;
    };
    
    extern void tool_init ();
    extern void tool_tick (float t);
    extern void tool_input ();
    extern void tool_render ();
    extern bool tool_use (int32_t type);
    extern void tool_destroy_all ();
    extern void tool_smoke (const float *pos);
    extern void tool_fire (const float *pos, float fire_t);
    extern void tool_explosion (const float *pos);

