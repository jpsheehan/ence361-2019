/*******************************************************************************
 *
 * slider.c
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

#include "slider.h"

// SW1: J1-10 PA7
static const int SW1_PERIPH = SYSCTL_PERIPH_GPIOA;
static const int SW1_PORT_BASE = GPIO_PORTA_BASE;
static const int SW1_PORT_PIN = GPIO_PIN_7;

// SW2: J1-09 PA6
static const int SW2_PERIPH = SYSCTL_PERIPH_GPIOA;
static const int SW2_PORT_BASE = GPIO_PORTA_BASE;
static const int SW2_PORT_PIN = GPIO_PIN_6;

static bool g_previous_state[SLIDER_SWITCH_COUNT];
static bool g_current_state[SLIDER_SWITCH_COUNT];

void slider_init(void)
{
    // initialise SW1
    SysCtlPeripheralEnable(SW1_PERIPH);
    GPIOPinTypeGPIOInput(SW1_PORT_BASE, SW1_PORT_PIN);
    GPIOPadConfigSet(SW1_PORT_BASE, SW1_PORT_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
    g_previous_state[SLIDER_SW1] = SLIDER_DOWN;
    g_current_state[SLIDER_SW1] = SLIDER_DOWN;

    // initialise SW2
    SysCtlPeripheralEnable(SW2_PERIPH);
    GPIOPinTypeGPIOInput(SW2_PORT_BASE, SW2_PORT_PIN);
    GPIOPadConfigSet(SW2_PORT_BASE, SW2_PORT_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
    g_previous_state[SLIDER_SW2] = SLIDER_DOWN;
    g_current_state[SLIDER_SW2] = SLIDER_DOWN;
}

void slider_update(void)
{
    int i;
    for (i = 0; i < SLIDER_SWITCH_COUNT; i++)
    {
        g_previous_state[i] = g_current_state[i];
    }

    g_current_state[SLIDER_SW1] = GPIOPinRead(SW1_PORT_BASE, SW1_PORT_PIN) == SW1_PORT_PIN;
    g_current_state[SLIDER_SW2] = GPIOPinRead(SW2_PORT_BASE, SW2_PORT_PIN) == SW2_PORT_PIN;
}

SliderState slider_check(SliderSwitch t_switch)
{
    return g_current_state[t_switch] ? SLIDER_UP : SLIDER_DOWN;
}

bool slider_changed(SliderSwitch t_switch)
{
    return g_previous_state[t_switch] != g_current_state[t_switch];
}
