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
#define clamp(val, mn, mx) (max(min(val, mx), mn))
#define range(val, mn, mx) (val >= mn && val <= mx)

/**
 * Waits for `t_delay` seconds before returning. Note that this is
 * approximately correct but will not be exact and should not be used as a 
 * reliable timer.
 */
void utils_wait_for_seconds(uint32_t t_delay);

#endif /* UTILS_H_ */
