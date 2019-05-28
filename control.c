#include <stdint.h>

#include "control.h"
#include "setpoint.h"
#include "altitude.h"
#include "yaw.h"
#include "pwm.h"
#include "flight_mode.h"
#include "utils.h"

// Min speed of main rotor, allows for proper anti-clockwise yaw control (duty cycle %)
static const int MIN_MAIN_DUTY = 20;
// Max speed of main motor to stay within spec (duty cycle %)
static const int MAX_MAIN_DUTY = 70;

// Min speed of tail rotor, prevents wear on motor by idling it instead of completely powering off during large C-CW movements (duty cycle %)
static const int MIN_TAIL_DUTY = 5;
// Max speed of tail motor to stay within spec (duty cycle %)
static const int MAX_TAIL_DUTY = 70;

// Clamps for Kp and Kd gains for each rotor (duty cycle %)
static const int MAIN_GAIN_CLAMP = 10;
static const int TAIL_GAIN_CLAMP = 12;

// Maximum accumulated I error (duty cycle %)
static const int MAXIERROR = 35;

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
        t_gains.kp, // Kp
        t_gains.ki, // Ki
        t_gains.kd, // Kd
        0, // lastError
        0, // cumulative error
        MAXIERROR/t_gains.ki, // Limit cumulative error to maximum of +- 50%
        0, // previous gain that was applied
        0}; // current duty% of motor
}

void control_init(ControlGains t_altitude_gains, ControlGains t_yaw_gains)
{
    // initialise the control states of the altitude and yaw
    g_control_altitude = control_get_state_from_gains(t_altitude_gains);
    g_control_yaw = control_get_state_from_gains(t_yaw_gains);
}

void control_update_altitude(uint32_t t_time_diff_micro, KernelTask* t_task)
{
    if (!g_enable_altitude)
    {
        return;
    }

    // setup temp variables for new gains
    float Pgain = 0;
    float Igain = 0;
    float Dgain = 0;
    int16_t newGain = 0;
    int16_t newDuty = 0;


    // the difference between what we want and what we have (as a percentage)
    int16_t error = setpoint_get_altitude() - alt_get();

    // P control, clamped to 10%
    Pgain = error*g_control_altitude.kp;
    Pgain = clamp(Pgain, -MAIN_GAIN_CLAMP, MAIN_GAIN_CLAMP);

    // I control
    g_control_altitude.cumulative += error;
    // fail-safe the cumulative error by clamping its bounds
    g_control_altitude.cumulative = clamp(g_control_altitude.cumulative, -g_control_altitude.cumulative_max, g_control_altitude.cumulative_max);
    Igain = g_control_altitude.cumulative * g_control_altitude.ki;

    // D control
    Dgain = (error - g_control_altitude.lastError)*g_control_altitude.kd;
    g_control_altitude.lastError = error;
    Dgain = clamp(Dgain, -MAIN_GAIN_CLAMP, MAIN_GAIN_CLAMP);

    // Calculate new motor duty percentage gain
    newGain = Pgain + Igain + Dgain;

    // new motor duty is the current duty plus the difference between old and new gain
    newDuty = g_control_altitude.duty + newGain - g_control_altitude.lastGain;

    // clamp motor to be within spec
    newDuty = clamp(newDuty, MIN_MAIN_DUTY, MAX_MAIN_DUTY);

    // update the duty and the last gain applied
    g_control_altitude.lastGain = newGain;
    g_control_altitude.duty = newDuty;

    pwm_set_main_duty(g_control_altitude.duty);

}

void control_update_yaw(uint32_t t_time_diff_micro, KernelTask* t_task)
{
    if (!g_enable_yaw)
    {
        return;
    }

    float Pgain = 0;
    float Igain = 0;
    float Dgain = 0;
    bool clockWise = true;
    int16_t newGain = 0;
    int16_t newDuty = 0;

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
    Pgain = clamp(Pgain, -TAIL_GAIN_CLAMP, TAIL_GAIN_CLAMP);

    // I control
    g_control_yaw.cumulative += error; // Control is called with fixed frequency so time delta can be ignored.
    // fail-safe the cumulative error by clamping its bounds
    g_control_yaw.cumulative = clamp(g_control_yaw.cumulative, -g_control_yaw.cumulative_max, g_control_yaw.cumulative_max);
    Igain = g_control_yaw.cumulative*g_control_yaw.ki;

    // D control
    Dgain = (error - g_control_yaw.lastError)*g_control_yaw.kd; // Control is called with fixed frequency so time delta can be ignored.
    g_control_yaw.lastError = error;
    Dgain = clamp(Dgain, -TAIL_GAIN_CLAMP, TAIL_GAIN_CLAMP);

    // Calculate new motor duty percentage gain
    newGain = Pgain + Igain + Dgain;

    // new motor duty is the current duty plus the difference between old and new gain
    newDuty = g_control_yaw.duty + newGain - g_control_yaw.lastGain;

    // clamp motor to be within spec
    newDuty = clamp(newDuty, MIN_TAIL_DUTY, MAX_TAIL_DUTY);

    // update the duty and the last gain applied
    g_control_yaw.lastGain = newGain;
    g_control_yaw.duty = newDuty;

    pwm_set_tail_duty(g_control_yaw.duty);

}

void control_enable_yaw(bool t_enabled)
{
    g_enable_yaw = t_enabled;
    if (!g_enable_yaw)
    {
        g_control_yaw.cumulative = 0;
        g_control_yaw.duty = 0;
        g_control_yaw.lastError = 0;
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
        g_control_altitude.cumulative = 0;
        g_control_altitude.duty = 0;
        g_control_altitude.lastError = 0;
        pwm_set_main_duty(0);
    }
    else
    {
        pwm_set_main_duty(g_control_altitude.duty);
    }
}
