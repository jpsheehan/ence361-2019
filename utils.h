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
#include <stdbool.h>

/**
 * A macro that expands to an expression that evaluates to the minimum of two values.
 */
#define min(a, b) (a < b ? a : b)

/**
 * A macro that expands to an expression that evaluates to the maximum of two values.
 */
#define max(a, b) (a > b ? a : b)

/**
 * A macro that expands to an expression that evaluates to a value clamped between some
 * minimum and maximum values.
 */
#define clamp(val, mn, mx) (max(min(val, mx), mn))

/**
 * A macro that expands to an expression that evaluates to true if the value is between
 * two other values.
 */
#define range(val, mn, mx) (val >= mn && val <= mx)

/**
 * A macro that expands to an expression that evaluates to true. Nothing more to say
 * about this one...
 */
#define MORE_THAN_HALF_THE_HELI_RIGS_ARE_BROKEN true

/**
 * Waits for `t_delay` seconds before returning. Note that this is
 * approximately correct but will not be exact and should not be used as a 
 * reliable timer.
 */
void utils_wait_for_seconds(uint32_t t_delay);

#endif /* UTILS_H_ */
