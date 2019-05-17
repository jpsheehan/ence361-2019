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

#include "display.h"
#include "altitude.h"
#include "utils.h"
#include "uart.h"
#include "yaw.h"
#include "pwm.h"
#include "setpoint.h"
#include "kernel.h"
#include "flightMode.h"
#include "control.h"
#include "input.h"

#define ALTITUDE_YAW_REF 5       //Altitude % to hover at while finding yaw reference
#define PWM_TAIL_DUTY_YAW_REF 50 //Duty cycle % to apply to Tail while finding reference

static const int KERNEL_FREQUENCY = 1024;

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
    disp_render(0);

    while (!alt_get_is_calibrated())
    {
        alt_process_adc(0);

        // check that we have filled the buffer with data
        if (alt_get_is_buffer_full())
        {
            alt_update(0);
            alt_calibrate();
            disp_advanceState();
        }
    }
    //    flightMode_set_next(); //altitude calibrated so move to landed
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
    yaw_init();
    uart_init();
    input_init();
    pwm_init();
    kernel_init(KERNEL_FREQUENCY);
    setpoint_init();
    flightMode_init();
    control_init((ControlGains){1.0f, 1.0f, 1.0f},
                 (ControlGains){1.0f, 1.0f, 1.0f});

    kernel_add_task((KernelTask){&input_update, 0});             // always process input
    kernel_add_task((KernelTask){&alt_process_adc, 256});        // process ADC stuff 256 times per second
    kernel_add_task((KernelTask){&alt_update, 0});               // always update the altitude
    kernel_add_task((KernelTask){&disp_render, 1});              // update the screen once per second
    kernel_add_task((KernelTask){&uart_update, 4});              // update the UART four times per second
    kernel_add_task((KernelTask){&control_update_altitude, 10}); // perform control stuff 10 times per second
    kernel_add_task((KernelTask){&control_update_yaw, 10});
    kernel_add_task((KernelTask){&flightMode_update, 20}); // run state checking 20 times per sec

    //
    // Enable interrupts to the processor.
    IntMasterEnable();

    startup_sequence();

    while (true)
    {
        kernel_run();
    }
}
