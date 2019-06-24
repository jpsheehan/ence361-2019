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
 * This module contains prototypes for initialising PWM, changing duty cycle and
 * returning current duty cycle, for Main and Tail rotors.
 * 
 * This module reuses code from pwmGen.c by P.J. Bones as
 * used in Lab3 ENCE361-19S1 =>
 * pwmGen.c - Example code which generates a single PWM
 *    output on J4-05 (M0PWM7) with duty cycle fixed and
 *    the frequency controlled by UP and DOWN buttons in
 *    the range 50 Hz to 400 Hz.
 *
 * P.J. Bones   UCECE
 * Last modified:  7.2.2018
 *
 **********************************************************/

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

/**
 * A macro for incrementing the tail duty by a set amount.
 */
#define pwm_inc_tail_duty(amount) pwm_set_tail_duty(pwm_get_tail_duty() + amount)

/**
 * A macro for decrementing the tail duty by a set amount.
 */
#define pwm_dec_tail_duty(amount) pwm_set_tail_duty(pwm_get_tail_duty() - amount)

/**
 * A macro for incrementing the main duty by a set amount.
 */
#define pwm_inc_main_duty(amount) pwm_set_main_duty(pwm_get_main_duty() + amount)

/**
 * A macro for decrementing the main duty by a set amount.
 */
#define pwm_dec_main_duty(amount) pwm_set_main_duty(pwm_get_main_duty() - amount)

#endif
