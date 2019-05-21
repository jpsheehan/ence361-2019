/*******************************************************************************
 * 
 * altitude.c
 * 
 * ENEL361 Helicopter Project
 * Friday Morning, Group 7
 * 
 * Written by:
 *  - Manu Hamblyn  <mfb31<@uclive.ac.nz>   95140875
 *  - Will Cowper   <wgc22@uclive.ac.nz>    81163265
 *  - Jesse Sheehan <jps111@uclive.ac.nz>   53366509
 * 
 * This file contains portions of code that written by P.J. Bones. These portions are noted in the comments.
 * 
 * Description:
 * This module contains functionality required for calculating the mean altitude
 * as a raw value and as a percentage of the overall height.
 * Functions are provided to initialise, calibrate, update and return
 * the altitude values.
 * 
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

// TODO: See which includes can be safely removed
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"

#include "circBufT.h"
#include "altitude.h"
#include "kernel.h"

/**
 * The size of the buffer used to store the raw ADC values. This needs to be big enough that outliers in the data cannot affect the calculated mean in an adverse way.
 */
static const int ALT_BUF_SIZE = 32;

/**
 * Figure out a way to have this but also not repeat it in main.c!!! This is the frequency of the adc stuff
 */
static const int ALT_SAMPLE_RATE_HZ = 256;

/**
 * We are using ADC0 so we set up the base and peripheral
 */
static const int ADC_BASE = ADC0_BASE;
static const int ADC_PERIPH = SYSCTL_PERIPH_ADC0;

/**
 * We want sequence 3, step 0 of ADC0
 */
static const int ADC_SEQUENCE = 3;
static const int ADC_STEP = 0;

/**
 * The ideal resolution delta for a helicopter rig. This value may change depending on which helicopter rig is used. The ideal value is calculated as follows:
 * 
 * When the helicopter changes its height from landed (0% altitude) to full height (100% altitude) there is a voltage drop of 0.8 V. 
 * 
 * Because we are using a 12-bit ADC, that the maximum value we can read from the ADC is 2^12 - 1 (4095). The Tiva board uses 3.3 V as its supply voltage, so a resolution of 4095 corresponds to 3.3 V.
 * 
 * Hence the difference in 0.8 V corresponds to 4095 * 0.8 / 3.3 (993).
 */
static const int ALT_DELTA = 993;

/**
 * The circular buffer used to store the raw ADC values for calculating the mean.
 */
static circBuf_t g_circ_buffer;

/**
 * The reference altitude. This is updated when calling the alt_calibrate function. This is required for calculating the altitude as a percentage.
 */
static uint16_t g_alt_ref;

/**
 * The mean altitude. This is updated when the `void alt_update()` function is called.
 */
static uint32_t g_alt_raw;

/**
 * The mean altitude as a percentage of full height. This is updated when the `void alt_update()` function is called.
 */
static int16_t g_alt_percent;

/**
 * Indicates if the altitude has been calibrated yet. This is set when calling the `void alt_calibrate()` function and returned when calling the `bool alt_getIsCalibrated()` function.
 */
static bool g_has_been_calibrated = false;

/**
 * (Original Code by P.J. Bones)
 * The handler for the ADC conversion complete interrupt.
 * Writes to the circular buffer.
 */
void alt_adc_int_handler(void)
{
    uint32_t value;

    //
    // Get the single sample from ADC0.  ADC_BASE is defined in
    // inc/hw_memmap.h
    ADCSequenceDataGet(ADC_BASE, ADC_SEQUENCE, &value);
    //
    // Place it in the circular buffer (advancing write index)
    writeCircBuf(&g_circ_buffer, value);
    //
    // Clean up, clearing the interrupt
    ADCIntClear(ADC_BASE, ADC_SEQUENCE);
}

/**
 * (Original code by P.J. Bones)
 * Initialises the ADC module on the Tivaboard.
 * TODO: Give more information about the ADC we are using to perform the conversion.
 */
void alt_init_adc(void)
{
    //
    // The ADC0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(ADC_PERIPH);

    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a signal to start the
    // conversion.
    ADCSequenceConfigure(ADC_BASE, ADC_SEQUENCE, ADC_TRIGGER_PROCESSOR, ADC_STEP);

    //
    // Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
    // single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
    // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
    // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing a single
    // conversion using sequence 3 we will only configure step 0.  For more
    // on the ADC sequences and steps, refer to the LM3S1968 datasheet.
    ADCSequenceStepConfigure(ADC_BASE, ADC_SEQUENCE, ADC_STEP, ADC_CTL_CH9 | ADC_CTL_IE | ADC_CTL_END);

    //
    // Since sample sequence 3 is now configured, it must be enabled.
    ADCSequenceEnable(ADC_BASE, ADC_SEQUENCE);

    //
    // Register the interrupt handler
    ADCIntRegister(ADC_BASE, ADC_SEQUENCE, alt_adc_int_handler);

    //
    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable(ADC_BASE, ADC_SEQUENCE);
}

/**
 * (Original code by P.J. Bones)
 * The interrupt handler for the for SysTick interrupt.
 */
void alt_process_adc(uint32_t t_time_diff_micro)
{
    //
    // Initiate a conversion
    //
    ADCProcessorTrigger(ADC_BASE, ADC_SEQUENCE);
}

void alt_init(void)
{
    alt_init_adc();
    initCircBuf(&g_circ_buffer, ALT_BUF_SIZE);
}

void alt_update(uint32_t t_time_diff_micro)
{
    int32_t sum;
    uint16_t i;

    // add up all the values in the circular buffer
    sum = 0;
    for (i = 0; i < ALT_BUF_SIZE; i++)
        sum = sum + readCircBuf(&g_circ_buffer);

    // calculate the mean of the data in the circular buffer
    g_alt_raw = (2 * sum + ALT_BUF_SIZE) / (2 * ALT_BUF_SIZE);

    // calculate the percentage mean
    g_alt_percent = (int16_t)((((int32_t)g_alt_ref - (int32_t)g_alt_raw) * (int32_t)100) / (int32_t)ALT_DELTA);
}

void alt_calibrate(void)
{
    g_alt_ref = g_alt_raw;
    g_has_been_calibrated = true;
}

int16_t alt_get(void)
{
    return g_alt_percent;
}

bool alt_has_been_calibrated(void)
{
    return g_has_been_calibrated;
}

bool alt_get_is_buffer_full(void)
{
    return (kernel_get_systick_count() > ALT_SAMPLE_RATE_HZ);
}

void alt_reset_calibration_state(void)
{
    g_has_been_calibrated = false;
}
