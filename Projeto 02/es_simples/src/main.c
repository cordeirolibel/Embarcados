#include <stdbool.h>
#include <stdint.h>
#include "inc/tm4c1294ncpdt.h" // CMSIS-Core
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h" // driverlib
#include "driverlib/gpio.h"
#include "driverlib/systick.h"

#include <stdio.h>
#include <inttypes.h>

#include "temp_driver.h"        // Driver do temporizador/contador de pulso
#include "grlib/grlib.h"        // Biblioteca gráfica
#include "cfaf128x128x16.h"     // Biblioteca para o display          
#include "system_tm4c1294ncpdt.h" // CMSIS-Core



tContext sContext;
uint32_t period = 25000000;     // hue

void initOLED (){
  cfaf128x128x16Init();
  cfaf128x128x16Clear();
  GrContextInit(&sContext, &g_sCfaf128x128x16);
  GrFlush(&sContext);
  GrContextFontSet(&sContext, g_psFontFixed6x8);
  GrContextForegroundSet(&sContext, ClrWhite);
  GrContextBackgroundSet(&sContext, ClrBlack);  
}

// As funções intToString e floatToString foram pegas de um exemplo -- talvez alterar, ou mudar para um header
static void intToString(int64_t value, char * pBuf, uint32_t len, uint32_t base, uint8_t zeros){
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

/*
static void floatToString(float value, char *pBuf, uint32_t len, uint32_t base, uint8_t zeros, uint8_t precision){
	static const char* pAscii = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	uint8_t start = 0xFF;
	if(len < 2)
		return;
	
	if (base < 2 || base > 36)
		return;
	
	if(zeros + precision + 1 > len)
		return;
	
	intToString((int64_t) value, pBuf, len, base, zeros);
	while(pBuf[++start] != '\0' && start < len); 

	if(start + precision + 1 > len)
		return;
	
	pBuf[start+precision+1] = '\0';
	
	if(value < 0)
		value = -value;
	pBuf[start++] = '.';
	while(precision-- > 0){
		value -= (uint32_t) value;
		value *= (float) base;
		pBuf[start++] = pAscii[(uint32_t) value];
	}
}
*/

void WriteResult(){
  // GrStringDraw ( contexto, string, length, x, y, opaque?)
  uint32_t leitura;
  char dispBuf[10];     // ?
  leitura = read();
  // intToString ( valor, buffer, length, base, zeros )
  intToString(leitura, dispBuf, 10, 10, 8);             // testar isso
  GrStringDraw(&sContext,(char*)dispBuf, -1, 0, 60, true);
  if (period == 25000000)
  {
    GrStringDraw(&sContext, "       Hz         ", -1, 0, 70, true);
  }
  else 
  {
    GrStringDraw(&sContext, "       kHz         ", -1, 0, 70, true);
  }
  reset();
}

void SysTick_Handler(void){
  WriteResult();
      
} // SysTick_Handler

void main(void){
  uint32_t ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                              SYSCTL_OSC_MAIN |
                                              SYSCTL_USE_PLL |
                                              SYSCTL_CFG_VCO_480),
                                              25000000); // PLL em 25MHz
  
  SysTickEnable();              // Habilita SysTick
  SysTickPeriodSet(period);    // f = 1Hz (?)   -- 2500000 = 5Hz
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ); // Habilita GPIO J (push-button SW1 = PJ0)
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ)); // Aguarda final da habilitação
    
  GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0); // push-buttons SW1
  GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

  SysTickIntEnable();   // Habilita interrupção SysTick 
  init();               // Inicializa timer (contador de pulso)
  SystemInit();         // Inicializa System (?) 
  initOLED();           // Inicializa display
  
  while(1){
    // Ver o que acontece se segurar o botão
    if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) == GPIO_PIN_0) // Testa estado do push-button SW1
      if (period == 25000000)
      {
        period = 25000;                 // Muda a escala para kHz (?)
        SysTickPeriodSet(period);
      }
      else 
      {
        period = 25000000;                 // Muda a escala para Hz (?)
        SysTickPeriodSet(period);
      }

  } // while
} // main