/*
 * setpoint.c
 *
 *  Created on: 9/05/2019
 *      Author: jps111
 */

#include "utils.h"
#include "setpoint.h"

static int16_t g_desired_yaw = 0;
static int16_t g_desired_altitude = 0;

static const int8_t YAW_DELTA = 15;
static const int8_t ALTITUDE_DELTA = 10;

void setpoint_init(void)
{
    g_desired_yaw = 0;
    g_desired_altitude = 0;
}

void setpoint_increment_yaw(void)
{
    g_desired_yaw = g_desired_yaw + YAW_DELTA;
    if (g_desired_yaw >= 360)
    {
        g_desired_yaw -= 360;
    }
}

void setpoint_decrement_yaw(void)
{
    g_desired_yaw = g_desired_yaw - YAW_DELTA;
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
    g_desired_altitude = min(g_desired_altitude + ALTITUDE_DELTA, 100);
}

void setpoint_decrement_altitude(void)
{
    g_desired_altitude = max(g_desired_altitude - ALTITUDE_DELTA, 0);
}

int16_t setpoint_get_altitude(void)
{
    return g_desired_altitude;
}
