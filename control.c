#include <stdint.h>

#include "control.h"
#include "setpoint.h"
#include "altitude.h"
#include "yaw.h"
#include "pwmGen.h"
#include "flightMode.h"

static ControlState g_control_altitude;
static ControlState g_control_yaw;

/**
 * Helper function to create a ControlState struct from a ControlGains struct.
 */
ControlState control_get_state_from_gains(ControlGains t_gains)
{
  return (ControlState){
      t_gains.kp,
      t_gains.ki,
      t_gains.kd};
}

void control_init(ControlGains t_altitude_gains, ControlGains t_yaw_gains)
{
  // initialise the control states of the altitude and yaw
  g_control_altitude = control_get_state_from_gains(t_altitude_gains);
  g_control_yaw = control_get_state_from_gains(t_yaw_gains);
}

void control_update_altitude(void)
{
  // the difference between what we want and what we have (as a percentage)
  int16_t error = setpoint_get_altitude() - alt_getPercent();

  // do control system stuff...

  // call pwm_set_main_duty
}

void control_update_yaw(void)
{
  // the difference between what we want and what we have (in degrees)
  int16_t error = setpoint_get_yaw() - yaw_getDegrees();

  // do control system stuff...

  // pwm_set_tail_duty
}
