
    struct ball {
        struct n1_phys::ent *_ent;
        float ptcl_t;
        snd *_snd_bounce;
    };

extern bool ball_init (bool which);
extern struct ball *ball_add (float *pos);
extern void ball_tick (float t);
extern void ball_destroy (void *_v);
void ball_col_callback (const n1_phys::ecol *col);

