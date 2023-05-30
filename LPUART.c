/*
 * LPUART.c
 *
 *  Created on: May 18, 2023
 *      Author: penel
 */

#include "stm32l4a6xx.h"
#include "stm32l4xx_hal.h"

#define UART_PORT GPIOG
#define UART_PORT_EN RCC_AHB2ENR_GPIOGEN

void LPUART_init( void ){
	PWR->CR2 |= (PWR_CR2_IOSV);              // power avail on PG[15:2] (LPUART1)
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOGEN);   // enable GPIOG clock
	RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN; // enable LPUART clock bridge

	/* USER: configure GPIOG registers MODER/PUPDR/OTYPER/OSPEEDR then
	   select AF mode and specify which function with AFR[0] and AFR[1] */
	//reset pin mode to 0
	UART_PORT -> MODER &= ~(GPIO_MODER_MODE7 | GPIO_MODER_MODE8);
	UART_PORT -> MODER |= (GPIO_MODER_MODE7_1 | GPIO_MODER_MODE8_1);
	//Set pins to push-pull
	UART_PORT -> OTYPER &= ~(GPIO_OTYPER_OT7 | GPIO_OTYPER_OT8);
	//Clears PUPDR
	UART_PORT ->PUPDR   &= ~(GPIO_PUPDR_PUPD7_1 | GPIO_PUPDR_PUPD8_1);
	//Sets to High Speed
	UART_PORT->OSPEEDR |= (GPIO_OSPEEDR_OSPEED7 | GPIO_OSPEEDR_OSPEED8);
	//Setting Alternate Function
	UART_PORT -> AFR[0]  &= ((0x000F << GPIO_AFRL_AFSEL7_Pos));
	UART_PORT -> AFR[1]  &= ((0x000F << GPIO_AFRL_AFSEL0_Pos));
	UART_PORT -> AFR[0]  |= ((0x0008 << GPIO_AFRL_AFSEL7_Pos));
	UART_PORT -> AFR[1]  |= ((0x0008 << GPIO_AFRL_AFSEL0_Pos));

	LPUART1->CR1 &= ~(USART_CR1_M1 | USART_CR1_M0); // 8-bit data
	LPUART1->CR1 |= USART_CR1_UE;                   // enable LPUART1
	LPUART1->CR1 |= (USART_CR1_TE | USART_CR1_RE);  // enable xmit & recv
	LPUART1->CR1 |= USART_CR1_RXNEIE;        // enable LPUART1 recv interrupt
	LPUART1->ISR &= ~(USART_ISR_RXNE);       // clear Recv-Not-Empty flag
	/* USER: set baud rate register (LPUART1->BRR) */
	LPUART1 -> BRR = 0x22B9;
	NVIC->ISER[2] = (1 << (LPUART1_IRQn & 0x1F));   // enable LPUART1 ISR
	__enable_irq();                          // enable global interrupts

}

void LPUART_Print(const char* message) {
   uint16_t iStrIdx = 0;
   while ( message[iStrIdx] != 0 ) {
      while(!(LPUART1->ISR & USART_ISR_TXE)); // wait for empty xmit buffer
      LPUART1->TDR = message[iStrIdx];       // send this character
	iStrIdx++;                             // advance index to next char
   }
}

void LPUART_ESC_Print(const char* message){
	LPUART_Print("\x1B[");
//	LPUART_Print("[");
	LPUART_Print(message);
}

void LPUART_write(){
	LPUART_ESC_Print("3B");
	LPUART_ESC_Print("5C");
	LPUART_Print("All good students read the");
	LPUART_ESC_Print("1B");
	LPUART_ESC_Print("21D");
	LPUART_ESC_Print("5m");
	LPUART_Print("Reference Manual");
	LPUART_ESC_Print("H");
	LPUART_ESC_Print("0m");
	LPUART_Print("Input");
}

void LPUART1_IRQHandler( void ) {
   uint8_t charRecv;
   if (LPUART1->ISR & USART_ISR_RXNE) {
      charRecv = LPUART1->RDR;
      switch ( charRecv ) {
	   case 'R':
		  LPUART_ESC_Print("31m");
	      break;
	   case 'G' :
		  LPUART_ESC_Print("32m");
		  break;
	   case 'B' :
		  LPUART_ESC_Print("34m");
		  break;
	   case 'W' :
		  LPUART_ESC_Print("37m");
		  break;
	   default:
	      while( !(LPUART1->ISR & USART_ISR_TXE) );    // wait for empty TX buffer
		  LPUART1->TDR = charRecv;  // echo char to terminal
	}  // end switch
   }
}
