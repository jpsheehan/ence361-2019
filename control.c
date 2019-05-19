#include <stdint.h>

#include "control.h"
#include "setpoint.h"
#include "altitude.h"
#include "yaw.h"
#include "pwm.h"
#include "flight_mode.h"
#include "utils.h"

// Min speed of main rotor, allows for proper anti-clockwise yaw control
#define MINMOTORDUTY 15

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
    int32_t newGain = 0;


    // the difference between what we want and what we have (as a percentage)
    int16_t error = setpoint_get_altitude() - alt_get();

    // P control
    Pgain = error*g_control_altitude.kp;

    // I control
    g_control_altitude.cumulative += error;
    Igain = g_control_altitude.cumulative * g_control_altitude.ki;
    //Igain = 0;

    // D control
    Dgain = (error - g_control_altitude.lastError);
    //Dgain = 0;
    g_control_altitude.lastError = error;

    // Calculate new motor duty
    newGain = MINMOTORDUTY + Pgain + Igain + Dgain;
    newGain = clamp(newGain, MINMOTORDUTY, 100);

    g_control_altitude.duty = newGain;

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
    bool clockWise = true;
    int32_t newGain = 0;

    // the difference between what we want and what we have (in degrees)
    int16_t error = (setpoint_get_yaw() - yaw_get());

    // negative error implies set point is behind us (CCW direction)
    if (error < 0) {
        clockWise = false;
        error = abs(error);
    }
    // An error over 180 will always be further than going in the opposite direction
    if (error > 180) {
        error = (360 - error);
        // flip whatever direction we were going in originally
        clockWise = !clockWise;
    }
    // Add duty to rotate CW, subtract duty to rotate CCW
    if (!clockWise) {
        error = -error;
    }

    // P control
     Pgain = error*g_control_yaw.kp;
    //Pgain = g_control_altitude.duty;

    // I control
    g_control_yaw.cumulative += error; // Control is called with fixed frequency so time delta can be ignored.
    Igain = g_control_yaw.cumulative*g_control_yaw.ki;

    // D control
    Dgain = (error - g_control_yaw.lastError); // Control is called with fixed frequency so time delta can be ignored.
    g_control_yaw.lastError = error;

    newGain = g_control_altitude.duty + (Pgain + Igain + Dgain);

    newGain = clamp(newGain, 0, 100);

    g_control_yaw.duty = newGain;

    pwm_set_tail_duty(g_control_yaw.duty);
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
        pwm_set_tail_duty(g_control_altitude.duty);
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
