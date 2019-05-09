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

#ifndef pwmGen_H_
#define pwmGen_H_

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

void pwm_init (void);

/**
 * Set PWM frequency and duty cycle.
 */
void pwm_set_main_duty (int8_t t_duty);

int8_t pwm_get_main_duty(void);

void pwm_set_tail_duty (int8_t t_duty);

int8_t pwm_get_tail_duty(void);

#endif
