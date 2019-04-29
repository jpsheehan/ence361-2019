/*******************************************************************************
 * 
 * pwm.c
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
 * This module contains functions for toggling a GPIO pin when desired. This
 * can be used when the rate of something would like to be measured (e.g. the
 * rate of the display redraw, etc).
 * 
 ******************************************************************************/

#include "pwm.h"

/**
 * True when the output is high. This is used to keep track of the output.
 */
static bool g_toggled = false;

void pwm_toggle(void)
{
    // toggle the global variable (if it's off turn it on, and vice-versa)
    g_toggled = !g_toggled;

    if (g_toggled)
    {
        // if the global is true, output a high signal
        GPIOPinWrite(PWM_PORT_BASE, PWM_PORT_PIN, PWM_PORT_PIN);
    }
    else
    {
        // otherwise, output a low signal
        GPIOPinWrite(PWM_PORT_BASE, PWM_PORT_PIN, 0x00);
    }
}

void pwm_init(void)
{
    // initialise a port as a peripheral
    SysCtlPeripheralEnable(PWM_PORT_PERIPH);

    // configure the pin as WPD with a strength of 4 mA.
    GPIOPadConfigSet(PWM_PORT_BASE, PWM_PORT_PIN, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPD);

    // configure the pin as an output
    GPIODirModeSet(PWM_PORT_BASE, PWM_PORT_PIN, GPIO_DIR_MODE_OUT);
}
