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

/**
 * True if the yaw has been changed.
 */
static bool g_yaw_changed = false;

/**
 * True if the altitude has been changed.
 */
static bool g_altitude_changed = false;

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
    g_yaw_changed = true;
}

void setpoint_decrement_yaw(void)
{
    // decrement the yaw and then wrap around if necessary
    g_desired_yaw -= YAW_DELTA;
    if (g_desired_yaw < 0)
    {
        g_desired_yaw += 360;
    }
    g_yaw_changed = true;
}

int16_t setpoint_get_yaw(void)
{
    return g_desired_yaw;
}


void setpoint_set_yaw(int16_t t_yaw)
{
    g_desired_yaw = clamp(t_yaw, 0, 359);
    g_yaw_changed = true;
}

void setpoint_increment_altitude(void)
{
    // increment the altitude and then clamp if necessary
    g_desired_altitude = min(g_desired_altitude + ALTITUDE_DELTA, 100);
    g_altitude_changed = true;
}

void setpoint_decrement_altitude(void)
{
    // decrement the altitude and then clamp if necessary
    g_desired_altitude = max(g_desired_altitude - ALTITUDE_DELTA, 0);
    g_altitude_changed = true;
}

int16_t setpoint_get_altitude(void)
{
    return g_desired_altitude;
}

void setpoint_set_altitude(int16_t t_altitude)
{
    g_desired_altitude = clamp(t_altitude, 0, 100);
    g_altitude_changed = true;
}

bool setpoint_get_yaw_changed(void)
{
    return g_yaw_changed;
}

bool setpoint_get_altitude_changed(void)
{
    return g_altitude_changed;
}

void setpoint_reset_yaw_changed(void)
{
    g_yaw_changed = false;
}

void setpoint_reset_altitude_changed(void)
{
    g_altitude_changed = false;
}
