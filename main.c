//*****************************************************************************
// ENCE361-S1 Milestone 1
// Group 7: Student I.D.s 53366509, 81163265, 95140875
//
// Code incudes various ECE sources including ADC demo, week2 lab,
// circ buffer, buttons
// Last modifed: 22_03_2019 by Group 7
//
// ***************************************************************************
// Original code:
// ADCdemo1.c - Simple interrupt driven program which samples with AIN0
//
// Author:  P.J. Bones	UCECE
// Last modified:	8.2.2018
//*****************************************************************************
// Based on the 'convert' series from 2016
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
// #include "inc/hw_memmap.h"
// #include "inc/hw_types.h"
// #include "driverlib/adc.h"
// #include "driverlib/pwm.h"
// #include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
// #include "driverlib/systick.h"
// #include "driverlib/interrupt.h"
// #include "driverlib/debug.h"

#include "circBufT.h"
#include "buttons4.h"
#include "quadrature.h"
#include "display.h"
#include "altitude.h"
#include "pwm.h"

//*****************************************************************************
// Initialisation functions for the clock (incl. SysTick), ADC, display
//*****************************************************************************
void clock_init (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
}

int main(void)
{
	clock_init();
	alt_init();
	disp_init();
	btn_init();
	pwm_init();
	quad_init();

    //
    // Enable interrupts to the processor.
    IntMasterEnable();

	while (1)
	{
	    butStates_t butState;

	    if (alt_getIsCalibrated()) {

            //
            // Background task: calculate the (approximate) mean of the values in the
            // circular buffer and display it, together with the sample number.

            btn_update();

            // check for reference calibration
            butState = btn_check(LEFT);
            if (butState == PUSHED) {
                alt_calibrate();
            }

            // check for display state change
            butState = btn_check(UP);
            if (butState == PUSHED) {
                disp_advanceState();
            }

            alt_update();

            disp_render();

	    } else {

	        disp_render();

	        // wait a 3 seconds
	        utils_waitForSeconds(3);

	        // check that we have filled the buffer with data
	        if (alt_getIsBufferFull()) {
	            alt_update();
	            alt_calibrate();
	            disp_advanceState();
	        }

	    }

	    pwm_toggle();
	}
}

