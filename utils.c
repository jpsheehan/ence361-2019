/*
 * utils.c
 *
 *  Created on: 10/04/2019
 *      Author: jps111
 */

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/sysctl.h"

#include "utils.h"

// depends on the clock speed
#define SECOND_DELAY_COEFFICIENT 6

int32_t utils_clamp(int32_t value, int32_t min, int32_t max)
{
    if (value < min) {
        return min;
    }

    if (value > max) {
        return max;
    }

    return value;
}

/**
 * Does what it says on the tin.
 */
void utils_waitForSeconds(uint32_t delay_s)
{
    SysCtlDelay((SysCtlClockGet() * delay_s) / SECOND_DELAY_COEFFICIENT);
}
