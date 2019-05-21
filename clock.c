/*
 * clock.c
 *
 *  Created on: 21/05/2019
 *      Author: jps111
 */


#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"

#include "clock.h"

/**
 * (Original Code by P.J. Bones)
 * Initialisation functions for the clock (incl. SysTick), ADC, display
 */
void clock_init(void)
{
    // Set the clock rate to 40 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
}
