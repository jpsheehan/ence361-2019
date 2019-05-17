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

void utils_wait_for_seconds(uint32_t t_delay)
{
    SysCtlDelay((SysCtlClockGet() * t_delay) / UTILS_SECOND_DELAY_COEFFICIENT);
}
