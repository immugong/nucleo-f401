/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "cmsis_os.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
// 核心区分宏：1=VL53L7CX，0=VL53L8CX，一键切换
#define USE_VL53L7 0

#if USE_VL53L7
#include "vl53l7cx_api.h"
#else
#include "vl53l8cx_api.h"
#endif
/* USER CODE END Includes */
/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */
/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define is_interrupt 0 /*is_interrupt = 1 => 中断获取数据, = 0 => 轮询获取数据 */
// 统一I2C地址宏，适配两款传感器
#if USE_VL53L7
#define VL53LX_DEFAULT_I2C_ADDRESS VL53L7CX_DEFAULT_I2C_ADDRESS
#else
#define VL53LX_DEFAULT_I2C_ADDRESS VL53L8CX_DEFAULT_I2C_ADDRESS
#endif
/* USER CODE END PD */
/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */
/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint8_t 				status, loop, isAlive, isReady;
volatile int IntCount;
uint8_t p_data_ready;
// 条件定义传感器配置/结果结构体，适配L7/L8
#if USE_VL53L7
VL53L7CX_Configuration 	Dev;
VL53L7CX_ResultsData 	Results;
#else
VL53L8CX_Configuration 	Dev;
VL53L8CX_ResultsData 	Results;
#endif
uint8_t resolution;
uint16_t idx;
/* USER CODE END PV */
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */
// 声明传感器初始化函数
uint8_t VL53LX_Sensor_Init(void);
/* USER CODE END PFP */
/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin==INT_C_Pin)
	{
		IntCount++;
	}
}

// 独立封装：VL53LX传感器初始化函数（适配L7/L8）
uint8_t VL53LX_Sensor_Init(void)
{
	uint8_t init_status = 0;
	// 配置传感器I2C地址
	Dev.platform.address = VL53LX_DEFAULT_I2C_ADDRESS;
	
	// 1. 检测传感器是否在线
	#if USE_VL53L7
	init_status = vl53l7cx_is_alive(&Dev, &isAlive);
	#else
  VL53L8CX_Reset_Sensor(&(Dev.platform));
	init_status = vl53l8cx_is_alive(&Dev, &isAlive);
	#endif
	if(!isAlive || init_status != 0)
	{
		#if USE_VL53L7
		printf("VL53L7CX not detected at address (0x%x)\r\n", Dev.platform.address);
		#else
		printf("VL53L8CX not detected at address (0x%x)\r\n", Dev.platform.address);
		#endif
		return 0xFF; // 传感器未检测到，返回错误码
	}
	
	// 2. 打印初始化提示
	#if USE_VL53L7
	printf("VL53L7CX Sensor initializing, please wait few seconds\r\n");
	#else
	printf("VL53L8CX Sensor initializing, please wait few seconds\r\n");
	#endif
	
	// 3. 传感器底层初始化
	#if USE_VL53L7
	init_status = vl53l7cx_init(&Dev);
	#else
	init_status = vl53l8cx_init(&Dev);
	#endif
	if(init_status != 0)
	{
		#if USE_VL53L7
		printf("VL53L7CX ULD Loading failed, err code: %d\r\n", init_status);
		#else
		printf("VL53L8CX ULD Loading failed, err code: %d\r\n", init_status);
		#endif
		return init_status; // 返回初始化错误码
	}
	
	// 4. 打印初始化成功信息及API版本
	#if USE_VL53L7
	printf("VL53L7CX ULD ready ! (Version : %s)\r\n", VL53L7CX_API_REVISION);
	// 5. 配置L7测距参数：2Hz频率 + 连续测距模式
	init_status = vl53l7cx_set_ranging_frequency_hz(&Dev, 2);
	if(init_status == 0)
		init_status = vl53l7cx_set_ranging_mode(&Dev, VL53L7CX_RANGING_MODE_CONTINUOUS);
	#else
	printf("VL53L8CX ULD ready ! (Version : %s)\r\n", VL53L8CX_API_REVISION);
	// 5. 配置L8测距参数：2Hz频率 + 连续测距模式
	init_status = vl53l8cx_set_ranging_frequency_hz(&Dev, 2);
	if(init_status == 0)
		init_status = vl53l8cx_set_ranging_mode(&Dev, VL53L8CX_RANGING_MODE_CONTINUOUS);
	#endif
	
	// 6. 测距参数配置错误判断
	if(init_status != 0)
	{
		#if USE_VL53L7
		printf("VL53L7CX Param Config failed, err code: %d\r\n", init_status);
		#else
		printf("VL53L8CX Param Config failed, err code: %d\r\n", init_status);
		#endif
	}
	else
	{
		printf("VL53LX Ranging Param Config Success !\r\n");
	}
	
	return init_status; // 返回最终初始化状态（0=成功，非0=失败）
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
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_USART6_UART_Init();
  MX_I2C2_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
  int set_Value = 0;
  uint8_t sensor_init_ret = 0;
  
  // 调用独立的传感器初始化函数
  sensor_init_ret = VL53LX_Sensor_Init();
  if(sensor_init_ret != 0)
  {
  	printf("VL53LX Sensor Init Total Failed ! Ret: %d\r\n", sensor_init_ret);
  	return sensor_init_ret; // 初始化失败，退出主函数
  }
  printf("VL53LX Sensor Init All Success !\r\n");
  /* USER CODE END 2 */
  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();
  /* Start scheduler */
  osKernelStart();
  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}
/* USER CODE BEGIN 4 */
/* USER CODE END 4 */
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
#ifdef USE_FULL_ASSERT
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