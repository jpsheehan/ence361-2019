/*
 * quadrature.c
 *
 *  Created on: 10/04/2019
 *      Author: jps111
 */

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

volatile static uint8_t g_previous_state;
volatile static QuadratureState g_quadrature_state;
volatile static uint16_t g_slot_count;

#define QUAD_MAX_SLOT_COUNT 448 // because there are 112 teeth and 4 phases
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
    GPIOIntDisable(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1);
    GPIOPinTypeGPIOInput (GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1);
    GPIOPadConfigSet (GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1, GPIO_STRENGTH_2MA,
       GPIO_PIN_TYPE_STD_WPD);
    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1, GPIO_BOTH_EDGES);

    GPIOIntRegister(GPIO_PORTB_BASE, quadratureIntHandler);
    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1);
}

void quad_update_state(bool signal_a, bool signal_b)
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

    // update g_previous_raw_quadrature_state to this
    g_previous_state = this_state;
}

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

void quad_intHandler()
{
    // read signal A
    bool signal_a = GPIOPinRead(GPIO_PORTB_BASE, GPIO_INT_PIN_0);

    // read signal B
    bool signal_b = GPIOPinRead(GPIO_PORTB_BASE, GPIO_INT_PIN_1);

    // update the quadrature stuff
    quad_updateState(signal_a, signal_b);

    GPIOIntClear(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1);
}
