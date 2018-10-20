#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_memmap.h"
#include "inc/tm4c1294ncpdt.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/pin_map.h"

#include "temp_driver.h"

#define NULL 0


volatile char buffer[12];
volatile uint32_t leitura = 0;
volatile uint8_t sysTickCnt = 0;

static void intToString(int64_t value, volatile char *pBuf, uint32_t len, uint32_t base, uint8_t zeros);

void SysTick_Handler(void)
{
    sysTickCnt++;

    if(sysTickCnt >= 12)
    {
        uint8_t cnt = 0;
        leitura = read() / 2;
        intToString(leitura,buffer,12,10,0);
        while(buffer[cnt] && cnt < 12)
        {
            UARTCharPut(UART0_BASE,buffer[cnt++]);
        }
        UARTCharPut(UART0_BASE,'\n');
        UARTCharPut(UART0_BASE,'\r');
        reset();
        sysTickCnt = 0;
    }
} // SysTick_Handler



void main(void)
{
  uint32_t ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                              SYSCTL_OSC_MAIN |
                                              SYSCTL_USE_PLL |
                                              SYSCTL_CFG_VCO_480),
                                              120000000); // PLL em 120MHz


  SysTickEnable();              // Habilita SysTick
  SysTickPeriodSet(10000000);   // f = 1/12Hz


  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));

  GPIOPinTypeUART(GPIO_PORTA_AHB_BASE,GPIO_PIN_0|GPIO_PIN_1);
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);

  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0));

  UARTConfigSetExpClk(UART0_BASE,ui32SysClock,115200,UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE);
  UARTCharPut(UART0_BASE,'t');
  UARTCharPut(UART0_BASE,'e');
  UARTCharPut(UART0_BASE,'s');
  UARTCharPut(UART0_BASE,'t');
  UARTCharPut(UART0_BASE,'e');
  UARTCharPut(UART0_BASE,'\n');

  SysTickIntEnable();   // Habilita interrupção SysTick
  init();               // Inicializa timer (contador de pulso)

  while(1)
  {
  } // while

  return;
} // main



static void intToString(int64_t value, volatile char *pBuf, uint32_t len, uint32_t base, uint8_t zeros)
{
    static const char* pAscii = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    bool n = false;
    int pos = 0, d = 0;
    int64_t tmpValue = value;

    // the buffer must not be null and at least have a length of 2 to handle one
    // digit and null-terminator
    if (pBuf == NULL || len < 2)
            return;

    // a valid base cannot be less than 2 or larger than 36
    // a base value of 2 means binary representation. A value of 1 would mean only zeros
    // a base larger than 36 can only be used if a larger alphabet were used.
    if (base < 2 || base > 36)
            return;

    if (zeros > len)
        return;

    // negative value
    if (value < 0)
    {
            tmpValue = -tmpValue;
            value    = -value;
            pBuf[pos++] = '-';
            n = true;
    }

    // calculate the required length of the buffer
    do {
            pos++;
            tmpValue /= base;
    } while(tmpValue > 0);


    if (pos > len)
            // the len parameter is invalid.
            return;

    if(zeros > pos){
        pBuf[zeros] = '\0';
        do{
            pBuf[d++ + (n ? 1 : 0)] = pAscii[0];
        }
        while(zeros > d + pos);
    }
    else
        pBuf[pos] = '\0';

    pos += d;
    do {
            pBuf[--pos] = pAscii[value % base];
            value /= base;
    } while(value > 0);
}
