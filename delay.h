/*
 * delay.h
 *
 *  Created on: May 30, 2023
 *      Author: penel
 */

#ifndef INC_DELAY_H_
#define INC_DELAY_H_

#include "stm32l4xx_hal.h"

void SysTick_Init();
void delay_us(const uint32_t time_us);

#endif

