/*
 * ADC.c
 *
 *  Created on: May 23, 2023
 *      Author: penel
 */

#include "main.h"
#include "ADC.h"
#include "stm32l4a6xx.h"
#include "stm32l4xx_hal.h"

#define DELAY 40000
#define TOTAL 20

void ADC_init(void){
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;         // turn on clock for ADC
	// power up & calibrate ADC
	ADC123_COMMON->CCR |= (1 << ADC_CCR_CKMODE_Pos); // clock source = HCLK/1
	ADC1->CR &= ~(ADC_CR_DEEPPWD);             // disable deep-power-down
	ADC1->CR |= (ADC_CR_ADVREGEN);             // enable V regulator - see RM 18.4.6
	delay_us(20);                              // wait 20us for ADC to power up
	ADC1->DIFSEL &= ~(ADC_DIFSEL_DIFSEL_5);    // PA0=ADC1_IN5, single-ended
	ADC1->CR &= ~(ADC_CR_ADEN | ADC_CR_ADCALDIF); // disable ADC, single-end calib
	ADC1->CR |= ADC_CR_ADCAL;                  // start calibration
	while (ADC1->CR & ADC_CR_ADCAL) {;}        // wait for calib to finish
	// enable ADC
	ADC1->ISR |= (ADC_ISR_ADRDY);              // set to clr ADC Ready flag
	ADC1->CR |= ADC_CR_ADEN;                   // enable ADC
	while(!(ADC1->ISR & ADC_ISR_ADRDY)) {;}    // wait for ADC Ready flag
	ADC1->ISR |= (ADC_ISR_ADRDY);              // set to clr ADC Ready flag
	// configure ADC sampling & sequencing
	ADC1->SQR1  |= (5 << ADC_SQR1_SQ1_Pos);    // sequence = 1 conv., ch 5
	ADC1->SMPR1 |= (1 << ADC_SMPR1_SMP5_Pos);  // ch 5 sample time = 6.5 clocks
	ADC1->CFGR  &= ~( ADC_CFGR_CONT  |         // single conversion mode
	                  ADC_CFGR_EXTEN |         // h/w trig disabled for s/w trig
	                  ADC_CFGR_RES   );        // 12-bit resolution
	// configure & enable ADC interrupt
	ADC1->IER |= ADC_IER_EOCIE;                // enable end-of-conv interrupt
	ADC1->ISR &= ~(ADC_ISR_EOC);               // clear EOC flag
	NVIC->ISER[0] = (1<<(ADC1_2_IRQn & 0x1F)); // enable ADC interrupt service
	__enable_irq();                            // enable global interrupts
	// configure GPIO pin PA0
	RCC->AHB2ENR  |= (RCC_AHB2ENR_GPIOAEN);    // connect clock to GPIOA
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL0);      // clear alt. function select
	GPIOA->AFR[0] |= (7 << GPIO_AFRL_AFSEL0_Pos); // choose AF 7 (PA0=ADC1_IN5)
	GPIOA->MODER  |= (GPIO_MODER_MODE0);	       // analog mode for PA0 (set MODER last)

	ADC1->CR |= ADC_CR_ADSTART;                // start 1st conversion
}

uint16_t MAX(uint16_t arr[]){
	uint16_t pos = 0;
	uint16_t max = arr[pos];
	while (pos < TOTAL){
		if (arr[pos] > max){
			max = arr[pos];
		}
		pos++;
	}
	return max;
}

uint16_t MIN(uint16_t arr[]){
	uint16_t pos = 0;
	uint16_t min = arr[pos];
	while (pos < TOTAL){
		if (arr[pos] < min){
			min = arr[pos];
		}
		pos++;
	}
	return min;
}

uint32_t AVG(uint16_t arr[]){
	uint16_t pos = 0;
	uint16_t val = 0;
	while (pos < TOTAL){
		val += arr[pos];
		pos++;
	}
	uint32_t avg = val/TOTAL;
	return avg;
}

float CALIBRATE(uint16_t num){
	float res = (821*num-5060)/1000000.0;
	return res;
}

void stringConversion(float num, char* string, int decimalPlace){
	int i = 0;
	int integer = (int)num;
	int decimal = num - integer;

	int j = i;
	do{
		string[j++] = integer % 10 + '0';
		integer /= 10;
	}while(integer);

	for (int k = i; k < j/2; k++){
		char temp = string[k];
		string[k] = string[j-k-1];
		string[j-k-1] = temp;
	}
	if (decimalPlace > 0) string[j++] = '.';
	for (int k = 0; k < decimalPlace; k++) string [j++] = (int)(decimal *= 10) %10 + '0';

	string[j] = '0';
}

void ADC_trigger(){
    ADC1->CR |= ADC_CR_ADSTART;
}
