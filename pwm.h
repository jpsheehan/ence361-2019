/*******************************************************************************
 * 
 * pwmGen.h
 * 
 * ENEL361 Helicopter Project
 * Friday Morning, Group 7
 * 
 * 09/05/2019
 *
 * Written by:
 *  - Manu Hamblyn  <mfb31<@uclive.ac.nz>   95140875
 *  - Will Cowper   <wgc22@uclive.ac.nz>    81163265
 *  - Jesse Sheehan <jps111@uclive.ac.nz>   53366509
 * 
 * Description:
 * This module contains functions for initialising PWM, changing duty cycle and
 * frequency. Min / Max / Start values are also defined.
 * 
 ******************************************************************************/

#ifndef PWM_H_
#define PWM_H_

#include <stdint.h>

/**
 * Initialises the PWM module. Sets the duty cycles to 0 by default.
 */
void pwm_init(void);

/**
 * Set duty cycle of the main rotor.
 */
void pwm_set_main_duty(int8_t t_duty);

/**
 * Returns the duty cycle of the main rotor.
 */
int8_t pwm_get_main_duty(void);

/**
 * Sets the duty cycle of the tail rotor.
 */
void pwm_set_tail_duty(int8_t t_duty);

/**
 * Returns the duty cycle of the tail rotor.
 */
int8_t pwm_get_tail_duty(void);

#endif
