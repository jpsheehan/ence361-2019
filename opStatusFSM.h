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
 * This module contains the definitions for the Operating mode (or FLight Status) Finite Sate Machine
 *
 *********************************************************************************************/
 
#ifndef OPSTATUSFSM_H_
#define OPSTATUSFSM_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * Defines the possible states for the operating mode (a.k.a. flight status), state machine.
 * Finding yaw reference is part of take off, Setting altitude occurs at start-up up so CAL start may not be needed. 
 */
enum operating_mode { LANDED, TAKE_OFF, IN_FLIGHT, LANDING, CAL};
typedef enum operating_mode OperatingMode;

/**
 * Initialise operating mode (a.k.a. flight status), state machine.
 */
void opMode_init(void)

/**
 * Get current operating mode (a.k.a. flight status), state machine.
 */
OperatingMode opMode_get_current(void);

/**
 * Set operating mode (a.k.a. flight status), state machine..
 */
void opMode_set_current(OperatingMode);


#endif /* OPSTATUSFSM_H_ */
