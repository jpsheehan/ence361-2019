/*
 * kernel.h
 *
 *  Created on: 10/05/2019
 *      Author: jps111
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdint.h>

void kernel_init(void);

void kernel_add_task(void (*t_task)(void));

void kernel_run(void);



#endif /* KERNEL_H_ */
