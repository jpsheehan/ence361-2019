/*******************************************************************************
 *
 * opStatus.c
 *
 * ENEL361 Helicopter Project
 * Friday Morning, Group 7
 *
 * 10/05/2019
 *
 * Written by:
 *  - Manu Hamblyn  <mfb31<@uclive.ac.nz>   95140875
 *  - Will Cowper   <wgc22@uclive.ac.nz>    81163265
 *  - Jesse Sheehan <jps111@uclive.ac.nz>   53366509
 *
 * This file contains portions of code that written by P.J. Bones. These portions are noted in the comments.
 *
 * Description:
 * This module controls the Operating mode (or FLight Status) Finite Sate Machine
 *
 *********************************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "opStausFSM.h"


/**
 * Holds the previous state of the Operating mode (or FLight Status) Finite Sate Machine
 */
volatile static OperatingMode opMode_previous_state;

/**
 * Holds the current state of the Operating mode (or FLight Status) Finite Sate Machine
 */
volatile static OperatingMode opMode_current_state;

/**
 * Initialise operating mode (a.k.a. flight status), state machine.
 */
void init_OpMode(void)

/**
 * Get current operating mode (a.k.a. flight status), state machine.
 */
OperatingMode get_OpMode(void);

/**
 * Set operating mode (a.k.a. flight status), state machine..
 */
void set_OpMoode(OperatingMode);

