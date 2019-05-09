/*******************************************************************************
 *
 * kernel.h
 *
 * ENEL361 Helicopter Project
 * Friday Morning, Group 7
 *
 * Written by:
 *  - Manu Hamblyn  <mfb31<@uclive.ac.nz>   95140875
 *  - Will Cowper   <wgc22@uclive.ac.nz>    81163265
 *  - Jesse Sheehan <jps111@uclive.ac.nz>   53366509
 *
 * Description:
 * This module contains functions for scheduling tasks.
 *
 ******************************************************************************/

#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * Initialises the kernel.
 */
void kernel_init(void);

/**
 * Adds a task with a priority level (higher is lower priority) to the kernel.
 */
void kernel_add_task(void (*t_task)(void), uint8_t t_priority);

/**
 * Runs the next kernel task.
 */
void kernel_run(void);

/**
 * Returns true if memory was allocated ok. Nothing will run if this returns false.
 */
bool kernel_ready(void);

#endif /* KERNEL_H_ */
