/*
 * timer6.c
 *
 *  Created on: Jul 21, 2024
 *      Author: ahmed
 */
/********
 *
 * What i do in this file
 *
 * 		1) 	PAO interrupt wake-up:
 * 			Done in 	File:    main.c
 * 			       	Function:	 void MX_GPIO_Init(void)
 * 			       	  config: 	 1. disable port A to go to sleep mode
 * 			       	  				as it is the event that will wake up the CPU
 * 			       	  					__HAL_RCC_GPIOA_CLK_SLEEP_DISABLE();
 * 			       	  			 2. Set The Mode to interrupt FALLING
 * 			       	  			   		GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
 * 			       	  			   		and the pin PULL GPIO_InitStruct.Pull = GPIO_PULLUP;
 *
		2) 	purple signal in state low:
			DONE in     File:	timer6.c
					function:	EXTI0_IRQHandler(void)
					  config:   when the interrupt button pressed the EXTI will RESET PB15


		3) 	start timer 6 10 mS:
						File:	timer6.c
					function:	EXTI0_IRQHandler(void)
					  config:   when the interrupt button pressed the TIM16 will Start

		4) 	timer finish purple in state high:

 *						File:	timer6.c
					function:	TIM16_IRQHandler(void)
					  config:   when the TIM16 interrupt fire after 10ms the TIM16 will Stop and SET PB15 and go to Sleep mode
 *
 *
 *
 *
 *
 */
#include "timer6.h"

extern TIM_HandleTypeDef htim16;
volatile uint32_t elapsed_time_ms = 0; // Variable to store elapsed time in ms

void MX_TIM16_Init(void) {
  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;

  /* Assuming TIM16 clock source is either 4 MHz or 2 MHz */
  /* If you use the Clock source from function
   * SystemClock_Config   		--> config clk to 4MHz
   * SystemClock_Config_2M 		--> config clk to 2MHz
   * */
  uint32_t tim16_clk = HAL_RCC_GetPCLK1Freq(); // Get the TIM16 clock frequency

  if (tim16_clk == 4000000) {
    // 4 MHz clock source
    htim16.Init.Prescaler = 4999; // Prescaler value (4 MHz / 4 = 1 MHz)
    htim16.Init.Period = 8 - 1; // Auto-reload value (1 MHz / 1000 = 1 kHz or 1 ms)
  } else if (tim16_clk == 2000000) {
    // 2 MHz clock source
    htim16.Init.Prescaler = 4999; // Prescaler value (2 MHz / 2 = 1 MHz)
    htim16.Init.Period = 4 - 1; // Auto-reload value (1 MHz / 1000 = 1 kHz or 1 ms)
  } else {
    // Handle unexpected clock source frequencies
    Error_Handler();
  }

  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  if (HAL_TIM_Base_Init(&htim16) != HAL_OK) {
    Error_Handler();
  }

  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */
}

void TIM16_Start(void)
{
    if (HAL_TIM_Base_Start_IT(&htim16) != HAL_OK)
    {
        Error_Handler();
    }
}

void TIM16_Stop(void)
{
    if (HAL_TIM_Base_Stop_IT(&htim16) != HAL_OK)
    {
        Error_Handler();
    }
}


void TIM16_IRQHandler(void) {
  /* USER CODE BEGIN TIM16_IRQn 0 */

  /* USER CODE END TIM16_IRQn 0 */
  HAL_TIM_IRQHandler(&htim16);
  /* USER CODE BEGIN TIM16_IRQn 1 */
  //TIM16->SR &= ~TIM_SR_UIF; // Clear Update Interrupt flag
  HAL_GPIO_WritePin(GPIOB, ZB_STATE_GPIO_Pin, GPIO_PIN_SET);
  __WFI();
  /* USER CODE END TIM16_IRQn 1 */
}


uint32_t TIM16_CalculateTimeInMs(void) {
    uint32_t counter_value = TIM16->CNT;
    // Convert the counter value to milliseconds
    // Since the timer ticks every 0.25 microseconds:
    uint32_t time_in_us = counter_value * 0.25; // Counter value * 0.25 microseconds per tick
    uint32_t time_in_ms = time_in_us / 1000;    // Convert microseconds to milliseconds
    return time_in_ms;
}

void EXTI0_IRQHandler(void){

	HAL_GPIO_EXTI_IRQHandler(G13_STATE_GPIO_Pin);
	HAL_GPIO_WritePin(GPIOB, ZB_STATE_GPIO_Pin, GPIO_PIN_RESET);
	TIM16_Start();
}
