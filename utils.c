/*******************************************************************************
 * 
 * utils.c
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
 * This module contains simple functions used in several places throughout this
 * project.
 * 
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/sysctl.h"

#include "utils.h"

/**
 * This macro depends upon the clock speed. For example:
 * - 40 MHz = 6
 * - 20 MHz = 3
 * - etc...
 */
#define UTILS_SECOND_DELAY_COEFFICIENT 6

int32_t utils_clamp(int32_t t_value, int32_t t_min, int32_t t_max)
{
    // if the value is less than the minimum, return the minimum
    if (t_value < t_min)
    {
        return t_min;
    }

    // if the value is greater than the maximum, return the maximum
    if (t_value > t_max)
    {
        return t_max;
    }

    // otherwise, return the value
    return t_value;
}

void utils_waitForSeconds(uint32_t t_delay)
{
    SysCtlDelay((SysCtlClockGet() * t_delay) / UTILS_SECOND_DELAY_COEFFICIENT);
}
