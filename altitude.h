/*
 * altitude.h
 *
 *  Created on: 13/03/2019
 *      Author: jps111
 */

#ifndef ALTITUDE_H_
#define ALTITUDE_H_

#include <stdint.h>
#include <stdbool.h>

#define BUF_SIZE 32
#define SAMPLE_RATE_HZ 256

#define ALTITUDE_DELTA 993

void altitudeInit ();
void updateAltitude();
void calibrate();

int32_t getAltitudePercentage();
uint32_t getAltitudeRaw();
bool getHasBeenCalibrated();

#endif /* ALTITUDE_H_ */
