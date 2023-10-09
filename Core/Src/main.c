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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>
#include "mylibs/shell.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */



int motor_set_speed = 0,motor_current_speed=0;

int start_PWM()
{//Start and init the PWM to speed = 0;

	int speed_stopped = __HAL_TIM_GET_AUTORELOAD(&htim1)/2;
	motor_set_speed = 0;
	motor_current_speed = 0;
	TIM1->CCR1 = speed_stopped;
	TIM1->CCR2 = speed_stopped;

	if(HAL_OK == HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1))
	{
		if(HAL_OK == HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2))
		{
			if(HAL_OK == HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1))
			{
				if(HAL_OK == HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2))
				{
					return SUCCESS;
				}
			}
		}
	}
	return ERROR;
}
int stop_PWM()
{
	/*
	if (HAL_OK ==(
			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1)&&
			HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1)&&
			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2)&&
			HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2)))
	{
		return SUCCESS;
	}else{
		return ERROR;
	}
	*/
	if(HAL_OK == HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1))
	{
		if(HAL_OK == HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2))
		{
			if(HAL_OK == HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1))
			{
				if(HAL_OK == HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2))
				{
					return SUCCESS;
				}
			}
		}
	}
	return ERROR;
}

int set_PWM(int new_speed)
{
	if((new_speed>100) || (new_speed<-100))
	{
		return ERROR;
	}else{
		const int ccr_size = __HAL_TIM_GET_AUTORELOAD(&htim1);
		int ccr_size_div_2 = ccr_size/2;
		int ccr_U_value,ccr_V_value;
		float f_speed = (float)new_speed/100;
		if(new_speed >0)
		{//sens de marche horaire
			ccr_U_value = ccr_size_div_2+(f_speed*ccr_size_div_2);
			ccr_V_value = ccr_size-ccr_U_value;
		}
		else
		{//sens de marche anti_horraire
			ccr_V_value = ccr_size_div_2+(fabs(f_speed)*ccr_size_div_2);
			ccr_U_value = ccr_size-ccr_V_value;
		}
		TIM1->CCR1 = ccr_U_value;
		TIM1->CCR2 = ccr_V_value;
		return SUCCESS;
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC2_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
	Shell_Init();
	//initialiaze PWM
	//start_PWM();
	//HAL_TIM_Base_Start(&htim1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
		Shell_Loop();
		if(motor_current_speed != motor_set_speed)
		{
			if (motor_current_speed < motor_set_speed)
			{
				motor_current_speed++;
			}else{
				motor_current_speed--;
			}
			set_PWM(motor_current_speed);
			HAL_Delay(50);
		}
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV6;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
