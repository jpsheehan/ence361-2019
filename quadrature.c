/*
 * quadrature.c
 *
 *  Created on: 10/04/2019
 *      Author: jps111
 */


#include "quadrature.h"

volatile static uint8_t g_previous_state;
volatile static QuadratureState g_quadrature_state;

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

        } else {
            if (
                    (this_state == 0 && g_previous_state == 2) ||
                    (this_state == 1 && g_previous_state == 0) ||
                    (this_state == 2 && g_previous_state == 3) ||
                    (this_state == 3 && g_previous_state == 1)) {
                g_quadrature_state = CLOCKWISE;
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

