/*
 * displayHW.c
 *
 *  Created on: 24 de out de 2018
 *      Author: DevOp
 */

#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_memmap.h"
#include "inc/tm4c1294ncpdt.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"

#include "displayHW.h"

#define RST_PORT    GPIO_PORTL_BASE
#define RST_PIN     GPIO_PIN_3
#define CS_PORT     GPIO_PORTN_BASE
#define CS_PIN      GPIO_PIN_2

void configDisplayHW(void)
{
    // Display config pins
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD));

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL));

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));

    // SPI pins
    GPIOPinTypeSSI(GPIO_PORTD_AHB_BASE,GPIO_PIN_1|GPIO_PIN_3);
    GPIOPinConfigure(GPIO_PD1_SSI2XDAT0);   // MOSI
    GPIOPinConfigure(GPIO_PD3_SSI2CLK);     // SCLK

    // GPIO pins
    GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE,GPIO_PIN_3);   // RST
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE,GPIO_PIN_2);   // CS

    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2); // Habilita SSI2
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_SSI2)); // Aguarda final da habilitação

    SSIConfigSetExpClk(SSI2_BASE, 120e6, SSI_FRF_MOTO_MODE_1, SSI_MODE_MASTER, 1e6, 8+1);
    SSIEnable(SSI2_BASE);

    // Reset Cycle
    GPIOPinWrite(RST_PORT, RST_PIN, ~RST_PIN);
    for(uint8_t cnt = 0; cnt < 255; cnt++);
    GPIOPinWrite(RST_PORT, RST_PIN, RST_PIN);

}
