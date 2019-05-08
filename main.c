/*******************************************************************************
 *
 * main.c
 *
 * ENEL361 Helicopter Project
 * Friday Morning, Group 7
 *
 * Written by:
 *  - Manu Hamblyn  <mfb31<@uclive.ac.nz>   95140875
 *  - Will Cowper   <wgc22@uclive.ac.nz>    81163265
 *  - Jesse Sheehan <jps111@uclive.ac.nz>   53366509
 *
 * This file contains portions of code that written by P.J. Bones. These portions are noted in the comments.
 *
 * Description:
 * This module contains the main entry point for the program. It also contains
 * a function to initialise the system clock.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"

#include "button.h"
#include "display.h"
#include "altitude.h"
#include "pwm.h"
#include "utils.h"
#include "uart.h"
#include "yaw.h"
#include "setpoint.h"

/**
 * (Original Code by P.J. Bones)
 * Initialisation functions for the clock (incl. SysTick), ADC, display
 */
void clock_init (void)
{
    // Set the clock rate to 40 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
}

/**
 * The main loop of the program.
 * Responsible for initialising all the modules, responding to input and rendering text on the screen.
 */
int main(void)
{
    // Setup all required modules
	clock_init();
	alt_init();
	disp_init();
	btn_init();
	pwm_init();
	yaw_init();
	uart_init();
	setpoint_init();

    //
    // Enable interrupts to the processor.
    IntMasterEnable();

	while (true)
	{
	    // Button state
	    butStates_t butState;

	    // Ensure buffer is filled and adc has settled from inital power on.
	    if (alt_getIsCalibrated()) {

            //
            // Background task: calculate the (approximate) mean of the values in the
            // circular buffer and display it, together with the sample number.

            btn_update();

            // Check for counter-clockwise rotation button press
            butState = btn_check(LEFT);
            if (butState == PUSHED) {
                setpoint_decrement_yaw();
            }

            // Check for clockwise rotation button press
            butState = btn_check(RIGHT);
            if (butState == PUSHED) {
                setpoint_increment_yaw();
            }

            // Check for increase altitude button press
            butState = btn_check(UP);
            if (butState == PUSHED) {
                setpoint_increment_altitude();
            }

            // Check for decrease altitude button press
            butState = btn_check(DOWN);
            if (butState == PUSHED) {
                setpoint_decrement_altitude();
            }

            // Calculate altitude from mean average of buffer contents
            alt_update();

            // Render display state to the display
            disp_render();

	    } else {
	        // Render splash screen while we wait for buffer to fill
	        disp_render();

	        // Wait a 3 seconds
	        utils_waitForSeconds(3);

	        // check that we have filled the buffer with data
	        if (alt_getIsBufferFull()) {
	            alt_update();
	            alt_calibrate();
	            disp_advanceState();
	        }

	    }
	    // Used for tracking the speed of the loop.
	    // Toggles PWM on/off each loop
	    pwm_toggle();
	    uart_update();
	}
}

