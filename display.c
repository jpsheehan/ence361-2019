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
enum disp_state { DISP_STATE_CALIBRATION, DISP_STATE_ALL, DISP_STATE_PERCENT_ALT, DISP_STATE_RAW_ALT, DISP_STATE_YAW, DISP_STATE_OFF, DISP_STATE_TOTAL };
typedef enum disp_state DisplayState;

/**
 * Current display state
 */
static uint8_t g_displayState = DISP_STATE_CALIBRATION;

/**
 * Previous display state. Before rendering the display is cleared if this differs from current display state
 */
static uint8_t g_oldDisplayState = DISP_STATE_OFF;

void disp_init(void)
{
    // Intialise the Orbit OLED display
    OLEDInitialise ();
}


/**
 * Display raw 12-bit adc reading to the display
 */
void disp_rawAlt(void) {
    char string[17];  // 16 characters across the display

    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
    usnprintf (string, sizeof(string), "Altitude = %4d", alt_getRaw());

    // Update line on display.
    OLEDStringDraw (string, 0, 2);

}
/**
 * Display the adc reading as a percentage
 * Only clamped between 0-100 if DISPLAY_CLAMPED_PERCENTAGE_ALTITUDE is TRUE
 */
void disp_percentAlt(void) {
    char string[17];  // 16 characters across the display

    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
#if DISPLAY_CLAMPED_PERCENTAGE_ALTITUDE
    uint8_t clampedAltitudePercentage = utils_clamp(g_latestAltitudePercentage, 0, 100);
    usnprintf (string, sizeof(string), "Altitude = %3d%%", clampedAltitudePercentage);
#else
    usnprintf (string, sizeof(string), "Altitude = %3d%%", alt_getPercent());
#endif

    // Draw string on display at character column x axis and row y axis.
    OLEDStringDraw (string, 0, 2);

}


/**
 * Display yaw reading to the display between 0-359
 * Positive yaw is CW.
 */
void disp_yaw(void)
{
    char string[17];
    usnprintf (string, sizeof(string), "Yaw = %3d%c", yaw_getDegrees(), DISP_SYMBOL_DEGREES);
    OLEDStringDraw (string, 0, 2);
}
/**
 * Display raw 12-bit adc reading to the display
 */
void disp_clear(void) {
    OLEDStringDraw ("Helicopter Ctrl ", 0, 0);
    OLEDStringDraw ("                ", 0, 1);
    OLEDStringDraw ("                ", 0, 2);
    OLEDStringDraw ("                ", 0, 3);
}
/**
 * Display a blank line to the display, clearing any previous render.
 */
void disp_none(void) {
    OLEDStringDraw ("                ", 0, 0);
}
/**
 * Splash screen used during initial calibration while waiting for buffer to fill
 */
void disp_calibration(void) {
    OLEDStringDraw ("         ENCE361", 0, 0);
    OLEDStringDraw ("mfb31", 0, 1);
    OLEDStringDraw ("wgc22", 0, 2);
    OLEDStringDraw ("jps111", 0, 3);
}

/**
 * Advance display state when BTN2 is pressed
 */
void disp_advanceState(void)
{
    if (++g_displayState >= DISP_STATE_TOTAL) {
        g_displayState = DISP_STATE_CALIBRATION + 1;
    }
}
/**
 * Display yaw and altitude percentage at the same time
 */
void disp_all(void)
{
    char string[17];

    usnprintf (string, sizeof(string), "     Yaw = %4d%c", yaw_getDegrees(), DISP_SYMBOL_DEGREES);
    OLEDStringDraw (string, 0, 2);

    usnprintf (string, sizeof(string), "Altitude = %4d%%", alt_getPercent());
    OLEDStringDraw (string, 0, 3);
}

/**
 * Unknown display state fail-safe
 */
void disp_unknown(void)
{
    OLEDStringDraw("Unknown display", 0, 2);
    OLEDStringDraw("state!", 0, 3);
}

void disp_render(void)
{
    // Clear display if old state differs from new.
    // Prevents artifacts from previous state showing on new render.
    if (g_oldDisplayState != g_displayState) {
        g_oldDisplayState = g_displayState;
        disp_clear();
    }

    switch (g_displayState) {
    case DISP_STATE_RAW_ALT:
        disp_rawAlt();
        break;
    case DISP_STATE_PERCENT_ALT:
        disp_percentAlt();
        break;
    case DISP_STATE_OFF:
         disp_none();
        break;
    case DISP_STATE_YAW:
        disp_yaw();
        break;
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
