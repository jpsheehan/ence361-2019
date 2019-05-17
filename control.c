#include <stdint.h>

#include "control.h"
#include "setpoint.h"
#include "altitude.h"
#include "yaw.h"
#include "pwm.h"
#include "flight_mode.h"

ControlState g_control_altitude;
ControlState g_control_yaw;

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

void control_update_altitude(uint32_t t_time_diff_micro)
{
    if (!g_enable_altitude)
    {
        return;
    }

    float Pgain = 0;
    float Igain = 0;
    float Dgain = 0;


    // the difference between what we want and what we have (as a percentage)
    int16_t error = setpoint_get_altitude() - alt_get();

    // P control
    Pgain = error*g_control_altitude.kp;

    // I control
    g_control_altitude.cumulative += error;
    Igain = g_control_altitude.cumulative * g_control_altitude.ki;
    //Igain = 0;

    // D control
    //Dgain = (error - g_control_altitude.lastError) / t_time_diff_micro;
    Dgain = 0;
    g_control_altitude.lastError = error;


    g_control_altitude.duty = Pgain + Igain + Dgain;

    if (g_control_altitude.duty > 100) {
        g_control_altitude.duty = 100;
    }

    pwm_set_main_duty(g_control_altitude.duty);
}

void control_update_yaw(uint32_t t_time_diff_micro)
{
    if (!g_enable_yaw)
    {
        return;
    }

    float Pgain = 0;
    float Igain = 0;
    float Dgain = 0;

    // the difference between what we want and what we have (in degrees)
    int16_t error = setpoint_get_yaw() - yaw_get();

    // P control
    // Pgain = error*g_control_yaw.kp;
    Pgain = g_control_altitude.duty;

    // I control
    Igain = 0;
    g_control_yaw.cumulative += error;

    // D control
    Dgain = 0;
    g_control_yaw.lastError = error;

    g_control_yaw.duty = Pgain + Igain + Dgain;

    if (g_control_yaw.duty > 100) {
         g_control_yaw.duty = 100;
     }

    pwm_set_tail_duty(Pgain + Igain + Dgain);
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
