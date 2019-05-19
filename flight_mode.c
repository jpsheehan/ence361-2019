/*******************************************************************************
 *
 * opStatus.c
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
 * This file contains portions of code that written by P.J. Bones. These portions are noted in the comments.
 *
 * Description:
 * This module controls the Operating mode (or FLight Status) Finite Sate Machine
 *
 *********************************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "yaw.h"
#include "control.h"
#include "setpoint.h"
#include "pwm.h"
#include "altitude.h"
#include "flight_mode.h"
#include "utils.h"

#define PWM_TAIL_DUTY_YAW_REF 25 ///Duty cycle % to apply to Tail while finding reference

/**
 * The percentage altitude to hover on take-off and landing before finding the reference.
 */
static const int HOVER_ALTITUDE = 5;

/**
 * The angle of acceptable error in degrees for detecting reference when landing.
 */
static const int YAW_TOLERANCE = 3;

/**
 * Holds the current state of the Operating mode (or FLight Status) Finite Sate Machine
 */
volatile static FlightModeState g_mode;

void flight_mode_init(void)
{
    g_mode = LANDED;
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

void flight_mode_update(uint32_t t_time_diff_micro)
{
    if (g_mode == TAKE_OFF)
    {
        if (yaw_has_been_calibrated())
        {
            flight_mode_advance_state();
            control_enable_yaw(true);
            control_enable_altitude(true);
        }
        else
        {
            // find the 0 calibration point for yaw
            pwm_set_main_duty(0);
            pwm_set_tail_duty(PWM_TAIL_DUTY_YAW_REF);
        }
    }

    if (g_mode == LANDING)
    {
        uint16_t angle = (yaw_get() + 180) % 360;
        if (range(angle, 180 - YAW_TOLERANCE, 180 + YAW_TOLERANCE))
        {
            if (alt_get() <= 0)
            {
                control_enable_yaw(false);
                control_enable_altitude(false);
                yaw_reset_calibration_state();
                setpoint_set_yaw(0);
                setpoint_set_altitude(0);
                flight_mode_advance_state();
            }
            else
            {
                if (alt_get() <= HOVER_ALTITUDE)
                {
                    setpoint_set_altitude(0);
                }
                else
                {
                    setpoint_set_altitude(HOVER_ALTITUDE);
                }
            }
        }
        else
        {
            setpoint_set_yaw(0);
            setpoint_set_altitude(HOVER_ALTITUDE);
        }
    }
}
