#include "Copter.h"

/*
 * Init and run calls for stabilize flight mode
 */

// stabilize_run - runs the main stabilize controller
// should be called at 100hz or more

void ModeBasketball::run()
{
    attitude_control -> set_throttle_out(0.05f, true, g.throttle_filt);
}
