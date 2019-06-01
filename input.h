/*******************************************************************************
 *
 * input.h
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
 * This module contains logic for initialising and processing inputs from the
 * Orbit Booster Pack slider switches and buttons.
 *
 ******************************************************************************/

#ifndef INPUT_H_
#define INPUT_H_

#include <stdint.h>

#include "kernel.h"

/**
 * Initialises the input module.
 */
void input_init(void);

/**
 * KERNEL TASK
 * Updates the internal input state and may cause changes to the flight mode
 * state machine, setpoint data or PWM, etc...
 */
void input_update(KernelTask* t_task);

#endif /* INPUT_H_ */
