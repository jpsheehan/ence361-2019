/*******************************************************************************
 *
 * display.c
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
 * This module contains functions for initialising and updating the display.
 *
 ******************************************************************************/

#include <stdint.h>

#include "OrbitOLED/OrbitOLEDInterface.h"
#include "utils/ustdlib.h"

#include "display.h"
#include "altitude.h"
#include "utils.h"
#include "yaw.h"
#include "pwm.h"

/**
 * When true clamps percentage between 0 and 100%
 */
#define DISPLAY_CLAMPED_PERCENTAGE_ALTITUDE false

/**
 * Bytecode for rendering degree symbol on the display
 */
#define DISP_SYMBOL_DEGREES 0x60

/**
 * Enum of all states the display can be in. Cycled by pressing BTN2
 */
enum disp_state
{
    DISP_STATE_CALIBRATION,
    DISP_STATE_ALL,
    DISP_STATE_TOTAL
};
typedef enum disp_state DisplayState;

/**
 * Current display state
 */
static uint8_t g_displayState = DISP_STATE_CALIBRATION;

/**
 * Display raw 12-bit adc reading to the display
 */
void disp_clear(void)
{
    OLEDStringDraw("                ", 0, 0);
    OLEDStringDraw("                ", 0, 1);
    OLEDStringDraw("                ", 0, 2);
    OLEDStringDraw("                ", 0, 3);
}

/**
 * Splash screen used during initial calibration while waiting for buffer to fill
 */
void disp_calibration(void)
{
    OLEDStringDraw("         ENCE361", 0, 0);
    OLEDStringDraw("mfb31", 0, 1);
    OLEDStringDraw("wgc22", 0, 2);
    OLEDStringDraw("jps111", 0, 3);
}

/**
 * Advance display state when BTN2 is pressed
 */
void disp_advanceState(void)
{
    if (++g_displayState >= DISP_STATE_TOTAL)
    {
        g_displayState = DISP_STATE_CALIBRATION + 1;
    }
}

/**
 * Display yaw and altitude percentage at the same time
 */
void disp_all(void)
{
    char string[17];

    usnprintf(string, sizeof(string), "Main Duty: %4d%%", pwm_get_main_duty());
    OLEDStringDraw(string, 0, 0);

    usnprintf(string, sizeof(string), "Tail Duty: %4d%%", pwm_get_tail_duty());
    OLEDStringDraw(string, 0, 1);

    usnprintf(string, sizeof(string), "      Yaw: %4d%c", yaw_getDegrees(), DISP_SYMBOL_DEGREES);
    OLEDStringDraw(string, 0, 2);

    usnprintf(string, sizeof(string), " Altitude: %4d%%", alt_get());
    OLEDStringDraw(string, 0, 3);
}

/**
 * Unknown display state fail-safe
 */
void disp_unknown(void)
{
    OLEDStringDraw("Unknown display", 0, 2);
    OLEDStringDraw("state!", 0, 3);
}

void disp_render(uint32_t t_time_diff_micro)
{
    switch (g_displayState)
    {
    case DISP_STATE_CALIBRATION:
        disp_calibration();
        break;
    case DISP_STATE_ALL:
        disp_all();
        break;
    default:
        disp_unknown();
        break;
    }
}

void disp_init(void)
{
    // Intialise the Orbit OLED display
    OLEDInitialise();
    disp_clear();
}
