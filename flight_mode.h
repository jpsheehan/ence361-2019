/*******************************************************************************
 *
 * flight_mode.h
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
 * Description:
 * This module contains the definitions for the Operating Mode (or FLight Status)
 * of the helicopter under control of a Finite Sate Machine
 *
 *********************************************************************************/

#ifndef FLIGHT_MODE_H_
#define FLIGHT_MODE_H_

#include <stdint.h>
#include <stdbool.h>

#include "kernel.h"

/**
 * Defines the possible states for the operating mode (a.k.a. flight status),
 * state machine.
 * Finding yaw reference is part of take off,
 * Setting altitude occurs at start-up up so CAL start may not be needed.
 */
enum flight_mode_state_e { LANDED,
                                  TAKE_OFF,
                                  IN_FLIGHT,
                                  LANDING };
typedef enum flight_mode_state_e FlightModeState;

/**
 * Initialise operating mode (a.k.a. flight status), state machine.
 */
void flight_mode_init(void);

/**
 * Get current operating mode (a.k.a. flight status), state machine.
 */
FlightModeState flight_mode_get(void);

/**
 * Advance the state.
 */
void flight_mode_advance_state(void);

/**
 * KERNEL TASK.
 * Called by the kernel. Checks conditions and advances state.
 */
void flight_mode_update(uint32_t t_time_diff_micro, KernelTask* t_task);

#endif /* OPSTATUSFSM_H_ */
