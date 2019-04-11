/*
 * quadrature.c
 *
 *  Created on: 10/04/2019
 *      Author: jps111
 */


#include "quadrature.h"

volatile static uint8_t g_previous_state;
volatile static QuadratureState g_quadrature_state;
volatile static uint8_t g_slot_count;

#define DEGREES_PER_SLOT 360 / 112

void initQuadrature(void)
{
    g_previous_state = 0b00;
    g_quadrature_state = NOCHANGE;
}

void updateQuadratureState(bool signal_a, bool signal_b)
{
    // compare with previous state
    uint8_t this_state = (signal_a << 1) | signal_b;

    // update g_quadrature_state depending on the previous state
    if (this_state == g_previous_state) {
        g_quadrature_state = NOCHANGE;
    } else {
        if (
                (this_state == 0 && g_previous_state == 1) ||
                (this_state == 1 && g_previous_state == 3) ||
                (this_state == 2 && g_previous_state == 0) ||
                (this_state == 3 && g_previous_state == 2)) {

            g_quadrature_state = ANTICLOCKWISE;
            if (--g_slot_count > 111) {
                g_slot_count = 111;
            }

        } else {
            if (
                    (this_state == 0 && g_previous_state == 2) ||
                    (this_state == 1 && g_previous_state == 0) ||
                    (this_state == 2 && g_previous_state == 3) ||
                    (this_state == 3 && g_previous_state == 1)) {
                g_quadrature_state = CLOCKWISE;
                if (++g_slot_count > 111) {
                    g_slot_count = 0;
                }
            } else {
                g_quadrature_state = INVALID;
            }
        }

    }

    // update g_previous_raw_quadrature_state to this
    g_previous_state = this_state;
}

QuadratureState getQuadratureState(void)
{
    QuadratureState temp_state = g_quadrature_state;
    g_quadrature_state = NOCHANGE;
    return temp_state;
}

uint8_t getSlotCount(void)
{
    return g_slot_count;
}

uint16_t getYawDegrees(void)
{
    return g_slot_count * DEGREES_PER_SLOT;
}
