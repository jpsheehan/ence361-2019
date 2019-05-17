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
#include "flightMode.h"
#include "utils.h"

/**
 * Holds the previous state of the Operating mode (or FLight Status) Finite Sate Machine
 */
//volatile static OperatingMode opMode_previous_state;

/**
 * Holds the current state of the Operating mode (or FLight Status) Finite Sate Machine
 */
volatile static OperatingMode g_mode;

void flightMode_init(void)
{
    g_mode = LANDED;
    //opMode_previous_state = LANDED;
}

OperatingMode flightMode_get_mode(void)
{
    return g_mode;
}

void flightMode_set_current(OperatingMode t_mode)
{
    g_mode = t_mode;
}

void flightMode_set_next(void)
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

void flightMode_update(uint32_t t_time_diff_micro)
{
    if (g_mode == TAKE_OFF)
    {
        if (yaw_hasBeenCalibrated())
        {
            flightMode_set_next();
            control_enable_yaw(true);
            control_enable_altitude(true);
        }
        else
        {
            // find the 0 calibration point for yaw
            pwm_set_main_duty(5);
            pwm_set_tail_duty(50);
        }
    }

    if (g_mode == LANDING)
    {
        uint16_t angle = (yaw_getDegrees() + 180) % 360;
        if (range(angle, 180 - 5, 180 + 5))
        {
            if (alt_get() <= 0)
            {
                control_enable_yaw(false);
                control_enable_altitude(false);
                yaw_resetCalibrationState();
                setpoint_set_yaw(0);
                setpoint_set_altitude(0);
                flightMode_set_next();
            }
            else
            {
                setpoint_set_altitude(0);
            }
        }
        else
        {
            setpoint_set_yaw(0);
            setpoint_set_altitude(5);
        }
    }
}
