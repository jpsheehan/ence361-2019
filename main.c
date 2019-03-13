/*
 * main.c
 *
 *  Created on: 13/03/2019
 *      Author: jps111
 */

#include "altitude.h"

void main()
{
    AltitudeSamplingInit();
    AltitudeMeanInit();

    AltitudeCalibrate();

    while (1)
    {
        if (AltitudeGetMeanFlag()) {
            AltitudeCalculateMeans();

            // Clear the flag and continue at the start of the loop
            AltitudeResetMeanFlag();
            continue;
        }
    }
}

