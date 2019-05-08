/*******************************************************************************
 *
 * uart.c
 *
 * ENEL361 Helicopter Project
 * Friday Morning, Group 7
 *
 * Written by:
 *  - Manu Hamblyn  <mfb31<@uclive.ac.nz>   95140875
 *  - Will Cowper   <wgc22@uclive.ac.nz>    81163265
 *  - Jesse Sheehan <jps111@uclive.ac.nz>   53366509
 *
 * This file contains portions of code that written by P.J. Bones. These portions are noted in the comments.
 *
 * Description:
 * This module contains functions and constants that facilitate sending data via
 * USB UART to the host computer.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "utils/ustdlib.h"
#include "stdio.h"
#include "stdlib.h"

#include "yaw.h"
#include "uart.h"
#include "altitude.h"
#include "setpoint.h"

#define BAUD_RATE 9600
#define UART_USB_BASE           UART0_BASE
#define UART_USB_PERIPH_UART    SYSCTL_PERIPH_UART0
#define UART_USB_PERIPH_GPIO    SYSCTL_PERIPH_GPIOA
#define UART_USB_GPIO_BASE      GPIO_PORTA_BASE
#define UART_USB_GPIO_PIN_RX    GPIO_PIN_0
#define UART_USB_GPIO_PIN_TX    GPIO_PIN_1
#define UART_USB_GPIO_PINS      UART_USB_GPIO_PIN_RX | UART_USB_GPIO_PIN_TX

void uart_init(void)
{
    //
    // Enable GPIO port A which is used for UART0 pins.
    //
    SysCtlPeripheralEnable(UART_USB_PERIPH_UART);
    SysCtlPeripheralEnable(UART_USB_PERIPH_GPIO);
    //
    // Select the alternate (UART) function for these pins.
    //
    GPIOPinTypeUART(UART_USB_GPIO_BASE, UART_USB_GPIO_PINS);
    GPIOPinConfigure (GPIO_PA0_U0RX);
    GPIOPinConfigure (GPIO_PA1_U0TX);

    UARTConfigSetExpClk(UART_USB_BASE, SysCtlClockGet(), BAUD_RATE,
            UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
            UART_CONFIG_PAR_NONE);
    UARTFIFOEnable(UART_USB_BASE);
    UARTEnable(UART_USB_BASE);
}

/**
 * (Original Code by P.J. Bones)
 * Formats the string to send via UART and then sends it
 */
void uart_send(const char* t_buffer)
{
    // write the buffer out
    while (*t_buffer)
    {
        UARTCharPut(UART_USB_BASE, *t_buffer);
        t_buffer++;
    }
}

void uart_update(void)
{
    uint16_t target_yaw = setpoint_get_yaw();
    uint16_t actual_yaw = yaw_getDegrees();

    int16_t target_altitude = setpoint_get_altitude();
    int32_t actual_altitude = alt_getPercent(); // TODO: maybe change this to int16_t in the altitude module?
    uint8_t main_rotor_duty = 0; // TODO: Add main rotor duty
    uint8_t tail_rotor_duty = 0; // TODO: Add tail rotor duty
    uint8_t operating_mode = 0; // TODO: Add operating mode

    // format the outgoing data
    char buffer[40] = {0};
    usprintf(buffer, "Y%d\ty%d\tA%d\ta%d\tm%d\tt%d\to%d\n", target_yaw, actual_yaw, target_altitude, actual_altitude, main_rotor_duty, tail_rotor_duty, operating_mode);

    // send it
    uart_send(buffer);
}
