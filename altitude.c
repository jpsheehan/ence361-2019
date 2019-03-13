/*
 * altitude.c
 *
 *  Created on: 13/03/2019
 *      Author: jps111
 */

#include "altitude.h"

// the flag to be set if the altitude mean is to be calculated
static uint8_t altitude_mean_flag = 0;

// the raw altitude ADC average
static uint8_t altitude_mean_raw = 0;

// the percentage altitude
static uint8_t altitude_mean_percent = 0;

// the reference for the altitude
static uint8_t altitude_reference = 0;


/**
 * Initializes the ADC and timer for the altitude sampling.
 */
void AltitudeSamplingInit()
{

}

/**
 * The ISR for sampling the altitude.
 */
void AltitudeSamplingIntHandler()
{

}

/**
 * Initializes a timer for the altitude average calculations.
 */
void AltitudeMeanInit()
{

}

/**
 * The ISR for the altitude average calculations. This should set the altitude_mean_flag.
 */
void AltitudeMeanIntHandler()
{

}

/**
 * Sets the altitude_mean_raw and altitude_mean_percent global variables.
 */
void AltitudeCalculateMeans()
{

}

/**
 * Sets the altitude_reference global variable.
 */
void AltitudeCalibrate()
{

}

// Returns the raw mean ADC altitude value.
uint8_t AltitudeGetMeanRaw()
{

}

/**
 * Returns the percentage mean ADC altitude value.
 */
uint8_t AltitudeGetMeanPercent()
{

}

/**
 * Returns true if the altitude means should be recalculated.
 */
uint8_t AltitudeGetMeanFlag()
{

}

/**
 * Clears the altitude_mean_flag.
 */
void AltitudeResetMeanFlag()
{

}
