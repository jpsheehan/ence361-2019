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

enum quadrature_state { CLOCKWISE, ANTICLOCKWISE, NOCHANGE, INVALID };
typedef enum quadrature_state QuadratureState;

void initQuadrature(void);
void updateQuadratureState(bool signal_a, bool signal_b);
QuadratureState getQuadratureState(void);
void quadratureIntHandler(void);

#endif /* QUADRATURE_H_ */
