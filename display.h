/*******************************************************************************
 *
 * display.h
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
 * This module contains functions for initialising and updating the display.
 *
 ******************************************************************************/

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>

/**
 * Initialises the display module.
 * This must be called before any other functions in the display module.
 */
void disp_init(void);

/**
 * Used to cycle through the valid display states
 * Wraps back to state 0 when all states exhausted
 *
 */
void disp_advanceState(void);

/**
 * Clears the display if g_displayState differs from the previous time disp_render() was called
 * Then renders to the display based on the value of g_displayState
 */
void disp_render(uint32_t t_time_diff_micro);

#endif
