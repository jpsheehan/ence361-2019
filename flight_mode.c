/*******************************************************************************
 *
 * flight_mode.c
 *
 * ENEL361 Helicopter Project
 * Friday Morning, Group 7
 *
 * 10/05/2019
 *
 * Written by:
 *  - Manu Hamblyn  <mfb31<@uclive.ac.nz>   95140875
 *  - Will Cowper   <wgc22@uclive.ac.nz>    81163265
 *  - Jesse Sheehan <jps111@uclive.ac.nz>   53366509
 *
 * Description:
 * This module contains implements the Operating Mode (or FLight Status)
 * of the helicopter under control of a Finite Sate Machine
 *
 *********************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "altitude.h"
#include "control.h"
#include "flight_mode.h"
#include "pwm.h"
#include "setpoint.h"
#include "utils.h"
#include "yaw.h"

/**
 * The duty cycle (in percent) to apply to the tail rotor while finding the yaw reference
 */
static const int PWM_TAIL_DUTY_YAW_REF 18

/**
 * The percentage altitude to hover when in landing state,
 * before finding the reference and zero yaw.
 */
static const int HOVER_ALTITUDE = 5;

/**
 * The angle of acceptable error in degrees for detecting zero degrees when landing.
 */
static const int YAW_TOLERANCE = 3;

/**
 * Holds the current state of the Operating mode (or FLight Status) Finite Sate Machine
 */
volatile static FlightModeState g_mode;

void flight_mode_init(void)
{
    g_mode = LANDED;        //Start in the landed state
}

FlightModeState flight_mode_get(void)
{
    return g_mode;
}

void flight_mode_advance_state(void)
{
    switch (g_mode)
    {
    case LANDED:
        g_mode = TAKE_OFF;
        break;
    case TAKE_OFF:
        g_mode = IN_FLIGHT;
        break;
    case IN_FLIGHT:
        g_mode = LANDING;
        break;
    case LANDING:
        g_mode = LANDED;
        break;
    }
}

void flight_mode_update(uint32_t t_time_diff_micro, KernelTask* t_task)
{
    // If state is TAKE_OFF, find yaw reference, advance state,
    //  then enable PID controls
    if (g_mode == TAKE_OFF)
    {
        if (yaw_has_been_calibrated() && alt_has_been_calibrated())
        {
            // Activate Main and Tail PID
            flight_mode_advance_state();
            control_enable_yaw(true);
            control_enable_altitude(true);
        }
        else
        {
            // find the zero calibration point for yaw
            pwm_set_main_duty(0);
            pwm_set_tail_duty(PWM_TAIL_DUTY_YAW_REF);

            if (alt_is_buffer_full())
            {
                alt_calibrate();
            }
        }
    }

    // If state is LANDING, set yaw to zero, altitude to HOVER_ALTITUDE,
    //  once settled set altitude to zero.
    // Once settled at zero altitude, deactivate PID controls, reset
    //  calibration, set yaw and altitude setpoints to zero, advance state
    if (g_mode == LANDING)
    {
        uint16_t angle = (yaw_get() + 180) % 360;       // Get current yaw

        // Is current yaw within tolerance?
        if (range(angle, 180 - YAW_TOLERANCE, 180 + YAW_TOLERANCE))
        {
            // YES, do landing sequence

            // Is altitude at zero?
            if (alt_get() <= 0)
            {
                //YES, have landed
                control_enable_yaw(false);
                control_enable_altitude(false);

                yaw_reset_calibration_state();
                alt_reset_calibration_state();

                setpoint_set_yaw(0);
                setpoint_set_altitude(0);

                flight_mode_advance_state();
            }
            else
            // NO, controlled descent, use settling functions to
            //  get to zero yaw and desired altitude
            {
                // Is yaw and altitude settled for HOVER_ALTITUDE?
                if (alt_is_settled_around(HOVER_ALTITUDE) && yaw_is_settled_around(0))
                {
                    // YES, go to zero altitude
                    setpoint_set_altitude(0);
                }
                else
                {
                    // NO, go to HOVER_ALTITUDE
                    setpoint_set_altitude(HOVER_ALTITUDE);
                }
            }
        }
        // NO, get to zero yaw
        else
        {
            setpoint_set_yaw(0);

            // If yaw has settled, get to HOVER_ALTITUDE
            if (yaw_is_settled_around(0))
            {
                setpoint_set_altitude(HOVER_ALTITUDE);
            }
        }
    }
}
