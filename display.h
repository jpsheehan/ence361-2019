/*
 * display.h
 *
 *  Created on: 17/04/2019
 *      Author: wgc22
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>

#define DISPLAY_CALIBRATION 0
#define DISPLAY_PERCENT_ADC 1
#define DISPLAY_MEAN_ADC 2
#define DISPLAY_YAW 3
#define DISPLAY_OFF 4

#define DISPLAY_CLAMPED_PERCENTAGE_ALTITUDE false

void displayInit();
void displayStateAdvance();
void displayRender();

#endif /* DISPLAY_H_ */
