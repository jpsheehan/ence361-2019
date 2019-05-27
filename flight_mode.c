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

#include "altitude.h"
#include "control.h"
#include "flight_mode.h"
#include "pwm.h"
#include "setpoint.h"
#include "utils.h"
#include "yaw.h"

/**
 * Duty cycle % to apply to Tail while finding reference
 */
static const int PWM_TAIL_DUTY_YAW_REF = 20;

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

void flight_mode_update(uint32_t t_time_diff_micro, KernelTask* t_task)
{
    if (g_mode == TAKE_OFF)
    {
        if (yaw_has_been_calibrated() && alt_has_been_calibrated())
        {
            // if we are in TAKE_OFF mode and both the yaw and altitude have been calibrated,
            // then we enable the PID control systems and advance to IN_FLIGHT mode
            flight_mode_advance_state();
            control_enable_yaw(true);
            control_enable_altitude(true);
        }
        else
        {
            // if we are in TAKE_OFF mode and both of the yaw and altitude have not been calibrated,
            // then we turn off the main rotor and turn on the tail rotor
            pwm_set_main_duty(0);
            pwm_set_tail_duty(PWM_TAIL_DUTY_YAW_REF);

            // the yaw reference will be calibrated via an interrupt

            // we also calibrate the altitude if it is ready
            if (alt_is_buffer_full())
            {
                alt_calibrate();
            }
        }
    }

    if (g_mode == LANDING)
    {
        uint16_t angle = (yaw_get() + 180) % 360;
        if (range(angle, 180 - YAW_TOLERANCE, 180 + YAW_TOLERANCE))
        {
            if (alt_get() <= 0)
            {
                // if the angle is +/- 3 degrees of zero and our altitude is zero or lower,
                // then we reset the entire helicopter state and put it back in LANDED mode
                control_enable_yaw(false);
                control_enable_altitude(false);

                yaw_reset_calibration_state();
                alt_reset_calibration_state();

                setpoint_set_yaw(0);
                setpoint_set_altitude(0);

                flight_mode_advance_state();
            }
            else
            {
                if (alt_is_settled_around(HOVER_ALTITUDE) && yaw_is_settled_around(0))
                {
                    // if the angle is +/- 3 degrees of zero and our altitude is around 5%,
                    // then we set the desired altitude to 0%
                    setpoint_set_altitude(0);
                }
                else
                {
                    // if the angle is +/- 3 degrees of zero and our altitude is not around 5%,
                    // then we set our desired altitude to be 5%
                    setpoint_set_altitude(HOVER_ALTITUDE);
                }
            }
        }
        else
        {
            // if the angle is not +/- 3 degrees of zero
            // then we set our desired angle to be 0 degrees
            setpoint_set_yaw(0);

            if (yaw_is_settled_around(0))
            {
                setpoint_set_altitude(HOVER_ALTITUDE);
            }
        }
    }
}
