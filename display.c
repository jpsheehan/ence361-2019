/*
 * display.c
 *
 *  Created on: 17/04/2019
 *      Author: wgc22
 */


#include <stdint.h>

#include "OrbitOLED/OrbitOLEDInterface.h"
#include "utils/ustdlib.h"

#include "display.h"
#include "altitude.h"
#include "quadrature.h"
#include "utils.h"

#define DISPLAY_CLAMPED_PERCENTAGE_ALTITUDE false

enum disp_state { DISP_STATE_CALIBRATION, DISP_STATE_PERCENT_ALT, DISP_STATE_RAW_ALT, DISP_STATE_YAW, DISP_STATE_OFF, DISP_STATE_TOTAL };
typedef enum disp_state DisplayState;

static uint8_t g_displayState = DISP_STATE_CALIBRATION;
static uint8_t g_oldDisplayState = DISP_STATE_OFF;

void disp_init()
{
    // intialise the Orbit OLED display
    OLEDInitialise ();
}

void disp_rawAlt() {
    char string[17];  // 16 characters across the display

    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
    usnprintf (string, sizeof(string), "Altitude = %4d", alt_getRaw());

    // Update line on display.
    OLEDStringDraw (string, 0, 2);

}

void disp_percentAlt() {
    char string[17];  // 16 characters across the display

    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
#if DISPLAY_CLAMPED_PERCENTAGE_ALTITUDE
    uint8_t clampedAltitudePercentage = clamp(g_latestAltitudePercentage, 0, 100);
    usnprintf (string, sizeof(string), "Altitude = %3d%%", clampedAltitudePercentage);
#else
    usnprintf (string, sizeof(string), "Altitude = %3d%%", alt_getPercent());
#endif

    // Update line on display.
    OLEDStringDraw (string, 0, 2);

}

void disp_yaw()
{
    QuadratureState state = quad_getState();

    char string[17];
    usnprintf (string, sizeof(string), "Yaw = %3d", quad_getYawDegrees());
    OLEDStringDraw (string, 0, 2);
}

void disp_clear() {
    OLEDStringDraw ("Helicopter Ctrl ", 0, 0);
    OLEDStringDraw ("                ", 0, 1);
    OLEDStringDraw ("                ", 0, 2);
    OLEDStringDraw ("                ", 0, 3);
}

void disp_none() {
    OLEDStringDraw ("                ", 0, 0);
}

void disp_calibration() {
    OLEDStringDraw ("ENCE361", 9, 0);
    OLEDStringDraw ("mfb31", 0, 1);
    OLEDStringDraw ("wgc22", 0, 2);
    OLEDStringDraw ("jps111", 0, 3);
}


void disp_advanceState()
{
    if (++g_displayState >= DISP_STATE_TOTAL) {
        g_displayState = DISP_STATE_CALIBRATION + 1;
    }
}

void disp_render()
{
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
    }
}
