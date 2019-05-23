/*******************************************************************************
 *
 * pwmGen.c
 *
 * ENEL361 Helicopter Project
 * Friday Morning, Group 7
 *
 * 09/05/2019
 *
 * Written by:
 *  - Manu Hamblyn  <mfb31<@uclive.ac.nz>   95140875
 *  - Will Cowper   <wgc22@uclive.ac.nz>    81163265
 *  - Jesse Sheehan <jps111@uclive.ac.nz>   53366509
 *
 * Description:
 * This module contains functions for initialising PWM,
 * changing duty cycle and frequency. Min / Max / Start values are also defined.
 *
 * This module reuses code from pwmGen.c by P.J. Bones as
 * used in Lab3 ENCE361-19S1 =>
 * pwmGen.c - Example code which generates a single PWM
 *    output on J4-05 (M0PWM7) with duty cycle fixed and
 *    the frequency controlled by UP and DOWN buttons in
 *    the range 50 Hz to 400 Hz.
 * 2017: Modified for Tiva and using straightforward, polled
 *    button debouncing implemented in 'buttons4' module.
 *
 * P.J. Bones   UCECE
 * Last modified:  7.2.2018
 *
 **********************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pwm.h"

#include "pwm.h"
#include "utils.h"

/**********************************************************
 * Generates a single PWM signal on Tiva board pin J4-05 =
 * PC5 (M0PWM7).  This is the same PWM output as the
 * helicopter main rotor.
 **********************************************************/

/**********************************************************
 * Constants
 **********************************************************/

// PWM configuration
static const int PWM_RATE = 300;
static const int PWM_DIVIDER_CODE = SYSCTL_PWMDIV_8; //40MHz system clk
static const int PWM_DIVIDER = 8;

//  PWM Hardware Details M0PWM7 (gen 3)
//  ---Main Rotor PWM: PC5, J4-05
static const int PWM_MAIN_BASE = PWM0_BASE;   //PWM module 0
static const int PWM_MAIN_GEN = PWM_GEN_3;    //PWM generator
static const int PWM_MAIN_OUTNUM = PWM_OUT_7; //outputs 7 (& 6) correspond to GEN_3
static const int PWM_MAIN_OUTBIT = PWM_OUT_7_BIT;
static const uint32_t PWM_MAIN_PERIPH_PWM = SYSCTL_PERIPH_PWM0; //PWM module 0
static const uint32_t PWM_MAIN_PERIPH_GPIO = SYSCTL_PERIPH_GPIOC;
static const int PWM_MAIN_GPIO_BASE = GPIO_PORTC_BASE;
static const int PWM_MAIN_GPIO_CONFIG = GPIO_PC5_M0PWM7;
static const int PWM_MAIN_GPIO_PIN = GPIO_PIN_5;

//  PWM Hardware Details M1PWM5 (gen 5)
//  ---Tail Rotor PWM: PF1, J3-10
static const int PWM_TAIL_BASE = PWM1_BASE;   //PWM module 1
static const int PWM_TAIL_GEN = PWM_GEN_2;    //PWM generator
static const int PWM_TAIL_OUTNUM = PWM_OUT_5; //outputs 5 (& 4) correspond to GEN_2
static const int PWM_TAIL_OUTBIT = PWM_OUT_5_BIT;
static const uint32_t PWM_TAIL_PERIPH_PWM = SYSCTL_PERIPH_PWM1; //PWM module 1
static const uint32_t PWM_TAIL_PERIPH_GPIO = SYSCTL_PERIPH_GPIOF;
static const int PWM_TAIL_GPIO_BASE = GPIO_PORTF_BASE;   //GPIO port
static const int PWM_TAIL_GPIO_CONFIG = GPIO_PF1_M1PWM5; //PWM module 1, PWM o/p 5
static const int PWM_TAIL_GPIO_PIN = GPIO_PIN_1;         //GPIO pin

static int8_t g_main_duty;
static int8_t g_tail_duty;

static uint32_t g_pwm_period;

/*********************************************************
 * initialisePWM
 * M0PWM7 (J4-05, PC5) is used for the main rotor motor
 *********************************************************/
void pwm_init(void)
{
    g_pwm_period = SysCtlClockGet() / (PWM_DIVIDER * PWM_RATE);

    SysCtlPWMClockSet(PWM_DIVIDER_CODE);

    // initialise the main rotor
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_GPIO);

    GPIOPinConfigure(PWM_MAIN_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_MAIN_GPIO_BASE, PWM_MAIN_GPIO_PIN);

    PWMGenConfigure(PWM_MAIN_BASE, PWM_MAIN_GEN,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    // Set the initial PWM parameters
    PWMGenPeriodSet(PWM_MAIN_BASE, PWM_MAIN_GEN, g_pwm_period);
    pwm_set_main_duty(0);

    PWMGenEnable(PWM_MAIN_BASE, PWM_MAIN_GEN);

    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, true);

    // initialise the tail rotor
    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_GPIO);

    GPIOPinConfigure(PWM_TAIL_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_TAIL_GPIO_BASE, PWM_TAIL_GPIO_PIN);

    PWMGenConfigure(PWM_TAIL_BASE, PWM_TAIL_GEN,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    // Set the initial PWM parameters
    PWMGenPeriodSet(PWM_TAIL_BASE, PWM_TAIL_GEN, g_pwm_period);
    pwm_set_tail_duty(0);

    PWMGenEnable(PWM_TAIL_BASE, PWM_TAIL_GEN);

    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, true);
}

/********************************************************
 * Function to set the duty cycle of main PWM - M0PWM7
 ********************************************************/
void pwm_set_main_duty(int8_t t_duty)
{
    g_main_duty = clamp(t_duty, 0, 100);
    PWMPulseWidthSet(PWM_MAIN_BASE, PWM_MAIN_OUTNUM,
                     g_pwm_period * g_main_duty / 100);
}

int8_t pwm_get_main_duty(void)
{
    return g_main_duty;
}

/********************************************************
 * Function to set the duty cycle of tail PWM - M1PWM5
 ********************************************************/
void pwm_set_tail_duty(int8_t t_duty)
{
    g_tail_duty = clamp(t_duty, 0, 100);
    PWMPulseWidthSet(PWM_TAIL_BASE, PWM_TAIL_OUTNUM,
                     g_pwm_period * g_tail_duty / 100);
}

int8_t pwm_get_tail_duty(void)
{
    return g_tail_duty;
}
