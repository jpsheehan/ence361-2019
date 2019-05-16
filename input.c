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

        /*
        if (flightMode_get_current == TAKE_OFF) {
            setpoint_set_altitude(ALTITUDE_YAW_REF);        //Some altitude to find yaw reference? (counter torque could assist). Could just make zero if not needed.
            pwm_set_tail_duty(PWM_TAIL_DUTY_YAW_REF);       //directly drive the TAIL until we get yaw reference

            flightMode_set_next();          //were in TAKE_OFF, move to in flight
            setpoint_set_yaw(0);            //hold at zero yaw with PID
            //TO DO
            //start Tail PID                //careful with the order!!
        }

        if (flightMode_get_current == LANDING) {
            setpoint_set_altitude(ALTITUDE_YAW_REF);               //hold some altitude until at yaw reference
            setpoint_set_yaw(0);
            while !( (yaw_getDegrees() >= 358) && (yaw_getDegrees() <= 2) ) { //error margin = +/- min resolution. CHECK!!
                continue;
            }
            setpoint_set_altitude(0);
            flightMode_set_next();                   //were landing, now landed
        }
        */
//    }
    }
}
