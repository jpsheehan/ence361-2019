/*******************************************************************************
 * 
 * quadrature_enc.h
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

#ifndef QUADRATURE_H_
#define QUADRATURE_H_

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
void quad_init();

/**
 * Returns the number of slots counted due to the current movement.
 * There are 112 slots per full (360 degrees) rotation.
 */
uint8_t quad_getSlotCount();

/**
 * Returns the current yaw value in degrees.
 * This is a bearing the range 0 - 359 degrees.
 */
uint16_t quad_getYawDegrees();

#endif /* QUADRATURE_H_ */
