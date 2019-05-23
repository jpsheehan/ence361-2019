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

#include "kernel.h"

/**
 * Initialises the altitude module.
 * This must be called before any other functions in the altitude module.
 */
void alt_init(void);

/**
 * Calculates the mean value of the altitude, both the raw resolution and the percentage values. This should be called before one wants to use the altitude values in any other calculations.
 */
void alt_update(uint32_t t_time_diff_micro, KernelTask* t_task);

/**
 * Calibrates the altitude to the current mean value.
 * This must be called before calling `void alt_update(void)`.
 */
void alt_calibrate(void);

/**
 * Returns the mean altitude as a percentage (usually from 0 - 100). This value can be less than 0 or greater than 100.
 */
int16_t alt_get(void);

/**
 * Returns `true` if the altitude has been calibrated.
 */
bool alt_has_been_calibrated(void);

/**
 * Returns `true` if the circular buffer is full and ready to be calculated.
 */
bool alt_is_buffer_full(void);

/**
 * Performs an ADC conversion.
 */
void alt_process_adc(uint32_t t_time_diff_micro, KernelTask* t_task);

/**
 * Resets the calibration state of the altitude.
 */
void alt_reset_calibration_state(void);

/**
 * Returns true if the yaw has "settled".
 */
bool alt_is_settled(void);

/**
 * Kernel Task
 * Updates the settling buffer.
 */
void alt_update_settling(uint32_t t_time_diff_micro, KernelTask* t_task);

/**
 * Returns true if the altitude is settled around a particular value.
 */
bool alt_is_settled_around(int32_t t_value);

/**
 * Returns the value that the altitude has settled around
 */
int32_t alt_get_settled(void);

#endif
