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
 * This module contains functions required for calculating the slot
 *  count, yaw values, and initialising the quadrature state machine.
 * The states for the quadrature state machine are also defined.
 * A settling function is also provided in this module.
 * 
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

#include "circBufT.h"
#include "mutex.h"
#include "utils.h"
#include "yaw.h"

/**
 * Defines the possible sates for the quadrature state machine.
 */
enum quadrature_state { QUAD_STATE_CLOCKWISE, QUAD_STATE_ANTICLOCKWISE, QUAD_STATE_NOCHANGE, QUAD_STATE_INVALID };
typedef enum quadrature_state QuadratureState;

/**
 * For calculating the yaw in degrees.
 * 112 teeth over 4 phases gives 448
 */
static const int YAW_MAX_SLOT_COUNT = 448;

/**
 * The size of the settling buffer.
 */
static const int YAW_SETTLING_BUF_SIZE = 10;

/**
 * The maximum difference between the minimum and maximum values (in degrees)
 * of the settling buffer for the yaw_is_settled() to return true.
 */
static const int YAW_SETTLING_MARGIN = 2;

/**
 * Holds the previous state of the Quadrature FSM
 */
static uint8_t g_previous_state;

/**
 * Holds the current state of the Quadrature FSM
 */
static volatile QuadratureState g_quadrature_state;

/**
 * The mutex for the current quadrature state.
 */
static Mutex g_quadrature_state_mutex;

/**
 * Holds the slot count (i.e. number of teeth moved from reference).
 */
static volatile uint16_t g_slot_count;

/**
 * The mutex for the slot count.
 */
static Mutex g_slot_count_mutex;

/**
 * Indicates if the yaw has been calibrated.
 */
static volatile bool g_has_been_calibrated;

/**
 * The mutex for the has been calibrated variable.
 */
static Mutex g_has_been_calibrated_mutex;

/**
 * The buffer that holds the degree values for settling calculations.
 */
static circBuf_t g_settling_buffer;

/**
 * Yaw Quadrature Encoding:
 * PB0 is the Phase A pin
 * PB1 is the Phase B pin
 */
static const uint32_t YAW_QUAD_PERIPH = SYSCTL_PERIPH_GPIOB;
static const int YAW_QUAD_BASE = GPIO_PORTB_BASE;
static const int YAW_QUAD_INT_PIN_1 = GPIO_INT_PIN_0;
static const int YAW_QUAD_INT_PIN_2 = GPIO_INT_PIN_1;
static const int YAW_QUAD_PIN_1 = GPIO_PIN_0;
static const int YAW_QUAD_PIN_2 = GPIO_PIN_1;
static const int YAW_QUAD_SIG_STRENGTH = GPIO_STRENGTH_4MA;
static const int YAW_QUAD_PIN_TYPE = GPIO_PIN_TYPE_STD_WPD;
static const int YAW_QUAD_EDGE_TYPE = GPIO_BOTH_EDGES;
static const int YAW_QUAD_DDR = GPIO_DIR_MODE_IN;

/**
 * Yaw Reference:
 * PC4 is the reference pin
 */
static const uint32_t YAW_REF_PERIPH = SYSCTL_PERIPH_GPIOC;
static const int YAW_REF_BASE = GPIO_PORTC_BASE;
static const int YAW_REF_INT_PIN = GPIO_INT_PIN_4;
static const int YAW_REF_PIN = GPIO_PIN_4;
static const int YAW_REF_SIG_STRENGTH = GPIO_STRENGTH_2MA;

// specification conflict: pull up and rising edge are reliable
static const int YAW_REF_PIN_TYPE = GPIO_PIN_TYPE_STD_WPU;
static const int YAW_REF_EDGE_TYPE = GPIO_RISING_EDGE;

// prototypes for functions local to the yaw module
void yaw_update_state(bool t_signal_a, bool t_signal_b);
void yaw_int_handler(void);
void yaw_reference_int_handler(void);
QuadratureState yaw_get_state(void);

/**
 * Initialise yaw including:
 * state machine (previous and current states), calibration,
 * slot count, circular buffer for settling,
 * set up input pins, interrupts etc.
 */
void yaw_init(void)
{
    g_previous_state = 0b00;
    g_quadrature_state = QUAD_STATE_NOCHANGE;
    g_has_been_calibrated = false;
    g_slot_count = 0;
    initCircBuf(&g_settling_buffer, YAW_SETTLING_BUF_SIZE);
    
    // setup the pins (PB0 is A, PB1 is B)
    SysCtlPeripheralEnable(YAW_QUAD_PERIPH);

    // disable interrupts for safety
    GPIOIntDisable(YAW_QUAD_BASE, YAW_QUAD_INT_PIN_1 | YAW_QUAD_INT_PIN_2);

    // Set the GPIO pins as inputs
    GPIOPinTypeGPIOInput(YAW_QUAD_BASE, YAW_QUAD_PIN_1 | YAW_QUAD_PIN_2);

    // Set the GPIO pins Weak Pull Down, 2mA
    GPIOPadConfigSet(YAW_QUAD_BASE, YAW_QUAD_PIN_1 | YAW_QUAD_PIN_2, YAW_QUAD_SIG_STRENGTH, YAW_QUAD_PIN_TYPE);

    // Set the GPIO pins to generate interrupts on both rising and falling edges
    GPIOIntTypeSet(YAW_QUAD_BASE, YAW_QUAD_PIN_1 | YAW_QUAD_PIN_2, YAW_QUAD_EDGE_TYPE);

    // Register the interrupt handler
    GPIOIntRegister(YAW_QUAD_BASE, yaw_int_handler);

    // Enable interrupts on GPIO Port B Pins 0,1 for Yaw channels A and B
    // (clears any outstanding interrupts)
    GPIOIntEnable(YAW_QUAD_BASE, YAW_QUAD_INT_PIN_1 | YAW_QUAD_INT_PIN_2);


    // enable the peripheral
    SysCtlPeripheralEnable(YAW_REF_PERIPH);

    // disable interrupts
    GPIOIntDisable(YAW_REF_BASE, YAW_REF_INT_PIN);

    // set it up as an input
    GPIOPinTypeGPIOInput(YAW_REF_BASE, YAW_REF_PIN);

    // set data direction register as input mfb
    GPIODirModeSet(YAW_REF_BASE, YAW_REF_PIN, YAW_QUAD_DDR);

    // configure it to be a weak pull down
    // try WPU
    GPIOPadConfigSet(YAW_REF_BASE, YAW_REF_PIN, YAW_REF_SIG_STRENGTH, YAW_REF_PIN_TYPE);

    // configure the interrupt to be falling edge only
    GPIOIntTypeSet(YAW_REF_BASE, YAW_REF_PIN, YAW_REF_EDGE_TYPE);

    // register the interrupt handler
    GPIOIntRegister(YAW_REF_BASE, yaw_reference_int_handler);

    // enable interrupts on this pin
    GPIOIntEnable(YAW_REF_BASE, YAW_REF_INT_PIN);
}

/**
 * The interrupt handler for the yaw reference.
 */
void yaw_reference_int_handler(void)
{
    // clear the interrupt flag first, so that it has time to actually be cleared
    GPIOIntClear(YAW_REF_BASE, YAW_REF_INT_PIN);

    if (!g_has_been_calibrated)
    {
        mutex_lock(g_slot_count_mutex);
        g_slot_count = 0;
        mutex_unlock(g_slot_count_mutex);

        mutex_lock(g_has_been_calibrated_mutex);
        g_has_been_calibrated = true;
        mutex_unlock(g_has_been_calibrated_mutex);
    }

}

/**
* Updates the current state of the Quadrature FSM.
* The general thinking is explained in the following document:
* https://cdn.sparkfun.com/datasheets/Robotics/How%20to%20use%20a%20quadrature%20encoder.pdf
*/
void yaw_update_state(bool t_signal_a, bool t_signal_b)
{
    mutex_lock(g_slot_count_mutex);
    mutex_lock(g_quadrature_state_mutex);

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

    mutex_unlock(g_quadrature_state_mutex);
    mutex_unlock(g_slot_count_mutex);
}

void yaw_update_settling(uint32_t t_time_diff_micro, KernelTask* t_task)
{
    // we add 180 degrees because we only care about the settling
    // around 0 degrees and because of underflows this is difficult
    writeCircBuf(&g_settling_buffer, yaw_get() + 180);
}

/**
* Returns the current state of the Quadrature FSM.
*/
QuadratureState yaw_get_state(void)
{
    mutex_wait(g_quadrature_state_mutex);
    QuadratureState temp_state = g_quadrature_state;

    mutex_wait(g_quadrature_state_mutex);
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
    // clear the interrupt flag first as it takes some cycles to actually be cleared
    GPIOIntClear(YAW_QUAD_BASE, YAW_QUAD_INT_PIN_1 | YAW_QUAD_INT_PIN_2);

    if (g_has_been_calibrated)
    {
        // read signal A
        bool signal_a = GPIOPinRead(YAW_QUAD_BASE, YAW_QUAD_PIN_1);

        // read signal B
        bool signal_b = GPIOPinRead(YAW_QUAD_BASE, YAW_QUAD_PIN_2);

        // update the quadrature stuff
        yaw_update_state(signal_a, signal_b);
    }
}

void yaw_reset_calibration_state(void)
{
    mutex_wait(g_has_been_calibrated_mutex);
    g_has_been_calibrated = false;
}

bool yaw_has_been_calibrated(void)
{
    mutex_wait(g_has_been_calibrated_mutex);
    return g_has_been_calibrated;
}

bool yaw_is_settled(void)
{
    return getRangeCircBuf(&g_settling_buffer) <= YAW_SETTLING_MARGIN * 2;
}

int32_t yaw_get_settled(void)
{
    if (yaw_is_settled()) {
        return (getSmallestCircBuf(&g_settling_buffer) + YAW_SETTLING_MARGIN - 180);
    }
    return -1;
}

bool yaw_is_settled_around(int32_t t_value)
{
    if (yaw_is_settled())
    {
        return range(180 + yaw_get_settled(), 180 + t_value - YAW_SETTLING_MARGIN, 180 + t_value + YAW_SETTLING_MARGIN);
    }
    else
    {
        return false;
    }
}

