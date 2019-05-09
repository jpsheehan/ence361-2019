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
//#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"

#include "button.h"
#include "display.h"
#include "altitude.h"
#include "utils.h"
#include "uart.h"
#include "yaw.h"
#include "pwmGen.h"
#include "setpoint.h"
#include "kernel.h"

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

void startup_sequence(void)
{
    // Render splash screen while we wait for buffer to fill
    disp_render();

    // Wait a 3 seconds
    utils_waitForSeconds(3);

    while (!alt_getIsCalibrated())
    {

        // check that we have filled the buffer with data
        if (alt_getIsBufferFull()) {
            alt_update();
            alt_calibrate();
            disp_advanceState();
        }
    }
}

void process_inputs(void)
{
    // Button state
    butStates_t butState;

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
	yaw_init();
	uart_init();
	pwm_init();
	kernel_init();
	setpoint_init();

	kernel_add_task(&process_inputs, 0);
	kernel_add_task(&alt_update, 0);
	kernel_add_task(&disp_render, 0);
	kernel_add_task(&uart_update, 0);

    //
    // Enable interrupts to the processor.
    IntMasterEnable();

    pwm_set_tail_duty(20);

    startup_sequence();

	while (true)
	{
        kernel_run();
	}
}

