/*
 * LPUART.h
 *
 *  Created on: May 18, 2023
 *      Author: penel
 */

#ifndef INC_UART_H_
#define INC_UART_H_
#include "stm32l4xx_hal.h"
#include <stdio.h>


void LPUART_init( void );
void LPUART_Print( const char* );
void LPUART1_IRQHandler( void );
void LPUART_ESC_Print( const char*);
void LPUART_write( void );
void LPUART_moveCursor(int spaces, int lines);

#endif /* INC_UART_H_ */
