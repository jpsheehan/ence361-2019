/*******************************************************************************
 *
 * control.h
 *
 * ENEL361 Helicopter Project
 * Friday Morning, Group 7
 *
 * Written by:
 *  - Manu Hamblyn  <mfb31<@uclive.ac.nz>   95140875
 *  - Will Cowper   <wgc22@uclive.ac.nz>    81163265
 *  - Jesse Sheehan <jps111@uclive.ac.nz>   53366509
 *
 *
 *
 * Description:
 * header file for control.c
 * used yaw and altitude PID controllers
 *
 ******************************************************************************/



#ifndef CONTROL_H_
#define CONTROL_H_

#include <stdint.h>
#include <stdbool.h>

#include "kernel.h"

struct control_gains_s
{
  float kp;
  float ki;
  float kd;
};

/**
 * Represents some initial gains for a control system.
 */
typedef struct control_gains_s ControlGains;

/**
 * Initialises the two control systems using the specific gains.
 */
void control_init(ControlGains t_altitude_gains, ControlGains t_yaw_gains);

/**
 * Updates the altitude control system based on the any new data.
 */
void control_update_altitude(KernelTask* t_task);

/**
 * Updates the yaw control system based on any new data.
 */
void control_update_yaw(KernelTask* t_task);

/**
 * Enables or disables the altitude controller.
 */
void control_enable_altitude(bool t_enabled);

/**
 * Enables or disables the yaw controller.
 */
void control_enable_yaw(bool t_enabled);

#endif
