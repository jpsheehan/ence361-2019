#include <stdint.h>

#include "control.h"
#include "setpoint.h"
#include "altitude.h"
#include "yaw.h"
#include "pwmGen.h"
#include "flightMode.h"

static ControlState g_control_altitude;
static ControlState g_control_yaw;

static bool g_enable_altitude;
static bool g_enable_yaw;

/**
 * Helper function to create a ControlState struct from a ControlGains struct.
 */
ControlState control_get_state_from_gains(ControlGains t_gains)
{
  return (ControlState){
      t_gains.kp,
      t_gains.ki,
      t_gains.kd,
      0};
}

void control_init(ControlGains t_altitude_gains, ControlGains t_yaw_gains)
{
  // initialise the control states of the altitude and yaw
  g_control_altitude = control_get_state_from_gains(t_altitude_gains);
  g_control_yaw = control_get_state_from_gains(t_yaw_gains);
}

void control_update_altitude(void)
{
    if (!g_enable_altitude)
    {
        return;
    }

    // the difference between what we want and what we have (as a percentage)
    int16_t error = setpoint_get_altitude() - alt_getPercent();

    // do control system stuff...

    // call pwm_set_main_duty
}

void control_update_yaw(void)
{
    if (!g_enable_yaw)
    {
        return;
    }

    // the difference between what we want and what we have (in degrees)
    int16_t error = setpoint_get_yaw() - yaw_getDegrees();

    // do control system stuff...

    // pwm_set_tail_duty
}

void control_enable_yaw(bool t_enabled)
{
    g_enable_yaw = t_enabled;
    if (!g_enable_yaw)
    {
        pwm_set_tail_duty(0);
    }
    else
    {
        pwm_set_tail_duty(g_control_yaw.duty);
    }
}

void control_enable_altitude(bool t_enabled)
{
    g_enable_altitude = t_enabled;
    if (!g_enable_altitude)
    {
        pwm_set_main_duty(0);
    }
    else
    {
        pwm_set_main_duty(g_control_yaw.duty);
    }
}
