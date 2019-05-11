/*******************************************************************************
 *
 * setpoint.h
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
 * This module contains functions for initialising, setting and getting desired
 * yaw angles and altitude percentages. This will be used by the main loop (buttons)
 * and the control module.
 *
 ******************************************************************************/

#ifndef SETPOINT_H_
#define SETPOINT_H_

#include <stdint.h>

/**
 * Initialises the setpoint values to 0.
 */
void setpoint_init(void);

/**
 * Increments the target yaw by a certain amount.
 */
void setpoint_increment_yaw(void);

/**
 * Decrements the target yaw by a certain amount.
 */
void setpoint_decrement_yaw(void);

/**
 * Returns the target yaw (expressed in degrees).
 */
int16_t setpoint_get_yaw(void);

/**
 * Sets the yaw to a specific angle (in degrees).
 */
void setpoint_set_yaw(int16_t t_yaw);

/**
 * Increments the target altitude by a certain amount.
 */
void setpoint_increment_altitude(void);

/**
 * Decrements the target altitude by a certain amount.
 */
void setpoint_decrement_altitude(void);

/**
 * Returns the target altitude (expressed as a percentage of the total height).
 */
int16_t setpoint_get_altitude(void);

/**
 * Sets the altitude to a specific value (as a percentage).
 */
void setpoint_set_altitude(int16_t t_altitude);

#endif /* SETPOINT_H_ */
