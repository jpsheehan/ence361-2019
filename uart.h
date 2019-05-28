/*******************************************************************************
 *
 * uart.h
 *
 * ENEL361 Helicopter Project
 * Friday Morning, Group 7
 *
 * Written by:
 *  - Manu Hamblyn  <mfb31<@uclive.ac.nz>   95140875
 *  - Will Cowper   <wgc22@uclive.ac.nz>    81163265
 *  - Jesse Sheehan <jps111@uclive.ac.nz>   53366509
 *
 * This file reference portions of code that written by P.J. Bones.
 * These portions are noted in the comments.
 *
 * Description:
 * This module contains prototypes that facilitate sending data via
 * USB UART to the host computer.
 *
 ******************************************************************************/

#ifndef UART_H_
#define UART_H_

#include <stdint.h>

#include "kernel.h"

/**
 * (Original Code by P.J. Bones)
 * initialiseUSB_UART - 8 bits, 1 stop bit, no parity
 */
void uart_init(void);

/**
 * Transmits the helicopter status via UART.
 */
void uart_flight_data_update(uint32_t t_time_diff_micro, KernelTask* t_task);

/**
 * Transmits the kernel duration statuses via UART.
 */
void uart_kernel_data_update(uint32_t t_time_diff_micro, KernelTask* t_task);

#endif /* UART_H_ */
