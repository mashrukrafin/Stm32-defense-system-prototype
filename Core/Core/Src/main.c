/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stdbool.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define idle   0
#define done   1
#define Freq  72000000
#define TRIG_PIN GPIO_PIN_13
#define TRIG_PORT GPIOB
#define INT_DUTY    500
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim4;

osThreadId servomotorHandle;
osThreadId sonarHandle;
osSemaphoreId myBinarySem01Handle;
/* USER CODE BEGIN PV */
int SERV_0=500, SERV_180=2500,SERV1_0=500, SERV1_180=2500, sig=1;
uint8_t State = idle;
uint16_t TIM1_OVC = 0,OV=0;
uint32_t Tedge = 0, T1 = 0, T2 = 0, Ticks = 0, T_Freq=0;
uint8_t Distance  = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM4_Init(void);
void Startservomotor(void const * argument);
void Startsonar(void const * argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void IR_LEFT(void){
	SERV_0=SERV_0+100;
	  	  		  if(SERV_0>=2500){
	  	  			  SERV_0=2500;
	  	  			  TIM2->CCR1=SERV_0;
	  	  	  }
	  	  		  else {
	  	  		  TIM2->CCR1=SERV_0;
	  	  		  }
	  	  		  SERV_180=SERV_0;
	  	  		  osDelay(100);
             }

void IR_RIGHT(void){
	SERV_180=SERV_180-100;
	  	  		  if(SERV_180<=500){
	  	  			  SERV_180=500;
	  	  			  TIM2->CCR1=SERV_180;
	  	  	  }
	  	  		  else {
	  	  		  TIM2->CCR1=SERV_180;
	  	  		  }
	  	  		  SERV_0=SERV_180;
	  	  		osDelay(100);
             }
void IR_UP(void){
	SERV1_0=SERV1_0+100;
	  	  		  if(SERV1_0>=2500){
	  	  			  SERV1_0=2500;
	  	  			  TIM2->CCR2=SERV1_0;
	  	  	  }
	  	  		  else {
	  	  		  TIM2->CCR2=SERV1_0;
	  	  		  }
	  	  		  SERV1_180=SERV1_0;
	  	  		  osDelay(100);
             }

void IR_DOWN(void){
	SERV1_180=SERV1_180-100;
	  	  		  if(SERV1_180<=500){
	  	  			  SERV1_180=500;
	  	  			  TIM2->CCR2=SERV1_180;
	  	  	  }
	  	  		  else {
	  	  		  TIM2->CCR2=SERV1_180;
	  	  		  }
	  	  		  SERV1_0=SERV1_180;
	  	  		osDelay(100);
             }

void rotate_servo (void) {
	for(int x=500; x<2500; x=x+20)
				  	  	      {
				  	  	        __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1, x);
				  	  	       osDelay(80);
				  	  	      }
				  for(int x=2500; x>500; x=x-20)
				  		  	 {
				  		  	   __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1, x);
				  		       osDelay(80);
				  		     }
}

void PWM_Shoot (void){
	if (sig == 1) {
		      for (int i = 0; i < 4; i++) {
		          TIM4->CCR1 = 500; // Set PWM duty cycle to 0 (LED OFF)
		         osDelay(200); // Delay 2 seconds
		      }
		      sig = 0; // Exit idle mode
		  }
	else
	{
		      TIM4->CCR1 = 0; // Set PWM duty cycle to 0 (LED OFF)
		     osDelay(500); // Delay 0.5 seconds
		  }
}

void Sonar_Triger(void)
{
	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, 1);
    __HAL_TIM_SET_COUNTER(&htim1, 0);
	while (__HAL_TIM_GET_COUNTER (&htim1) < 10);
	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, 0);
}

void State_Change(void){
    if(State==idle)
    {
    	T1 = Tedge;
        TIM1_OVC = 0;
        State = done;
        __HAL_TIM_SET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_4, TIM_INPUTCHANNELPOLARITY_FALLING);
    }
    else if(State==done)
    {
    	T2 = Tedge;
    	OV=TIM1_OVC;
        State = idle;
    	__HAL_TIM_SET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_4, TIM_INPUTCHANNELPOLARITY_RISING);
    }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim){
	Tedge=TIM1->CCR4;
	State_Change();
}

void servo (void){
	 SERV_0=TIM2->CCR1;
	 SERV_180=TIM2->CCR1;
		    	  if (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)==1){
		                IR_LEFT();
		                osDelay(100);
		    	  	  }
		    	  else if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3)==1){
		    		  IR_RIGHT();
		    		  osDelay(100);
		    	  }
		    	  else if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2)==1){
		    	  		 IR_UP();
		    	  		 osDelay(100);
		    	  }
		    	  else if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_6)==1){
		    	  		IR_DOWN();
		    	  		osDelay(100);
		    	  }
		    	  else {
		    	  		  TIM2->CCR1=TIM2->CCR1;
		    	  		  TIM2->CCR2=TIM2->CCR2;
		    	  		  osDelay(100);
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
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

    HAL_TIM_Base_Start_IT(&htim1);
    HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_4);
    __HAL_TIM_SET_CAPTUREPOLARITY(&htim1, TIM_CHANNEL_4, TIM_INPUTCHANNELPOLARITY_RISING);
    T_Freq=Freq/(TIM1->PSC+1);

    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, INT_DUTY);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, INT_DUTY);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, INT_DUTY);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 0);

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of myBinarySem01 */
  osSemaphoreDef(myBinarySem01);
  myBinarySem01Handle = osSemaphoreCreate(osSemaphore(myBinarySem01), 1);

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
  /* definition and creation of servomotor */
  osThreadDef(servomotor, Startservomotor, osPriorityAboveNormal, 0, 128);
  servomotorHandle = osThreadCreate(osThread(servomotor), NULL);

  /* definition and creation of sonar */
  osThreadDef(sonar, Startsonar, osPriorityNormal, 0, 256);
  sonarHandle = osThreadCreate(osThread(sonar), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 71;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 71;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 20000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 7199;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 700;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA2 PA3 PA4 PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_Startservomotor */
/**
  * @brief  Function implementing the servomotor thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Startservomotor */
void Startservomotor(void const * argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
	for(;;)
	  {
		  if (osSemaphoreWait(myBinarySem01Handle, osWaitForever) == osOK)
			  {
			  for(int x=500; x<2500 && Distance>=25; x=x+20)
			  	  	      {
			  	  	        __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1, x);
			  	  	       osDelay(80);
			  	  	      }
			  for(int x=2500; x>500 && Distance>=25; x=x-20)
			  		  	 {
			  		  	   __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1, x);
			  		       osDelay(80);
			  		     }
			  }
		 		  	 osDelay(100);
		  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_Startsonar */
/**
* @brief Function implementing the sonar thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Startsonar */
void Startsonar(void const * argument)
{
  /* USER CODE BEGIN Startsonar */
  /* Infinite loop */
	for(;;)
	  {
		bool b=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
		 if (State == idle) {
		  	              Ticks = T2 + (OV * TIM1->ARR) - T1;
		  	              Distance = ((float)Ticks / (float)T_Freq) * 34000 / 2;
		  	              if (Distance<=25 && Distance>15 && b==0)
		  	              {
		  	            	  servo();
		  	              }

		  	              else if (Distance<=15 && Distance>5 && b==0)
		  	              {
		  	            	  PWM_Shoot();
		  	              }
		  	              else if (b==1){
		  	            	  rotate_servo();
		  	              }

		  	              else {
		  	            	osSemaphoreRelease(myBinarySem01Handle);
		  	              }
		  	          }
		  	      Sonar_Triger();
		  	      osDelay(10);
	  }
  /* USER CODE END Startsonar */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM3 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM3) {
    HAL_IncTick();
  }
  if (htim->Instance == TIM1) {
	  TIM1_OVC++;
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
