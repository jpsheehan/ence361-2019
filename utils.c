/*
 * utils.c
 *
 *  Created on: 10/04/2019
 *      Author: jps111
 */

#include "utils.h"


int32_t clamp(int32_t value, int32_t min, int32_t max)
{
    if (value < min) {
        return min;
    }

    if (value > max) {
        return max;
    }

    return value;
}

