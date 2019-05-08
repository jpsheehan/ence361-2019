/*
 * setpoint.h
 *
 *  Created on: 9/05/2019
 *      Author: jps111
 */

#ifndef SETPOINT_H_
#define SETPOINT_H_

#include <stdint.h>

void setpoint_init(void);

void setpoint_increment_yaw(void);
void setpoint_decrement_yaw(void);
int16_t setpoint_get_yaw(void);

void setpoint_increment_altitude(void);
void setpoint_decrement_altitude(void);
int16_t setpoint_get_altitude(void);

#endif /* SETPOINT_H_ */
