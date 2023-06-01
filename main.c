/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#include "main.h"
#include "ADC.h"
#include "LPUART.h"

void SystemClock_Config(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();

  LPUART_init();
  ADC_init();

  int ADC_sample[20];
  int ADC_new;
  int ADC_newFlag;

  int sampleCount = 0;

  while (1)
  {
	  if(ADC_newFlag){
		  ADC_newFlag = 0;
		  ADC_sample[sampleCount++] = ADC_newFlag;
		  ADC_trigger();
	  }
	  if (sampleCount==20){
		  sampleCount = 0;

		  float countVal = 0;
		  char countVal_str[4];
		  float voltVal = 0;
		  char voltVal_str[5];

		  LPUART_ESC_Print("2J");
		  LPUART_ESC_Print("H");
		  LPUART_Print("ADC counts volts");

		  for (int i=0; i<3; i++){
			  LPUART_moveCursor(2+i, 1);
			  switch(i){
			  case 0:
				  LPUART_Print ("MIN ");
				  countVal = MIN(ADC_sample);
				  break;

			  case 1:
				  LPUART_Print ("MAX ");
				  countVal = MAX(ADC_sample);
				  break;

			  case 2:
				  LPUART_Print("AVG ");
				  countVal = AVG(ADC_sample);
				  break;
			  }

		  }

	  }
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif
