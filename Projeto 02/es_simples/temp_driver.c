 #include <stdbool.h>
#include <stdint.h>
#include "inc/tm4c1294ncpdt.h" // CMSIS-Core
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h" // driverlib
#include "driverlib/gpio.h"
#include "driverlib/systick.h"

// Pode usar? -- Aparentemente pode
#include "pin_map.h" 
#include "timer.h"


void initCount()
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); // Habilita GPIO A 
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)); // Aguarda final da habilitação
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); // Habilita Timer0
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)) // Aguarda final da habilitação
  
  TimerDisable(TIMER0_BASE,TIMER_A); // Desativa TimerA
  GPIOPinTypeTimer(GPIO_PORTA_BASE, GPIO_PIN_3); // Configura pinos para serem usados pelo periférico Timer
  GPIOPinConfigure(GPIO_PA0_T0CCP0); // Configura PA0 - (T0CCP0)
  
  TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_COUNT_UP); // Configura como edge counter
  TimerControlEvent(TIMER0_BASE, TIMER_A, TIMER_EVENT_POS_EDGE); // Configura para contar POSITIVE EDGES
  
}

void enableCount()
{
  TimerLoadSet(TIMER0_BASE, TIMER_A, 0); // Seta para iniciar com 0
  TimerEnable(TIMER0_BASE, TIMER_A); // Inicia Timer A como contador de rising edges
}

uint32_t readCount()
{
  return (TimerValueGet(TIMER0_BASE,TIMER_A)); // Lê o valor atual do timer
}

void countClear()
{
  TimerLoadSet(TIMER0_BASE, TIMER_A, 0x00); // Seta o valor do timer para 0 (talvez tenha que parar o timer antes)
}


