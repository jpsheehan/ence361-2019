/*******************************************************************************
 * 
 * yaw.c
 * 
 * ENEL361 Helicopter Project
 * Friday Morning, Group 7
 * 
 * Written by:
 *  - Manu Hamblyn  <mfb31<@uclive.ac.nz>   95140875
 *  - Will Cowper   <wgc22@uclive.ac.nz>    81163265
 *  - Jesse Sheehan <jps111@uclive.ac.nz>   53366509
 * 
 * Created on: 10/04/2019
 * 
 * Description:
 * This module contains function prototypes required for calculating the slot
 *  count, yaw values, and initialising the quadrature state machine.
 * The states for the quadrature state machine are also defined.
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

#include "yaw.h"

/**
 * Defines the possible sates for the quadrature state machine.
 */
enum quadrature_state { QUAD_STATE_CLOCKWISE, QUAD_STATE_ANTICLOCKWISE, QUAD_STATE_NOCHANGE, QUAD_STATE_INVALID };
typedef enum quadrature_state QuadratureState;

/**
 * Holds the previous state of the Quadrature FSM
 */
static uint8_t g_previous_state;

/**
 * Holds the current state of the Quadrature FSM
 */
static QuadratureState g_quadrature_state;

/**
 * Holds the slot count (i.e. number of teeth moved from reference).
 */
volatile static uint16_t g_slot_count;

/**
 * Indicates if the yaw has been calibrated.
 */
static bool g_has_been_calibrated;

/**
 * For calculating the yaw in degrees.
 * 112 teeth over 4 phases gives 448
 */
static const int YAW_MAX_SLOT_COUNT = 448;

// prototypes for functions local to the yaw module
void yaw_update_state(bool t_signal_a, bool t_signal_b);
void yaw_int_handler(void);
void yaw_reference_int_handler(void);
QuadratureState yaw_get_state(void);

void yaw_init(void)
{
    g_previous_state = 0b00;
    g_quadrature_state = QUAD_STATE_NOCHANGE;
    g_has_been_calibrated = false;
    g_slot_count = 0;
    
    // setup the pins (PB0 is A, PB1 is B)
    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOB);
	
	// disable interrupts for safety
    GPIOIntDisable(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1);
	
	// Set the GPIO pins as inputs
    GPIOPinTypeGPIOInput (GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1);
	
	// Set the GPIO pins Weak Pull Down, 2mA
    GPIOPadConfigSet (GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1, GPIO_STRENGTH_2MA,
       GPIO_PIN_TYPE_STD_WPD);
	   
	// Set the GPIO pins to generate interrupts on both rising and falling edges
    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1, GPIO_BOTH_EDGES);

	// Register the interrupt handler
    GPIOIntRegister(GPIO_PORTB_BASE, yaw_int_handler);
	
	// Enable interrupts on GPIO Port B Pins 0,1 for Yaw channels A and B
	// (clears any outstanding interrupts)
    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1);

    // PC4 is the yaw reference, set it up as an input with a falling edge interrupt trigger

    // enable the peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    // disable interrups
    GPIOIntDisable(GPIO_PORTC_BASE, GPIO_INT_PIN_4);

    // set it up as an input
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_4);

    // configure it to be a weak pull down
    GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);

    // configure the interrupt to be falling edge only
    GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE);

    // register the interrupt handler
    GPIOIntRegister(GPIO_PORTC_BASE, yaw_reference_int_handler);

    // enable interrupts on this pin
    GPIOIntEnable(GPIO_PORTC_BASE, GPIO_INT_PIN_4);
}

/**
 * The interrupt handler for the yaw reference.
 */
void yaw_reference_int_handler(void)
{
    if (!g_has_been_calibrated)
    {
        g_has_been_calibrated = true;
        g_slot_count = 0;
    }

    // clear the interrupt flag
    GPIOIntClear(GPIO_PORTC_BASE, GPIO_PIN_4);
}

/**
* Updates the current state of the Quadrature FSM.
* The general thinking is explained in the following document:
* https://cdn.sparkfun.com/datasheets/Robotics/How%20to%20use%20a%20quadrature%20encoder.pdf
*/
void yaw_update_state(bool t_signal_a, bool t_signal_b)
{
    // compare with previous state
    uint8_t this_state = (t_signal_a << 1) | t_signal_b;

    // update g_quadrature_state depending on the previous state
    if (this_state == g_previous_state) {
        g_quadrature_state = QUAD_STATE_NOCHANGE;
    } else {
        if (
                (this_state == 0 && g_previous_state == 1) ||
                (this_state == 1 && g_previous_state == 3) ||
                (this_state == 2 && g_previous_state == 0) ||
                (this_state == 3 && g_previous_state == 2)) {

            g_quadrature_state = QUAD_STATE_ANTICLOCKWISE;
            if (--g_slot_count > YAW_MAX_SLOT_COUNT - 1) {
                g_slot_count = YAW_MAX_SLOT_COUNT - 1;
            }

        } else {
            if (
                    (this_state == 0 && g_previous_state == 2) ||
                    (this_state == 1 && g_previous_state == 0) ||
                    (this_state == 2 && g_previous_state == 3) ||
                    (this_state == 3 && g_previous_state == 1)) {
                g_quadrature_state = QUAD_STATE_CLOCKWISE;
                if (++g_slot_count > YAW_MAX_SLOT_COUNT - 1) {
                    g_slot_count = 0;
                }
            } else {
                g_quadrature_state = QUAD_STATE_INVALID;
            }
        }
    }

    // update g_previous_raw_quadrature_state to this state
    g_previous_state = this_state;
}

/**
* Returns the current state of the Quadrature FSM.
*/
QuadratureState yaw_get_state(void)
{
    QuadratureState temp_state = g_quadrature_state;
    g_quadrature_state = QUAD_STATE_NOCHANGE;
    return temp_state;
}

uint16_t yaw_get(void)
{
    return g_slot_count * 360.0f / YAW_MAX_SLOT_COUNT;
}

/**
 * The interrupt handler for the for Quadrature interrupt.
 */
void yaw_int_handler(void)
{
    // read signal A
    bool signal_a = GPIOPinRead(GPIO_PORTB_BASE, GPIO_INT_PIN_0);

    // read signal B
    bool signal_b = GPIOPinRead(GPIO_PORTB_BASE, GPIO_INT_PIN_1);

    // update the quadrature stuff
    yaw_update_state(signal_a, signal_b);

	// clear the interrupt flag
    GPIOIntClear(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1);
}

void yaw_reset_calibration_state(void)
{
    g_has_been_calibrated = false;
}

bool yaw_has_been_calibrated(void)
{
    return g_has_been_calibrated;
}
