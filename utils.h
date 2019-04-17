/*
 * utils.h
 *
 *  Created on: 10/04/2019
 *      Author: jps111
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <stdint.h>

int32_t utils_clamp(int32_t value, int32_t min, int32_t max);
void utils_waitForSeconds(uint32_t delay_s);

#endif /* UTILS_H_ */
