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
#include "slider.h"
#include "flightMode.h"
#include "control.h"

#define ALTITUDE_YAW_REF 5       //Altitude % to hover at while finding yaw reference
#define PWM_TAIL_DUTY_YAW_REF 65 //Duty cycle % to apply to Tail while finding reference

static const int KERNEL_FREQUENCY = 256;

/**
 * (Original Code by P.J. Bones)
 * Initialisation functions for the clock (incl. SysTick), ADC, display
 */
void clock_init(void)
{
    // Set the clock rate to 40 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
}

void startup_sequence(void)
{
    // Render splash screen while we wait for buffer to fill
    disp_render();

    while (!alt_getIsCalibrated())
    {
        alt_process_adc();

        // check that we have filled the buffer with data
        if (alt_getIsBufferFull())
        {
            alt_update();
            alt_calibrate();
            disp_advanceState();
        }
    }
//    flightMode_set_next(); //altitude calibrated so move to landed
}

//#define flightMode_get_mode() IN_FLIGHT

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
    if (butState == PUSHED)
    {
        if (flightMode_get_mode() == IN_FLIGHT)
        {
            setpoint_decrement_yaw();
        }
    }

    // Check for clockwise rotation button press
    butState = btn_check(RIGHT);
    if (butState == PUSHED)
    {
        if (flightMode_get_mode() == IN_FLIGHT)
        {
            setpoint_increment_yaw();
        }
    }

    // Check for increase altitude button press
    butState = btn_check(UP);
    if (butState == PUSHED)
    {
        if (flightMode_get_mode() == IN_FLIGHT)
        {
            setpoint_increment_altitude();
        }
    }

    // Check for decrease altitude button press
    butState = btn_check(DOWN);
    if (butState == PUSHED)
    {
        if (flightMode_get_mode() == IN_FLIGHT)
        {
            setpoint_decrement_altitude();
        }
    }

    // check sw1
//    slider_update();
//    SliderState sw1_state = slider_check(SLIDER_SW1);
//    bool sw1_changed = slider_changed(SLIDER_SW1);
//
//    if (sw1_state == SLIDER_DOWN)
//    {
//        if (flightMode_get_mode == IN_FLIGHT)
//        {
//            flightMode_set_next(); //were flying, change to landing
//        }
//    }
//    else
//    {
//        if (sw1_state == SLIDER_UP && sw1_changed)
//        {
//            // slider has been changed into the up position
//            if (flightMode_get_mode == LANDED)
//            {
//                flightMode_set_next(); //were landed, change to take off
//            }
//        }

        /*
		if (flightMode_get_current == TAKE_OFF) {
			setpoint_set_altitude(ALTITUDE_YAW_REF);	    //Some altitude to find yaw reference? (counter torque could assist). Could just make zero if not needed.
			pwm_set_tail_duty(PWM_TAIL_DUTY_YAW_REF);       //directly drive the TAIL until we get yaw reference
			                                                //Work out yaw direction in response to Main and assist this.
		    while !( yaw_hasBeenCalibrated() ) {
		        continue;
		    }
            flightMode_set_next();          //were in TAKE_OFF, move to in flight
            setpoint_set_yaw(0);            //hold at zero yaw with PID
            //TO DO
            //start Tail PID                //careful with the order!!
		}

		if (flightMode_get_current == LANDING) {
            setpoint_set_altitude(ALTITUDE_YAW_REF);               //hold some altitude until at yaw reference
            setpoint_set_yaw(0);
            while !( (yaw_getDegrees() >= 358) && (yaw_getDegrees() <= 2) ) { //error margin = +/- min resolution. CHECK!!
                continue;
            }
            setpoint_set_altitude(0);
            flightMode_set_next();                   //were landing, now landed
		}
		*/
//    }

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
        kernel_init(KERNEL_FREQUENCY);
        setpoint_init();
        //	flightMode_init();
        slider_init();
        control_init(
            (ControlGains){1.0f, 1.0f, 1.0f},
            (ControlGains){1.0f, 1.0f, 1.0f});

        kernel_add_task((KernelTask){&process_inputs, 0}); // always process input
        kernel_add_task((KernelTask){&alt_process_adc, 256}); // always process ADC stuff
        kernel_add_task((KernelTask){&alt_update, 0}); // always update the altitude
        kernel_add_task((KernelTask){&disp_render, 1}); // update the screen once per second
        kernel_add_task((KernelTask){&uart_update, 4}); // update the UART four times per second
        kernel_add_task((KernelTask){&control_update_altitude, 10}); // perform control stuff 10 times per second
        kernel_add_task((KernelTask){&control_update_yaw, 10});

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
