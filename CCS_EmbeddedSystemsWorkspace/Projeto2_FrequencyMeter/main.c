#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_memmap.h"
#include "inc/tm4c1294ncpdt.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/pin_map.h"
#include "SystemDefaults.h"

#include "temp_driver.h"
#include "display.h"

#include "grlib/grlib.h"
#include "cfaf128x128x16.h"

#define NULL 0

// leitura
volatile uint32_t leitura = 0;
volatile uint8_t sysTickCnt = 0;
volatile bool newReadFlag = false;

static void intToString(int64_t value, volatile char *pBuf, uint32_t len, uint32_t base, uint8_t zeros);
static void initMcu(void);
static void sendUART(char *string);


tContext sContext;


void SysTick_Handler(void)
{
    sysTickCnt++;

    if(sysTickCnt >= 12)
    {
        leitura = read() / 2;
        reset();
        sysTickCnt = 0;
        newReadFlag = true;
    }
} // SysTick_Handler

void main(void)
{
    uint32_t period = LOW_FREQ;
    uint32_t delayCount = 0;
    uint32_t userSwitchState = 0;
    char buffer[12];

    initMcu();

    userSwitchState = GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0);

    sendUART("LeFreq\n\r");
    GrStringDraw(&sContext,"Frequency Meter",-1,0,0,true);

    while(1)
    {
        if(userSwitchState != GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0))
        {
            userSwitchState = GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0);

            for(uint16_t cnt = 0; cnt < DEBOUNCE;cnt++);

            if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) != GPIO_PIN_0)
            {
                if (period == LOW_FREQ)
                {
                    period = HIGH_FREQ;                 // Muda a escala para kHz (?)

                    SysTickPeriodSet(period);
                }
                else
                {
                    period = LOW_FREQ;                 // Muda a escala para Hz (?)

                    SysTickPeriodSet(period);
                }
            }
        } // if userSw

        if(newReadFlag)
        {
            if(period == LOW_FREQ)
            {
                intToString(leitura,buffer,12,10,0);

                sendUART(buffer);
                sendUART(" Hz\n\r");
                GrStringDraw(&sContext,"F:            ",-1,0,30,true);
                GrStringDraw(&sContext,buffer,-1,12,30,true);
                GrStringDraw(&sContext,"Hz",-1,60,30,true);
            }

            else
            {
                if(delayCount >= 1000)
                {
                    intToString(leitura,buffer,12,10,0);

                    sendUART(buffer);
                    sendUART(" kHz\n\r");
                    GrStringDraw(&sContext,"F:            ",-1,0,30,true);
                    GrStringDraw(&sContext,buffer,-1,12,30,true);
                    GrStringDraw(&sContext,"kHz",-1,60,30,true);

                    delayCount = 0;
                }

                else
                    delayCount++;

            }
            newReadFlag = false;
        } // if newRead
    } // while endless

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


static void initMcu(void)
{
    // Main Clock configuration
    uint32_t ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                                  SYSCTL_OSC_MAIN |
                                                  SYSCTL_USE_PLL |
                                                  SYSCTL_CFG_VCO_480),
                                                  120000000); // PLL em 120MHz

    // SysTick timer configuration
    SysTickEnable();              // Habilita SysTick
    SysTickPeriodSet(LOW_FREQ);   // f = 1/12Hz

    SysTickIntEnable();   // Habilita interrupção SysTick

    // TimerA0 config
    init();               // Inicializa timer (contador de pulso)

    // BackChannel UART config
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));

    GPIOPinTypeUART(GPIO_PORTA_AHB_BASE,GPIO_PIN_0|GPIO_PIN_1);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0));

    UARTConfigSetExpClk(UART0_BASE,ui32SysClock,115200,UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE);

    // GPIO User Button configuration
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ); // Habilita GPIO J (push-button SW1 = PJ0)
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ)); // Aguarda final da habilitação

    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0); // push-buttons SW1
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // Display Configuration
    //initDisplay();


    cfaf128x128x16Init();
    GrContextInit(&sContext, &g_sCfaf128x128x16);
    GrFlush(&sContext);
    GrContextFontSet(&sContext, g_psFontFixed6x8);
    GrContextForegroundSet(&sContext, ClrWhite);
    GrContextBackgroundSet(&sContext, 0x3840B0);

    return;
}

static void sendUART(char *string)
{
    while(*string)
    {
        UARTCharPut(UART0_BASE,*string);
        string++;
    }
}
