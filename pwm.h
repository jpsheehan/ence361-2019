/*******************************************************************************
 * 
 * pwm.h
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
 * This module contains functions for toggling a GPIO pin when desired. This
 * can be used when the rate of something would like to be measured (e.g. the
 * rate of the display redraw, etc).
 * 
 ******************************************************************************/

#ifndef PWM_H_
#define PWM_H_

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

/**
 * The base of the pin, because we are using this for PB3, we want the B base.
 */
#define PWM_PORT_BASE GPIO_PORTB_BASE

/**
 * The pin number, we are using PB3 so we set it to pin 3.
 */
#define PWM_PORT_PIN GPIO_PIN_3

/**
 * The peripheral we must enable, in this case, the GPIO B peripheral.
 */
#define PWM_PORT_PERIPH SYSCTL_PERIPH_GPIOB

/**
 * Initialises the PWM module. This must be called before `pwm_toggle`.
 */
void pwm_init(void);

/**
 * Toggles the PWM value (if it's high, change it to low, etc).
 */
void pwm_toggle(void);

#endif
