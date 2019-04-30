/*
 * display.h
 *
 *  Created on: 17/04/2019
 *      Author: wgc22
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

/**
 * Initialises the display module.
 * This must be called before any other functions in the display module.
 */
void disp_init();


void disp_advanceState();

/**
 * Clears the display if g_displayState differs from the previous time disp_render() was called
 * Then renders to the display based on the value of g_displayState
 */
void disp_render();

#endif /* DISPLAY_H_ */
