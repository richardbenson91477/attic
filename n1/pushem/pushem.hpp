#include "../n1.hpp"

    extern void play ();
    extern void entecol_callback (const struct n1_phys::ecol *col);
    extern bool init (bool which);
    extern bool tick (float t);
    extern void render ();

#include "conf.hpp"
#include "ball.hpp"
#include "menu.hpp"
#include "player.hpp"
#include "goal.hpp"
#include "cam.hpp"
#include "s.hpp"

