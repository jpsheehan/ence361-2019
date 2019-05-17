/*******************************************************************************
 * 
 * yaw.h
 * 
 * ENEL361 Helicopter Project
 * Friday Morning, Group 7
 * 
 * Written by:
 *  - Manu Hamblyn  <mfb31<@uclive.ac.nz>   95140875
 *  - Will Cowper   <wgc22@uclive.ac.nz>    81163265
 *  - Jesse Sheehan <jps111@uclive.ac.nz>   53366509
 * 
 * Created on: 10/04/2019
 * 
 * Description:
 * This module contains function prototypes required for calculating the slot
 *  count, yaw values, and initialising the quadrature state machine.
 * The states for the quadrature state machine are also defined.
 * 
 ******************************************************************************/

#ifndef YAW_H_
#define YAW_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * Defines the possible sates for the quadrature state machine.
 */
enum quadrature_state { QUAD_STATE_CLOCKWISE, QUAD_STATE_ANTICLOCKWISE, QUAD_STATE_NOCHANGE, QUAD_STATE_INVALID };
typedef enum quadrature_state QuadratureState;

/**
 * Initialises the quadrature module.
 * This must be called before any other functions in the quadrature module.
 */
void yaw_init(void);

/**
 * Returns the current yaw value in degrees.
 * This is a bearing the range 0 - 359 degrees.
 */
uint16_t yaw_get(void);

/**
 * Sets the calibration state to false.
 */
void yaw_reset_calibration_state(void);

/**
 * Returns true if the yaw has been calibrated to its reference.
 */
bool yaw_has_been_calibrated(void);

#endif /* YAW_H_ */
