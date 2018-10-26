/*
 * system_tm4c1294.h
 *
 *  Created on: 26 de out de 2018
 *      Author: Mateus
 */

#ifndef SYSTEM_TM4C1294_H_
#define SYSTEM_TM4C1294_H_

#include <stdint.h>

extern uint32_t SystemCoreClock;

extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);


#endif /* SYSTEM_TM4C1294_H_ */
