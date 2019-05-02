/*******************************************************************************
 * 
 * quadrature.c
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
 * This module contains functionality required for calculating the mean altitude
 * as a raw value and as a percentage of the overall height.
 * Functions are provided to initialise, calibrate, update and return
 * the altitude values.
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

#include "quadrature.h"

/**
 * Holds the previous state of the Quadrature FSM
 */
volatile static uint8_t g_previous_state;

/**
 * Holds the current state of the Quadrature FSM
 */
volatile static QuadratureState g_quadrature_state;

/**
 * Holds the slot count (i.e. number of teeth moved).
 */
volatile static uint16_t g_slot_count;

/**
 * For calculating the yaw in degrees.
 * 112 teeth over 4 phases gives 448
 */
#define QUAD_MAX_SLOT_COUNT 448
#define QUAD_DEGREES_PER_SLOT 360 / QUAD_MAX_SLOT_COUNT

// prototypes
void quad_update_state(bool signal_a, bool signal_b);
void quad_intHandler();
QuadratureState quad_getState();

void quad_init()
{
    g_previous_state = 0b00;
    g_quadrature_state = QUAD_STATE_NOCHANGE;
    
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
    GPIOIntRegister(GPIO_PORTB_BASE, quad_intHandler);
	
	// Enable interrupts on GPIO Port B Pins 0,1 for Yaw channels A and B
	// (clears any outstanding interrupts)
    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1);
}

/**
* Updates the current state of the Quadrature FSM.
*/
void quad_updateState(bool signal_a, bool signal_b)
{
    // compare with previous state
    uint8_t this_state = (signal_a << 1) | signal_b;

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
            if (--g_slot_count > QUAD_MAX_SLOT_COUNT - 1) {
                g_slot_count = QUAD_MAX_SLOT_COUNT - 1;
            }

        } else {
            if (
                    (this_state == 0 && g_previous_state == 2) ||
                    (this_state == 1 && g_previous_state == 0) ||
                    (this_state == 2 && g_previous_state == 3) ||
                    (this_state == 3 && g_previous_state == 1)) {
                g_quadrature_state = QUAD_STATE_CLOCKWISE;
                if (++g_slot_count > QUAD_MAX_SLOT_COUNT - 1) {
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
QuadratureState quad_getState()
{
    QuadratureState temp_state = g_quadrature_state;
    g_quadrature_state = QUAD_STATE_NOCHANGE;
    return temp_state;
}

uint8_t quad_getSlotCount()
{
    return g_slot_count;
}

uint16_t quad_getYawDegrees()
{
    return g_slot_count * QUAD_DEGREES_PER_SLOT;
}

/**
 * The interrupt handler for the for Quadrature interrupt.
 */
void quad_intHandler()
{
    // read signal A
    bool signal_a = GPIOPinRead(GPIO_PORTB_BASE, GPIO_INT_PIN_0);

    // read signal B
    bool signal_b = GPIOPinRead(GPIO_PORTB_BASE, GPIO_INT_PIN_1);

    // update the quadrature stuff
    quad_updateState(signal_a, signal_b);

	// clear the interrup flag
    GPIOIntClear(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1);
}
