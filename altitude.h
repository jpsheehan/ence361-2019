/*******************************************************************************
 * 
 * altitude.h
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
 * This module contains functionality required for calculating the mean altitude
 * as a raw value and as a percentage of the overall height.
 * Functions are provided to initialise, calibrate, update and return
 * the altitude values.
 * 
 ******************************************************************************/

#ifndef ALTITUDE_H_
#define ALTITUDE_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * Initialises the altitude module.
 * This must be called before any other functions in the altitude module.
 */
void alt_init(void);

/**
 * Calculates the mean value of the altitude, both the raw resolution and the percentage values. This should be called before one wants to use the altitude values in any other calculations.
 */
void alt_update(void);

/**
 * Calibrates the altitude to the current mean value.
 * This must be called before calling `void alt_update(void)`.
 */
void alt_calibrate(void);

/**
 * Returns the mean altitude as a percentage (usually from 0 - 100). This value can be less than 0 or greater than 100.
 */
int32_t alt_getPercent(void);

/**
 * Returns the mean altitude. This is a value between 0 and 4095 (inclusive).
 */
uint32_t alt_getRaw(void);

/**
 * Returns `true` if the altitude has been calibrated.
 */
bool alt_getIsCalibrated(void);

/**
 * Returns `true` if the circular buffer is full and ready to be calculated.
 */
bool alt_getIsBufferFull(void);

#endif
