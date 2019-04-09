//*****************************************************************************
// ENCE361-S1 Milestone 1
// Group 7: Student I.D.s 53366509, 81163265, 95140875
//
// Code incudes various ECE sources including ADC demo, week2 lab,
// circ buffer, buttons
// Last modifed: 22_03_2019 by Group 7
//
// ***************************************************************************
// Original code:
// ADCdemo1.c - Simple interrupt driven program which samples with AIN0
//
// Author:  P.J. Bones	UCECE
// Last modified:	8.2.2018
//*****************************************************************************
// Based on the 'convert' series from 2016
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"
#include "circBufT.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "buttons4.h"
#include "utils.h"

//*****************************************************************************
// Constants
//*****************************************************************************
#define BUF_SIZE 32
#define SAMPLE_RATE_HZ 256

#define ALTITUDE_DELTA 993

#define DISPLAY_OFF 0
#define DISPLAY_PERCENT_ADC 1
#define DISPLAY_MEAN_ADC 2

#define SECOND_DELAY_COEFFICIENT 3

//*****************************************************************************
// Global variables
//*****************************************************************************
static circBuf_t g_inBuffer;		// Buffer of size BUF_SIZE integers (sample values)
static uint32_t g_ulSampCnt;	// Counter for the interrupts
static uint16_t g_altitudeReference; // the reference value for calculating height.
static uint32_t g_latestAltitudeMean; // the latest altitude value
static uint32_t g_latestAltitudePercentage;
static bool g_hasBeenCalibrated = false;

static uint8_t g_displayState = DISPLAY_PERCENT_ADC;
static bool g_togglePB3 = false;

//*****************************************************************************
//
// The interrupt handler for the for SysTick interrupt.
//*****************************************************************************
void
SysTickIntHandler(void)
{
    //
    // Initiate a conversion
    //
    ADCProcessorTrigger(ADC0_BASE, 3); 
    g_ulSampCnt++;
}

//*****************************************************************************
//
// The handler for the ADC conversion complete interrupt.
// Writes to the circular buffer.
//*****************************************************************************
void
ADCIntHandler(void)
{
	uint32_t ulValue;
	
	//
	// Get the single sample from ADC0.  ADC_BASE is defined in
	// inc/hw_memmap.h
	ADCSequenceDataGet(ADC0_BASE, 3, &ulValue);
	//
	// Place it in the circular buffer (advancing write index)
	writeCircBuf (&g_inBuffer, ulValue);
	//
	// Clean up, clearing the interrupt
	ADCIntClear(ADC0_BASE, 3);                          
}

//*****************************************************************************
// Initialisation functions for the clock (incl. SysTick), ADC, display
//*****************************************************************************
void
initClock (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
}

void initSysTick(void) {
    //
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / SAMPLE_RATE_HZ);
    //
    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}

void 
initADC (void)
{
    //
    // The ADC0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    
    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a signal to start the
    // conversion.
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
  
    //
    // Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
    // single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
    // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
    // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing a single
    // conversion using sequence 3 we will only configure step 0.  For more
    // on the ADC sequences and steps, refer to the LM3S1968 datasheet.
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_IE |
                             ADC_CTL_END);    
                             
    //
    // Since sample sequence 3 is now configured, it must be enabled.
    ADCSequenceEnable(ADC0_BASE, 3);
  
    //
    // Register the interrupt handler
    ADCIntRegister (ADC0_BASE, 3, ADCIntHandler);
  
    //
    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable(ADC0_BASE, 3);
}

void
initDisplay (void)
{
    // intialise the Orbit OLED display
    OLEDInitialise ();
}

void
updateAltitude()
{
    int32_t sum;
    uint16_t i;
    int32_t altitudePercentage;

    sum = 0;
    for (i = 0; i < BUF_SIZE; i++)
        sum = sum + readCircBuf (&g_inBuffer);
    g_latestAltitudeMean = (2 * sum + BUF_SIZE) / (2 * BUF_SIZE);

    altitudePercentage = ((((int32_t)g_altitudeReference - (int32_t)g_latestAltitudeMean) * (int32_t)100) / (int32_t)ALTITUDE_DELTA);

    g_latestAltitudePercentage = (uint32_t)clamp(altitudePercentage, 0, 100);
}

void displayMeanADC() {
    char string[17];  // 16 characters across the display

    OLEDStringDraw ("Helicopter Ctrl", 0, 0);
    OLEDStringDraw ("                ", 0, 1);
    OLEDStringDraw ("                ", 0, 3);


    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
    usnprintf (string, sizeof(string), "Altitude = %4d", g_latestAltitudeMean);

    // Update line on display.
    OLEDStringDraw (string, 0, 2);

}

void displayPercentADC() {
    char string[17];  // 16 characters across the display

    OLEDStringDraw ("Helicopter Ctrl ", 0, 0);
    OLEDStringDraw ("                ", 0, 1);
    OLEDStringDraw ("                ", 0, 3);


    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
    usnprintf (string, sizeof(string), "Altitude = %3d%%", g_latestAltitudePercentage);

    // Update line on display.
    OLEDStringDraw (string, 0, 2);

}

void displayNone() {
    OLEDStringDraw ("                ", 0, 0);
    OLEDStringDraw ("                ", 0, 1);
    OLEDStringDraw ("                ", 0, 2);
    OLEDStringDraw ("                ", 0, 3);
}

void displayCalibration() {
    OLEDStringDraw ("ENCE361", 9, 0);
    OLEDStringDraw ("mfb31", 0, 1);
    OLEDStringDraw ("wgc22", 0, 2);
    OLEDStringDraw ("jps111", 0, 3);
}

void calibrate() {
    g_altitudeReference = g_latestAltitudeMean;
    g_hasBeenCalibrated = true;
}

void togglePB3() {

    g_togglePB3 = !g_togglePB3;

    if (g_togglePB3) {
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, GPIO_PIN_3);
    } else {
        GPIOPinWrite(GPIO_PORTB_BASE,  GPIO_PIN_3, 0x00);
    }
}

void initPB3() {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_3, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPD);
    GPIODirModeSet(GPIO_PORTB_BASE, GPIO_PIN_3, GPIO_DIR_MODE_OUT);
}

/**
 * Does what it says on the tin.
 */
void waitForSeconds(uint32_t delay_s)
{
    SysCtlDelay((SysCtlClockGet() * delay_s) / SECOND_DELAY_COEFFICIENT);
}

int
main(void)
{
	
	initClock ();
	initSysTick();
	initADC ();
	initDisplay ();
	initCircBuf (&g_inBuffer, BUF_SIZE);
	initButtons();
	initPB3();

    //
    // Enable interrupts to the processor.
    IntMasterEnable();

	while (1)
	{
	    butStates_t butState;

	    if (g_hasBeenCalibrated) {

            //
            // Background task: calculate the (approximate) mean of the values in the
            // circular buffer and display it, together with the sample number.

            updateButtons();

            // check for reference calibration
            butState = checkButton(LEFT);
            if (butState == PUSHED) {
                calibrate();
            }

            // check for display state change
            butState = checkButton(UP);
            if (butState == PUSHED) {
                if (++g_displayState > DISPLAY_MEAN_ADC) {
                    g_displayState = DISPLAY_OFF;
                }
            }

            updateAltitude();

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
            }

	    } else {

	        displayCalibration();

	        // wait a 3 seconds
	        waitForSeconds(3);

	        // check that we have filled the buffer with data
	        if (g_ulSampCnt > BUF_SIZE) {
	            updateAltitude();
	            calibrate();
	        }

	    }

	    togglePB3();
	}
}

