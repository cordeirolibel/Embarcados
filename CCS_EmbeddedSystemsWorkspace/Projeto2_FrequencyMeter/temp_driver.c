#include <stdbool.h>
#include <stdint.h>
#include "tm4c1294ncpdt.h"
#include "temp_driver.h"



void init()
{
      /// CONFIGURA GPIO PA0

  // habilita clock do GPIO_PORT_L
  SYSCTL_RCGCGPIO_R |= 0x01 << 10;
  while(!(SYSCTL_PRGPIO_R & (0x01 << 10)));
  
//// como configurar um pino para Digital Input (Timer CCP) -> datasheet processador pg 754 e tabela no começo do capítulo de GPIO
  
  // configura PL4 como um pino de IO digital
  GPIO_PORTL_DEN_R |= 0x10;
  
  // desabilita a saida open drain para o pino PL4
  GPIO_PORTL_ODR_R &= ~0x10;
  
  // configura PL4 como controlado por hardware alternativo
  GPIO_PORTL_AFSEL_R |= 0x10;
  
  // configura PL4 como T0_CCP0      [configMagicNumber] << offsetPino
  GPIO_PORTL_PCTL_R |= (0x03) << (4 * 4);
    
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
  
  // ambos os registradores [TAILR,TAPR] devem maiores que os de limite
  TIMER0_TAILR_R = 0xFFFF;
  TIMER0_TAPR_R = 0xFF;
  
  // limite da contagem como fundo de escala
  TIMER0_TAMATCHR_R = 0x0000;
  
  // limite da contagem como fundo de escala
  TIMER0_TAPMR_R = 0x01;
  
  // habilita Timer0A
  TIMER0_CTL_R |= 0x01;
}

uint32_t read()
{
    return (TIMER0_TAV_R & 0x0000FFFF);
}

void reset()
{
    // limpa TAR no próximo clock
    TIMER0_TAV_R = 0x00000000;
}


