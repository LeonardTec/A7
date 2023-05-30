/*
 * ADC.h
 *
 *  Created on: May 23, 2023
 *      Author: penel
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

void ADC_init(void);
int delay_us(int);
int MAX(int arr[]);
int MIN(int arr[]);
int AVG(int arr[]);
float CALIBRATE(uint16_t num);
void stringConversion(float num, char* string, int decimalPlace);
void ADC_trigger(void);

#endif /* INC_ADC_H_ */
