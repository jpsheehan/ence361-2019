/*******************************************************************************
 *
 * input.c
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
 * This module contains logic for initialising and processing inputs from the
 * Orbit Booster Pack slider switches and buttons.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/sysctl.h"

#include "button.h"
#include "slider.h"
#include "flight_mode.h"
#include "setpoint.h"
#include "input.h"

void input_init(void)
{
    btn_init();
    slider_init();
}

void input_update(uint32_t t_time_diff_micro)
{
    // Button state
    butStates_t butState;
    SliderState sw_state;
    bool sw_changed;

    //
    // Background task: calculate the (approximate) mean of the values in the
    // circular buffer and display it, together with the sample number.

    btn_update();

    // Check for counter-clockwise rotation button press
    butState = btn_check(LEFT);
    if (butState == PUSHED)
    {
        if (flight_mode_get() == IN_FLIGHT)
        {
            setpoint_decrement_yaw();
        }
    }

    // Check for clockwise rotation button press
    butState = btn_check(RIGHT);
    if (butState == PUSHED)
    {
        if (flight_mode_get() == IN_FLIGHT)
        {
            setpoint_increment_yaw();
        }
    }

    // Check for increase altitude button press
    butState = btn_check(UP);
    if (butState == PUSHED)
    {
        if (flight_mode_get() == IN_FLIGHT)
        {
            setpoint_increment_altitude();
        }
    }

    // Check for decrease altitude button press
    butState = btn_check(DOWN);
    if (butState == PUSHED)
    {
        if (flight_mode_get() == IN_FLIGHT)
        {
            setpoint_decrement_altitude();
        }
    }

    slider_update();

    // check sw1
    sw_state = slider_check(SLIDER_SW1);
    sw_changed = slider_changed(SLIDER_SW1);

    if (sw_state == SLIDER_DOWN)
    {
        if (flight_mode_get() == IN_FLIGHT)
        {
            flight_mode_advance_state(); //were flying, change to landing
        }
    }
    else
    {
        if (sw_state == SLIDER_UP && sw_changed)
        {
            // slider has been changed into the up position
            if (flight_mode_get() == LANDED)
            {
                flight_mode_advance_state(); //were landed, change to take off
            }
        }
    }

    sw_state = slider_check(SLIDER_SW2);
    sw_changed = slider_changed(SLIDER_SW2);

    if (sw_state == SLIDER_UP && sw_changed)
    {
        SysCtlReset();
    }


}
