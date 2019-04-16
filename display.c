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

static uint8_t g_displayState = DISPLAY_CALIBRATION;
static uint8_t g_oldDisplayState = DISPLAY_OFF;

void displayInit()
{
    // intialise the Orbit OLED display
    OLEDInitialise ();
}

void displayMeanADC() {
    char string[17];  // 16 characters across the display

    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
    usnprintf (string, sizeof(string), "Altitude = %4d", getAltitudeRaw());

    // Update line on display.
    OLEDStringDraw (string, 0, 2);

}

void displayPercentADC() {
    char string[17];  // 16 characters across the display

    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
#if DISPLAY_CLAMPED_PERCENTAGE_ALTITUDE
    uint8_t clampedAltitudePercentage = clamp(g_latestAltitudePercentage, 0, 100);
    usnprintf (string, sizeof(string), "Altitude = %3d%%", clampedAltitudePercentage);
#else
    usnprintf (string, sizeof(string), "Altitude = %3d%%", getAltitudePercentage());
#endif

    // Update line on display.
    OLEDStringDraw (string, 0, 2);

}

void displayYaw()
{
    QuadratureState state = getQuadratureState();

    char string[17];
    usnprintf (string, sizeof(string), "Yaw = %3d", getYawDegrees());
    OLEDStringDraw (string, 0, 2);
}

void displayClear() {
    OLEDStringDraw ("Helicopter Ctrl ", 0, 0);
    OLEDStringDraw ("                ", 0, 1);
    OLEDStringDraw ("                ", 0, 2);
    OLEDStringDraw ("                ", 0, 3);
}

void displayNone() {
    OLEDStringDraw ("                ", 0, 0);
}

void displayCalibration() {
    OLEDStringDraw ("ENCE361", 9, 0);
    OLEDStringDraw ("mfb31", 0, 1);
    OLEDStringDraw ("wgc22", 0, 2);
    OLEDStringDraw ("jps111", 0, 3);
}


void displayStateAdvance()
{
    if (++g_displayState > DISPLAY_OFF) {
        g_displayState = DISPLAY_CALIBRATION + 1;
    }
}

void displayRender()
{
    if (g_oldDisplayState != g_displayState) {
                    g_oldDisplayState = g_displayState;
                    displayClear();
                }

                switch (g_displayState) {
                case DISPLAY_MEAN_ADC:
                    displayMeanADC();
                    break;
                case DISPLAY_PERCENT_ADC:
                    displayPercentADC();
                    break;
                case DISPLAY_OFF:
                     displayNone();
                    break;
                case DISPLAY_YAW:
                    displayYaw();
                    break;
                case DISPLAY_CALIBRATION:
                    displayCalibration();
                    break;
                }
}
