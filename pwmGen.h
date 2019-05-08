/*******************************************************************************
 * 
 * pwmGen.h
 * 
 * ENEL361 Helicopter Project
 * Friday Morning, Group 7
 * 
 * 07 May 2019
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

void initialisePWM (void);




#endif
