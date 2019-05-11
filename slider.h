/*******************************************************************************
 *
 * slider.h
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
 * This module contains functions for initialising, setting and getting the state
 * of the two slider switches on the Orbit Booster Pack.
 *
 ******************************************************************************/

#ifndef SLIDER_H_
#define SLIDER_H_

#include <stdbool.h>

enum slider_state_e { SLIDER_UP, SLIDER_DOWN };

/**
 * Represents the slider states UP and DOWN.
 */
typedef enum slider_state_e SliderState;

enum slider_switch_e { SLIDER_SW1 = 0, SLIDER_SW2, SLIDER_SWITCH_COUNT };

/**
 * Represents the slider switches SW1 and SW2.
 */
typedef enum slider_switch_e SliderSwitch;

/**
 * Initialises the slider module.
 */
void slider_init(void);

/**
 * Updates the slider values.
 */
void slider_update(void);

/**
 * Returns the state of a specific slider switch.
 */
SliderState slider_check(SliderSwitch t_switch);

/**
 * Returns true if the slider switch was just changed.
 */
bool slider_changed(SliderSwitch t_switch);

#endif /* SLIDER_H_ */
