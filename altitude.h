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

#define ALT_BUF_SIZE 32
#define ALT_DELTA 993

void alt_init();
void alt_update();
void alt_calibrate();

int32_t alt_getPercent();
uint32_t alt_getRaw();
bool alt_getIsCalibrated();

#endif /* ALTITUDE_H_ */
