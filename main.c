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
#include "config.h"
#include "display.h"
#include "flight_mode.h"
#include "input.h"
#include "kernel.h"
#include "pwm.h"
#include "setpoint.h"
#include "uart.h"
#include "utils.h"
#include "yaw.h"

#if !CONFIG_DIRECT_CONTROL

/**
 * The altitude gains.
 */
static const float ALT_KP = 0.65f;
static const float ALT_KI = 0.012f;
static const float ALT_KD = 0.8f;

/**
 * The yaw gains.
 */
static const float YAW_KP = 0.8f;
static const float YAW_KI = 0.009f;
static const float YAW_KD = 0.8f;

#endif

/**
 * The "frequency" that the kernel runs at in Hz.
 */
static const int KERNEL_FREQUENCY = 400000;

// process ADC stuff 512 times per second
static const uint16_t ALT_ADC_FREQUENCY = 512;
static const uint8_t ALT_ADC_PRIORITY = 1;

// update the altitude 512 times a second
static const uint16_t ALT_CALC_FREQUENCY = 512;
static const uint8_t ALT_CALC_PRIORITY = 2;

// update the altitude settling 10 times per second
static const uint16_t ALT_SETTLING_FREQUENCY = 10;
static const uint8_t ALT_SETTLING_PRIORITY = 10;

// update the yaw settling 10 times per second
static const uint16_t YAW_SETTLING_FREQUENCY = 10;
static const uint8_t YAW_SETTLING_PRIORITY = 10;

// always process input
static const uint16_t INPUT_FREQUENCY = 0;
static const uint8_t INPUT_PRIORITY = 50;

#if !CONFIG_DIRECT_CONTROL

// perform altitude control stuff 30 times per second
static const uint16_t CONTROL_ALT_FREQUENCY = 30;
static const uint8_t CONTROL_ALT_PRIORITY = 5;

// perform yaw control stuff 30 times per second
static const uint16_t CONTROL_YAW_FREQUENCY = 30;
static const uint8_t CONTROL_YAW_PRIORITY = 5;

// run state checking 20 times per sec
static const uint16_t FLIGHT_MODE_FREQUENCY = 20;
static const uint8_t FLIGHT_MODE_PRIORITY = 10;

#endif

// update the screen once per second
static const uint16_t DISPLAY_FREQUENCY = 1;
static const uint8_t DISPLAY_PRIORITY = 100;

// send flight data four times per second via UART
static const uint16_t UART_FLIGHT_DATA_FREQUENCY = 4;
static const uint8_t UART_FLIGHT_DATA_PRIORITY = 100;

#if DUMP_KERNEL_DATA
// send kernel timing data once per second via UART
static const uint16_t UART_KERNEL_DATA_FREQUENCY = 1;
static const uint8_t UART_KERNEL_DATA_PRIORITY = 100;
#endif

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

    // we don't use the control systems if we are in direct control
#if !CONFIG_DIRECT_CONTROL
    control_init((ControlGains){ALT_KP, ALT_KI, ALT_KD},
                 (ControlGains){YAW_KP, YAW_KI, YAW_KD});
#endif

    // add tasks to the kernel
    kernel_add_task("altitude_adc", &alt_process_adc, ALT_ADC_FREQUENCY, ALT_ADC_PRIORITY);
    kernel_add_task("altitude_calc", &alt_update, ALT_CALC_FREQUENCY, ALT_CALC_PRIORITY);
    kernel_add_task("altitude_settling", &alt_update_settling, ALT_SETTLING_FREQUENCY, ALT_SETTLING_PRIORITY);
    kernel_add_task("yaw_settling", &yaw_update_settling, YAW_SETTLING_FREQUENCY, YAW_SETTLING_PRIORITY);
    kernel_add_task("input", &input_update, INPUT_FREQUENCY, INPUT_PRIORITY);
#if !CONFIG_DIRECT_CONTROL
    kernel_add_task("altitude_control", &control_update_altitude, CONTROL_ALT_FREQUENCY, CONTROL_ALT_PRIORITY);
    kernel_add_task("yaw_control", &control_update_yaw, CONTROL_YAW_FREQUENCY, CONTROL_YAW_PRIORITY);
    kernel_add_task("flight_mode", &flight_mode_update, FLIGHT_MODE_FREQUENCY, FLIGHT_MODE_PRIORITY);
#endif
    kernel_add_task("display", &disp_render, DISPLAY_FREQUENCY, DISPLAY_PRIORITY);
    kernel_add_task("uart_flight_data", &uart_flight_data_update, UART_FLIGHT_DATA_FREQUENCY, UART_FLIGHT_DATA_PRIORITY);

#if DUMP_KERNEL_DATA
    kernel_add_task("uart_kernel_data", &uart_kernel_data_update, UART_KERNEL_DATA_FREQUENCY, UART_KERNEL_DATA_PRIORITY);
#endif

#if SATURATE_KERNEL
    // add a kernel task to hold up the system with the highest priority and frequency
    kernel_add_task("kernel_saturation", &kernel_saturation_task, 4, 0);
#endif

    // ensure the kernel tasks are in priority order
    kernel_prioritise();

    // Enable interrupts to the processor.
    IntMasterEnable();

    // Render splash screen for a couple of seconds
    disp_render(NULL);
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

    while (MORE_THAN_HALF_THE_HELIRIGS_ARE_BROKEN || WE_STILL_HAVENT_GOTTEN_OUR_MIDTERM_TEST_RESULTS_BACK)
    {
        kernel_run();
    }
}
