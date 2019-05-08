/*******************************************************************************
 * 
 * utils.h
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

#ifndef UTILS_H_
#define UTILS_H_

#include <stdint.h>

#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)

/**
 * "Clamps" the `value` between `min` and `max` inclusive.
 */
int32_t utils_clamp(int32_t t_value, int32_t t_min, int32_t t_max);

/**
 * Waits for `t_delay` seconds before returning. Note that this is
 * approximately correct but will not be exact and should not be used as a 
 * reliable timer.
 */
void utils_waitForSeconds(uint32_t t_delay);

#endif /* UTILS_H_ */
