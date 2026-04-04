/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "vl53l7cx_api.h"
#include "PS2.h"
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
/* USER CODE BEGIN Variables */

extern uint8_t 				status, loop, isAlive, isReady;
extern volatile int IntCount;
extern uint8_t p_data_ready;
extern VL53L7CX_Configuration 	Dev;
extern VL53L7CX_ResultsData 	Results;
extern uint8_t resolution, isAlive;
extern uint16_t idx;

uint8_t test[]= {0x01,0x42};
uint8_t data[] = {0x00};
extern uint8_t PS2data[];
//extern uint8_t but;
extern uint16_t XY[4];
extern uint8_t All_But[];
uint8_t i1;

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId PS2TaskHandle;
osThreadId ToFTaskHandle;
osThreadId DebugTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void get_data_by_polling(VL53L7CX_Configuration *p_dev);
void get_data_by_interrupt(VL53L7CX_Configuration *p_dev);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartPS2Task(void const * argument);
void StratToFTask(void const * argument);
void StartDebugTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of PS2Task */
  //osThreadDef(PS2Task, StartPS2Task, osPriorityIdle, 0, 128);
  //PS2TaskHandle = osThreadCreate(osThread(PS2Task), NULL);

  /* definition and creation of ToFTask */
  osThreadDef(ToFTask, StratToFTask, osPriorityIdle, 0, 128);
  ToFTaskHandle = osThreadCreate(osThread(ToFTask), NULL);

  /* definition and creation of DebugTask */
  osThreadDef(DebugTask, StartDebugTask, osPriorityIdle, 0, 128);
  DebugTaskHandle = osThreadCreate(osThread(DebugTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartPS2Task */
/**
* @brief Function implementing the PS2Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartPS2Task */
void StartPS2Task(void const * argument)
{
  /* USER CODE BEGIN StartPS2Task */
  /* Infinite loop */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

		GetData();
		printf(" Lx: %d Ly: %d Rx: %d Ry: %d    \r\n",XY[2],XY[3],XY[0],XY[1]);
		for(i1 = 0;i1 < 16;i1++)
		{
			if(All_But[i1]) 
			{
				switch(i1)
				{
					case(0): printf(" Left");break;
					case(1): printf(" Down");break;
					case(2): printf(" Right");break;
					case(3): printf(" Up");break;
					case(4): printf(" Start");break;
					case(5): printf(" Select");break;
					case(6): printf(" Left");break;
					case(7): printf(" Select");break;
					case(8): printf(" X");break;
					case(9): printf(" A");break;
					case(10): printf(" B");break;
					case(11): printf(" Y");break;
					case(12): printf(" R1");break;
					case(13): printf(" L1");break;
					case(14): printf(" R2");break;
					case(15): printf(" L2");break;			
				}
				
			}
			
		}
		printf("被按下\r\n");		
		osDelay(50);
		
  }

  /* USER CODE END StartPS2Task */
}

/* USER CODE BEGIN Header_StratToFTask */
/**
* @brief Function implementing the ToFTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StratToFTask */
void StratToFTask(void const * argument)
{
  /* USER CODE BEGIN StratToFTask */
  /* Infinite loop */

  /*********************************/
	/*         Ranging loop          */
	/*********************************/
  printf("Ranging starts\r\n");
	status = vl53l7cx_start_ranging(&Dev);
  
  get_data_by_polling(&Dev);
  /* USER CODE END StratToFTask */
}

/* USER CODE BEGIN Header_StartDebugTask */
/**
* @brief Function implementing the DebugTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDebugTask */
void StartDebugTask(void const * argument)
{
  /* USER CODE BEGIN StartDebugTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDebugTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void get_data_by_polling(VL53L7CX_Configuration *p_dev){
	do
	{
		status = vl53l7cx_check_data_ready(&Dev, &p_data_ready);
		if(p_data_ready){
			status = vl53l7cx_get_resolution(p_dev, &resolution);
			status = vl53l7cx_get_ranging_data(p_dev, &Results);

			for(int i = 0; i < resolution;i++){
				/* Print per zone results */
				printf("Zone : %2d, Nb targets : %2u, Ambient : %4lu Kcps/spads, ",
						i,
						Results.nb_target_detected[i],
						Results.ambient_per_spad[i]);

				/* Print per target results */
				if(Results.nb_target_detected[i] > 0){
					printf("Target status : %3u, Distance : %4d mm\r\n",
							Results.target_status[VL53L7CX_NB_TARGET_PER_ZONE * i],
							Results.distance_mm[VL53L7CX_NB_TARGET_PER_ZONE * i]);
				}else{
					printf("Target status : 255, Distance : No target\r\n");
				}
			}
			printf("\r\n");
		}else{
			HAL_Delay(5);
		}
	}
	while(1);
}
/* USER CODE END Application */
