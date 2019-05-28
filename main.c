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

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "driverlib/interrupt.h"

#include "altitude.h"
#include "clock.h"
#include "control.h"
#include "display.h"
#include "flight_mode.h"
#include "input.h"
#include "kernel.h"
#include "pwm.h"
#include "setpoint.h"
#include "uart.h"
#include "utils.h"
#include "yaw.h"

/**
 * The altitude gains.
 */
static const float ALT_KP = 0.4f;
static const float ALT_KI = 0.009f;
static const float ALT_KD = .8f;

/**
 * The yaw gains.
 */
static const float YAW_KP = 0.8f;
static const float YAW_KI = 0.008f;
static const float YAW_KD = 0.8f;

/**
 * The "frequency" that the kernel runs at in Hz.
 */
static const int KERNEL_FREQUENCY = 1024;

// process ADC stuff 256 times per second
static const uint16_t ALT_ADC_FREQUENCY = 256;
static const uint8_t ALT_ADC_PRIORITY = 1;

// always update the altitude
static const uint16_t ALT_CALC_FREQUENCY = 0;
static const uint8_t ALT_CALC_PRIORITY = 1;

// update the altitude settling 10 times per second
static const uint16_t ALT_SETTLING_FREQUENCY = 10;
static const uint8_t ALT_SETTLING_PRIORITY = 10;

// update the yaw setting 10 times per second
static const uint16_t YAW_SETTLING_FREQUENCY = 10;
static const uint8_t YAW_SETTLING_PRIORITY = 10;

// always process input
static const uint16_t INPUT_FREQUENCY = 0;
static const uint8_t INPUT_PRIORITY = 2;

// perform altitude control stuff 30 times per second
static const uint16_t CONTROL_ALT_FREQUENCY = 30;
static const uint8_t CONTROL_ALT_PRIORITY = 10;

// perform yaw control stuff 30 times per second
static const uint16_t CONTROL_YAW_FREQUENCY = 30;
static const uint8_t CONTROL_YAW_PRIORITY = 10;

// run state checking 20 times per sec
static const uint16_t FLIGHT_MODE_FREQUENCY = 20;
static const uint8_t FLIGHT_MODE_PRIORITY = 10;

// update the screen once per second
static const uint16_t DISPLAY_FREQUENCY = 1;
static const uint8_t DISPLAY_PRIORITY = 100;

// update the UART four times per second
static const uint16_t UART_FREQUENCY = 4;
static const uint8_t UART_PRIORITY = 100;

/**
 * The amount of time to display the splash screen (in seconds)
 */
static const uint32_t SPLASH_SCREEN_WAIT_TIME = 3;

void initialise(void)
{

    // disable all interrupts
    IntMasterDisable();

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
    flight_mode_init();
    control_init((ControlGains){ALT_KP, ALT_KI, ALT_KD},
                 (ControlGains){YAW_KP, YAW_KI, YAW_KD});

    // add tasks to the kernel
    kernel_add_task(&alt_process_adc, ALT_ADC_FREQUENCY, ALT_ADC_PRIORITY);
    kernel_add_task(&alt_update, ALT_CALC_FREQUENCY, ALT_CALC_PRIORITY);
    kernel_add_task(&alt_update_settling, ALT_SETTLING_FREQUENCY, ALT_SETTLING_PRIORITY);
    kernel_add_task(&yaw_update_settling, YAW_SETTLING_FREQUENCY, YAW_SETTLING_PRIORITY);
    kernel_add_task(&input_update, INPUT_FREQUENCY, INPUT_PRIORITY);
    kernel_add_task(&control_update_altitude, CONTROL_ALT_FREQUENCY, CONTROL_ALT_PRIORITY);
    kernel_add_task(&control_update_yaw, CONTROL_YAW_FREQUENCY, CONTROL_YAW_PRIORITY);
    kernel_add_task(&flight_mode_update, FLIGHT_MODE_FREQUENCY, FLIGHT_MODE_PRIORITY);
    kernel_add_task(&disp_render, DISPLAY_FREQUENCY, DISPLAY_PRIORITY);
    kernel_add_task(&uart_update, UART_FREQUENCY, UART_PRIORITY);

    // Enable interrupts to the processor.
    IntMasterEnable();

    // Render splash screen for a couple of seconds
    disp_render(0, NULL);
    utils_wait_for_seconds(SPLASH_SCREEN_WAIT_TIME);
    disp_advance_state();
}

/**
 * The main loop of the program.
 * Initialises the modules and runs the kernel forever.
 */
int main(void)
{
    initialise();

    while (MORE_THAN_HALF_THE_HELI_RIGS_ARE_BROKEN)
    {
        kernel_run();
    }
}
