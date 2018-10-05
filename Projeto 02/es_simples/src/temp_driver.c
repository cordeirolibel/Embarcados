#include <stdbool.h>
#include <stdint.h>
#include "inc/tm4c1294ncpdt.h"
#include "temp_driver.h"

void init()
{
      /// CONFIGURA GPIO PA0

  // habilita clock do GPIO_PORT_A
  SYSCTL_RCGCGPIO_R |= 0x01;
  while(!(SYSCTL_PRGPIO_R & 0x01));
  
//// como configurar um pino para Digital Input (Timer CCP) -> datasheet processador pg 754 e tabela no começo do capítulo de GPIO
  
  // configura PA0 como um pino de IO digital
  GPIO_PORTA_AHB_DEN_R |= 0x01;
  
  // desabilita a saida open drain para o pino PA0
  GPIO_PORTA_AHB_ODR_R &= ~0x01;
  
  // configura PA0 como controlado por hardware alternativo
  GPIO_PORTA_AHB_AFSEL_R |= 0x01;
  
  // configura PA0 como T0_CCP0      [configMagicNumber] << offsetPino
  GPIO_PORTA_AHB_PCTL_R |= (0x03) << 0;
    
      /// CONFIGURA TIMER0A

  // habilitando clock do TIMER_A0
  SYSCTL_RCGCTIMER_R |= 0x01;
  while(!(SYSCTL_PRTIMER_R & 0x01));
  
//// como configurar um timer como input-edge-count: tabela 13-2 pg 957,  passo-a-passo da pg 972

  // desabilita Timer0
  TIMER0_CTL_R &= ~0x0101;
  
  // configura como dois timers de 16b independentes
  TIMER0_CFG_R = 0x04;
  
  // configura o timer0A como edge count mode
  TIMER0_TAMR_R &= ~0x04;
  
  // configura timer0A como um capture timer
  TIMER0_TAMR_R |= 0x03;
  
  // configura as bordas de contagem para ambas
  TIMER0_CTL_R |= (0x03 << 2);
  
  // configura direção de contagem como up
  TIMER0_TAMR_R |= 0x10;
  
  /*
Tecnicamente, daqui em diante é gambiarra: o datasheet do módulo pede que o limite de contagem,
o valor em PreescalerMatch + Match seja menor que o valor que será inserido ao ocorrer um match
Preescaler + Interval Loader.

Isso ocorre porque esse modo de contagem edge-input-count é para contar um certo número, conhecido,
de pulsos no pino de CCP.

problema similar num processador parecido (tm4c123gh6pm)
https://e2e.ti.com/support/microcontrollers/other/f/908/p/302816/1055214 
  */
  
  // limite da contagem como fundo de escala
  TIMER0_TAMATCHR_R |= 0xFFFF;
  
  // limite da contagem como fundo de escala
  TIMER0_TAPMR_R |= 0xFF;
  
  // habilita Timer0A
  TIMER0_CTL_R |= 0x01;
}

uint32_t read()
{
  return TIMER0_TAR_R;
}

void reset()
{
  // desabilita Timer0
  TIMER0_CTL_R &= ~0x0101;
  
  // configura reload value em 0
  TIMER0_TAILR_R = 0;
  
  // habilita Timer0A
  TIMER0_CTL_R |= 0x01;
}


