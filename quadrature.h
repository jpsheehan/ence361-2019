/*
 * quadrature_enc.h
 *
 *  Created on: 10/04/2019
 *      Author: jps111
 */

#ifndef QUADRATURE_H_
#define QUADRATURE_H_

#include <stdint.h>
#include <stdbool.h>

enum quadrature_state { QUAD_STATE_CLOCKWISE, QUAD_STATE_ANTICLOCKWISE, QUAD_STATE_NOCHANGE, QUAD_STATE_INVALID };
typedef enum quadrature_state QuadratureState;

void quad_init();
uint8_t quad_getSlotCount();
uint16_t quad_getYawDegrees();

#endif /* QUADRATURE_H_ */
