/*******************************************************************************
 *
 * setpoint.c
 *
 * ENEL361 Helicopter Project
 * Friday Morning, Group 7
 *
 * Written by:
 *  - Manu Hamblyn  <mfb31<@uclive.ac.nz>   95140875
 *  - Will Cowper   <wgc22@uclive.ac.nz>    81163265
 *  - Jesse Sheehan <jps111@uclive.ac.nz>   53366509
 *
 * Description:
 * This module contains functions for initialising, setting and getting desired
 * yaw angles and altitude percentages. This will be used by the main loop (buttons)
 * and the control module.
 *
 ******************************************************************************/

#include "utils.h"
#include "setpoint.h"

/**
 * The amount the yaw will change by for each increment/decrement (in degrees).
 */
static const int8_t YAW_DELTA = 15;

/**
 * The amount the altitude will change by for each increment/decrement (as a percentage).
 */
static const int8_t ALTITUDE_DELTA = 10;

/**
 * The target yaw value.
 */
static int16_t g_desired_yaw = 0;

/**
 * The target altitude value.
 */
static int16_t g_desired_altitude = 0;

void setpoint_init(void)
{
    g_desired_yaw = 0;
    g_desired_altitude = 0;
}

void setpoint_increment_yaw(void)
{
    // increment the yaw and then wrap around if necessary
    g_desired_yaw += YAW_DELTA;
    if (g_desired_yaw >= 360)
    {
        g_desired_yaw -= 360;
    }
}

void setpoint_decrement_yaw(void)
{
    // decrement the yaw and then wrap around if necessary
    g_desired_yaw -= YAW_DELTA;
    if (g_desired_yaw < 0)
    {
        g_desired_yaw += 360;
    }
}

int16_t setpoint_get_yaw(void)
{
    return g_desired_yaw;
}

void setpoint_increment_altitude(void)
{
    // increment the altitude and then clamp if necessary
    g_desired_altitude = min(g_desired_altitude + ALTITUDE_DELTA, 100);
}

void setpoint_decrement_altitude(void)
{
    // decrement the altitude and then clamp if necessary
    g_desired_altitude = max(g_desired_altitude - ALTITUDE_DELTA, 0);
}

int16_t setpoint_get_altitude(void)
{
    return g_desired_altitude;
}
