/**********************************************************
 *
 * pwmGen.c - Example code which generates a single PWM
 *    output on J4-05 (M0PWM7) with duty cycle fixed and
 *    the frequency controlled by UP and DOWN buttons in
 *    the range 50 Hz to 400 Hz.
 * 2017: Modified for Tiva and using straightforward, polled
 *    button debouncing implemented in 'buttons4' module.
 *
 * P.J. Bones   UCECE
 * Last modified:  7.2.2018
 **********************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h" //Needed for pin configure
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pwm.h"
#include "pwmGen.h"


/**********************************************************
 * Generates a single PWM signal on Tiva board pin J4-05 =
 * PC5 (M0PWM7).  This is the same PWM output as the
 * helicopter main rotor.
 **********************************************************/

/**********************************************************
 * Constants
 **********************************************************/
// Systick configuration
#define SYSTICK_RATE_HZ    100

// PWM configuration
#define PWM_RATE  200
#define PWM_FIXED_DUTY     50
#define PWM_DIVIDER_CODE   SYSCTL_PWMDIV_8  //40MHz system clk
#define PWM_DIVIDER        8

//  PWM Hardware Details M0PWM7 (gen 3)
//  ---Main Rotor PWM: PC5, J4-05
#define PWM_MAIN_BASE	     PWM0_BASE      //
#define PWM_MAIN_GEN         PWM_GEN_3              //PWM generator
#define PWM_MAIN_OUTNUM      PWM_OUT_7              //outputs 7 (& 6) correspond to GEN_3
#define PWM_MAIN_OUTBIT      PWM_OUT_7_BIT
#define PWM_MAIN_PERIPH_PWM	 SYSCTL_PERIPH_PWM0     //PWM module 0
#define PWM_MAIN_PERIPH_GPIO SYSCTL_PERIPH_GPIOC
#define PWM_MAIN_GPIO_BASE   GPIO_PORTC_BASE
#define PWM_MAIN_GPIO_CONFIG GPIO_PC5_M0PWM7
#define PWM_MAIN_GPIO_PIN    GPIO_PIN_5

//  PWM Hardware Details M1PWM5 (gen 5)
//  ---Tail Rotor PWM: PF1, J3-10
#define PWM_TAIL_BASE        PWM1_BASE
#define PWM_TAIL_GEN         PWM_GEN_2
#define PWM_TAIL_OUTNUM      PWM_OUT_5              //outputs 5 (& 4) correspond to GEN_2
#define PWM_TAIL_OUTBIT      PWM_OUT_7_BIT
#define PWM_TAIL_PERIPH_PWM  SYSCTL_PERIPH_PWM1     //PWM module 1
#define PWM_TAIL_PERIPH_GPIO SYSCTL_PERIPH_GPIOF
#define PWM_TAIL_GPIO_BASE   GPIO_PORTF_BASE        //GPIO port
#define PWM_TAIL_GPIO_CONFIG GPIO_PF1_M1PWM5        //PWM module 1, PWM o/p 5
#define PWM_TAIL_GPIO_PIN    GPIO_PIN_1             //GPIO pin

static int8_t g_main_duty;
static int8_t g_tail_duty;

/*******************************************
 *      Local prototypes
 *******************************************/


/*********************************************************
 * initialisePWM
 * M0PWM7 (J4-05, PC5) is used for the main rotor motor
 *********************************************************/
void
pwm_init (void)
{
    SysCtlPWMClockSet(PWM_DIVIDER_CODE);

    // initialise the main rotor
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_GPIO);

    GPIOPinConfigure(PWM_MAIN_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_MAIN_GPIO_BASE, PWM_MAIN_GPIO_PIN);

    PWMGenConfigure(PWM_MAIN_BASE, PWM_MAIN_GEN,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    // Set the initial PWM parameters
    pwm_set_main_duty (PWM_FIXED_DUTY);

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
    pwm_set_tail_duty (PWM_FIXED_DUTY);

    PWMGenEnable(PWM_TAIL_BASE, PWM_TAIL_GEN);

    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, true);
}

/********************************************************
 * Function to set the freq, duty cycle of M0PWM7
 ********************************************************/
void
pwm_set_main_duty (int8_t t_duty)
{
    g_main_duty = t_duty;

    // Calculate the PWM period corresponding to the freq.
    uint32_t ui32Period =
        SysCtlClockGet() / PWM_DIVIDER / PWM_RATE;

    PWMGenPeriodSet(PWM_MAIN_BASE, PWM_MAIN_GEN, ui32Period);
    PWMPulseWidthSet(PWM_MAIN_BASE, PWM_MAIN_OUTNUM, 
        ui32Period * g_main_duty / 100);
}

int8_t pwm_get_main_duty(void)
{
    return g_main_duty;
}

/********************************************************
 * Function to set the freq, duty cycle of M1PWM5
 ********************************************************/
void
pwm_set_tail_duty (int8_t t_duty)
{
    g_tail_duty = t_duty;

    // Calculate the PWM period corresponding to the freq.
    uint32_t ui32Period =
        SysCtlClockGet() / PWM_DIVIDER / PWM_RATE;

    PWMGenPeriodSet(PWM_TAIL_BASE, PWM_TAIL_GEN, ui32Period);
    PWMPulseWidthSet(PWM_TAIL_BASE, PWM_TAIL_OUTNUM,
                     ui32Period * g_tail_duty / 100);
}


int8_t pwm_get_tail_duty(void)
{
    return g_tail_duty;
}
