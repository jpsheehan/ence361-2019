/*
 * altitude.h
 *
 *  Created on: 13/03/2019
 *      Author: jps111
 */

#ifndef ALTITUDE_H_
#define ALTITUDE_H_

#include <stdint.h>

void AltitudeSamplingInit();
void AltitudeSamplingIntHandler();

void AltitudeMeanInit();
void AltitudeMeanIntHandler();

void AltitudeCalculateMeans();
void AltitudeCalibrate();

uint8_t AltitudeGetMeanRaw();
uint8_t AltitudeGetMeanPercent();
uint8_t AltitudeGetMeanFlag();
uint8_t AltitudeResetMeanFlag();

#endif /* ALTITUDE_H_ */
