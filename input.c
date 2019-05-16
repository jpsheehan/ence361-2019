#include <stdint.h>

#include "button.h"
#include "slider.h"
#include "flightMode.h"
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

    //
    // Background task: calculate the (approximate) mean of the values in the
    // circular buffer and display it, together with the sample number.

    btn_update();

    // Check for counter-clockwise rotation button press
    butState = btn_check(LEFT);
    if (butState == PUSHED)
    {
        if (flightMode_get_mode() == IN_FLIGHT)
        {
            setpoint_decrement_yaw();
        }
    }

    // Check for clockwise rotation button press
    butState = btn_check(RIGHT);
    if (butState == PUSHED)
    {
        if (flightMode_get_mode() == IN_FLIGHT)
        {
            setpoint_increment_yaw();
        }
    }

    // Check for increase altitude button press
    butState = btn_check(UP);
    if (butState == PUSHED)
    {
        if (flightMode_get_mode() == IN_FLIGHT)
        {
            setpoint_increment_altitude();
        }
    }

    // Check for decrease altitude button press
    butState = btn_check(DOWN);
    if (butState == PUSHED)
    {
        if (flightMode_get_mode() == IN_FLIGHT)
        {
            setpoint_decrement_altitude();
        }
    }

    // check sw1
    slider_update();
    SliderState sw1_state = slider_check(SLIDER_SW1);
    bool sw1_changed = slider_changed(SLIDER_SW1);

    if (sw1_state == SLIDER_DOWN)
    {
        if (flightMode_get_mode() == IN_FLIGHT)
        {
            flightMode_set_next(); //were flying, change to landing
        }
    }
    else
    {
        if (sw1_state == SLIDER_UP && sw1_changed)
        {
            // slider has been changed into the up position
            if (flightMode_get_mode() == LANDED)
            {
                flightMode_set_next(); //were landed, change to take off
            }
        }
    }
}
