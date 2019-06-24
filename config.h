/*
 * config.h
 *
 *  Created on: 24/06/2019
 *      Author: jps111
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdbool.h>

// set to true if we want to purposefully introduce long-running tasks
// to the kernel. this will mess it up.
#define SATURATE_KERNEL false

// set to true if we want to send kernel timing data down the UART
#define DUMP_KERNEL_DATA false

// set to true if we want to directly control the duty cycle of the helirig
// and turn off the control systems.
#define CONFIG_DIRECT_CONTROL false

/**
 * The amount to change the yaw duty cycle by when in direct control.
 */
#define CONFIG_DIRECT_CONTROL_YAW_DUTY_DELTA 1

/**
 * The amount to change the main duty cycle by when in direct control.
 */
#define CONFIG_DIRECT_CONTROL_MAIN_DUTY_DELTA 1

#endif /* CONFIG_H_ */
