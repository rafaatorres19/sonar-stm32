/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

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
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
int ISRCounter = 0;
uint32_t timeThreshold = 50; //50ms para que ignore pulsaciones accidentales
uint32_t startTime1=0, startTime2=0, startTime3=0, startTime4=0;
uint8_t boton;      //variable para girar el servo
int modoManual ; // 1: Modo manual, 0: Modo automático
uint8_t captura=1;
uint32_t X;
uint32_t Y;
uint32_t Z;
uint32_t distancia;
uint8_t send;
uint32_t vpotenciometro;
uint32_t vccr;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  RCC->AHBENR |= (RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN);// Esto para activar los relojes de los pines
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); //Iniciamos la PWM

  // LED LD1 es PC0

    	  // Con el moder es Para ponerlo en digital output  un 0 y un 1
	  	GPIOC->MODER   &= ~(1<<(2*0+1)); //Aqui un 0 en bit 1
        GPIOC->MODER   |=  (1<<2*0);// Aqui un 1 en el bit 0
        //Con el PUPDR lo ponemos a 00 ambos bits para que no haya ni pull up ni pull down
        GPIOC->PUPDR &= ~(1 << (2*0));     // Borra el bit 0 de PUPDR0 (PC0)
        GPIOC->PUPDR &= ~(1 << (2*0+1)); // Borra el bit 1 de PUPDR0 (PC0)//

  // LED LD2 es PC1

        // Con el moder es Para ponerlo en digital output  un 0 y un 1
        GPIOC->MODER   &= ~(1<<(2*1+1));//Aqui un 0 en bit 3
        GPIOC->MODER   |=  (1<<2*1);// Aqui un 1 en el bit 2
        //Con el PUPDR lo ponemos a 00 ambos bits para que no haya ni pull up ni pull down
        GPIOC->PUPDR &= ~(1 << (2*1));     // Borra el bit 2 de PUPDR0 (PC1)
        GPIOC->PUPDR &= ~(1 << (2 *1+1)); // Borra el bit 3 de PUPDR0 (PC1)

  // Boton USER (B1) es PC13
        GPIOC->MODER &= ~(1 << (2*13));     // Borra el bit 0 de MODER13 (pin de entrada)
        GPIOC->MODER &= ~(1 << (2*13 + 1)); // Borra el bit 1 de MODER13 (pin de entrada)
        GPIOC->PUPDR &= ~(1 << (2*13));     // Borra el bit 0 de PUPDR13 (sin resistencia pull-up o pull-down)
        GPIOC->PUPDR |= (1 << (2*13 + 1));  // Activa resistencia pull-down en PC13 (por defecto)

  // Interrupción EXTI13 para PC13
        EXTI->FTSR |=  (1<<13);             // Habilita la señal de bajada (flanco de bajada)
        EXTI->RTSR &= ~(1<<13);             // Deshabilita el flanco de subida
        SYSCFG->EXTICR[3] &= ~((1<<4)|(1<<5)|(1<<6)|(1<<7)); // Borra bits de configuración para EXTI13
        SYSCFG->EXTICR[3] |= (2<<4);       // Asigna el pin PC13 al EXTI13
        EXTI->IMR |= (1<<13);              // Habilita la interrupción para EXTI13
        NVIC->ISER[1] |= (1<<8);          // Habilita la interrupción en el NVIC para EXTI13

  // Boton B1 es PB1

        GPIOB->MODER &= ~(1 << (2*1));     // Borra el bit 0 de MODER1
        GPIOB->MODER &= ~(1 << (2*1 +1)); // Borra el bit 1 de MODER1
        // Configurar PUPDR (resistencias pull-up/pull-down en PB1)
        GPIOB->PUPDR &= ~(1 << (2*1));     // Borra el bit 0 de PUPDR1
        GPIOB->PUPDR &= ~(1 << (2*1 +1)); // Borra el bit 1 de PUPDR1

  // Interrupcion EXTI1 a PB1

        EXTI->FTSR |=  (1<<1);         // Habilita la señal de bajada
        EXTI->RTSR &= ~(1<<1);         //Desabilita el flanco de subida
        SYSCFG->EXTICR[0] &= ((1<<4)|(1<<5)|(1<<6)|(1<<7));//~0b1111;  //borrar bits
        SYSCFG->EXTICR[0] |= (1<<4); //0b0001;  //Asignar del pin 0 al pin 3 del GPIOB
        EXTI->IMR |= (1<<1);
        NVIC->ISER[0] |= (1<<7);

  // Boton B2 es PB2

        GPIOB->MODER &= ~(1 << (2*2));     // Borra el bit 0 de MODER1
        GPIOB->MODER &= ~(1 << (2*2 +1)); // Borra el bit 1 de MODER1
        // Configurar PUPDR (resistencias pull-up/pull-down en PB1)
        GPIOB->PUPDR &= ~(1 << (2*2));     // Borra el bit 0 de PUPDR1
        GPIOB->PUPDR &= ~(1 << (2*2+1)); // Borra el bit 1 de PUPDR1

  // Interrupcion EXTI2 a PB2

        EXTI->FTSR |=  (1<<2);
        EXTI->RTSR &= ~(1<<2);
        SYSCFG->EXTICR[0] &= ~((1<<8)|(1<<9)|(1<<10)|(1<<11));//((1<<4)|(1<<5)|(1<<6)|(1<<7));
        SYSCFG->EXTICR[0] |=  (1<<8);//(1<<4);
        EXTI->IMR |= (1<<2);
        NVIC->ISER[0] |= (1<<8);

  // Boton B3 es PB3

        GPIOB->MODER &= ~(1 << (2*3));     // Borra el bit 0 de MODER1
        GPIOB->MODER &= ~(1 << (2*3+1)); // Borra el bit 1 de MODER1
        // Configurar PUPDR (resistencias pull-up/pull-down en PB1)
        GPIOB->PUPDR &= ~(1 << (2*3));     // Borra el bit 0 de PUPDR1
        GPIOB->PUPDR &= ~(1 << (2*3+1)); // Borra el bit 1 de PUPDR1

  // Interrupcion EXTI3

        EXTI->FTSR |=  (1<<3);
        EXTI->RTSR &= ~(1<<3);
        SYSCFG->EXTICR[0] &= ~ ((1<<12)|(1<<13)|(1<<14)|(1<<15));//~((1<<8)|(1<<9)|(1<<10)|(1<<11));
        SYSCFG->EXTICR[0] |= (1<<12);//(1<<8);
        EXTI->IMR |= (1<<3);
        NVIC->ISER[0] |= (1<<9);

  // Boton B4 es PB4

        GPIOB->MODER &= ~(1 << (2*4));     // Borra el bit 0 de MODER1
        GPIOB->MODER &= ~(1 << (2*4+1)); // Borra el bit 1 de MODER1
        // Configurar PUPDR (resistencias pull-up/pull-down en PB1)
        GPIOB->PUPDR &= ~(1 << (2*4));     // Borra el bit 0 de PUPDR1
        GPIOB->PUPDR &= ~(1 << (2*4+1)); // Borra el bit 1 de PUPDR1

  // Interrupcion EXTI4

        EXTI->FTSR |=  (1<<4);
        EXTI->RTSR &= ~(1<<4);
        SYSCFG->EXTICR[1] &= ~ ((1<<0)|(1<<1)|(1<<2)|(1<<3));//((1<<12)|(1<<13)|(1<<14)|(1<<15));
        SYSCFG->EXTICR[1] |= (1<<0); //(1<<12);
        EXTI->IMR |= (1<<4);
        NVIC->ISER[0] |= (1<<10);

   //Indicamos un valor de PWM para que vuela a la posicion inicial
   __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 150);
   char buf[50];  // Buffer para el mensaje
  /* USER CODE END 2 */
   modoManual=1;
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
   HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);  //activa la señal PWM en el canal 1
   GPIOC->MODER &= ~(0x03 << (2 * 4));   // Limpiar los bits 8 y 9 (modo de PC4)
   GPIOC->MODER |= (0x01 << (2 * 4));    // Establecer PC4 como salida (01 en MODER)
   // Configuramos el pin PC4 para que no use resistencias pull-up ni pull-down
   GPIOC->PUPDR &= ~(0x03 << (2 * 4));


   while (1)
      {
          if (modoManual == 1)
          {
              switch (boton)
              {
                  case 1:  //Boton==1

                	  //Movimiento del servo 50=1ms -> -90º
                      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 50); //50=1ms -90º

                      //Medicion de la distancia
                      GPIOC->BSRR |= (1 << 4); // Trigger HIGH (PC4)
                      __HAL_TIM_SET_COUNTER(&htim3, 0);
                      HAL_TIM_Base_Start_IT(&htim3); // Temporizador para bajar el Trigger después de 10 us
                      __HAL_TIM_SET_COUNTER(&htim4, 0); //Reseteo para evitar valores residuales
                      HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1); // Activar captura de Echo

                      sprintf(buf, "La distancia es: %lu mm\r\n", distancia);
                      HAL_UART_Transmit(&huart2, (uint8_t *)buf, strlen(buf), 0xFFFFFFFF);

                      //Captura y reinicio
                      captura = 0; //Ya capturado
                      boton = 0; //Reinicia el boton
                      break;

                  case 2:  //Boton==2

                      //Movimiento del servo 100=1.3ms -> -30º
                      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 100); //50=1ms -30º

                      //Medicion de la distancia
                      GPIOC->BSRR |= (1 << 4); // Trigger HIGH (PC4)
                      __HAL_TIM_SET_COUNTER(&htim3, 0);
                      HAL_TIM_Base_Start_IT(&htim3); // Temporizador para bajar el Trigger después de 10 us
                      __HAL_TIM_SET_COUNTER(&htim4, 0); //Reseteo para evitar valores residuales
                      HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1); // Activar captura de Echo

                      sprintf(buf, "La distancia es: %lu mm\r\n", distancia);
                      HAL_UART_Transmit(&huart2, (uint8_t *)buf, strlen(buf), 0xFFFFFFFF);

                      //Captura y reinicio
                      captura = 0; //Ya capturado
                      boton = 0; //Reinicia el boton
                      break;

                  case 3:  //Boton==3
                      //Movimiento del servo 200=1ms -> -30º
                      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 200); //200=1.8ms 30º

                      //Medicion de la distancia
                      GPIOC->BSRR |= (1 << 4); // Trigger HIGH (PC4)
                      __HAL_TIM_SET_COUNTER(&htim3, 0);
                      HAL_TIM_Base_Start_IT(&htim3); // Temporizador para bajar el Trigger después de 10 us
                      __HAL_TIM_SET_COUNTER(&htim4, 0); //Reseteo para evitar valores residuales
                      HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1); // Activar captura de Echo

                      sprintf(buf, "La distancia es: %lu mm\r\n", distancia);
                      HAL_UART_Transmit(&huart2, (uint8_t *)buf, strlen(buf), 0xFFFFFFFF);

                      //Captura y reinicio
                      captura = 0; //Ya capturado
                      boton = 0; //Reinicia el boton
                      break;

                  case 4: //Boton==4

                	  //Movimiento del servo 250=2ms -> 90º
                      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 250); //250=2ms 90º

                      //Medicion de la distancia
                      GPIOC->BSRR |= (1 << 4); // Trigger HIGH (PC4)
                      __HAL_TIM_SET_COUNTER(&htim3, 0);
                      HAL_TIM_Base_Start_IT(&htim3); // Temporizador para bajar el Trigger después de 10 us
                      __HAL_TIM_SET_COUNTER(&htim4, 0); //Reseteo para evitar valores residuales
                      HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1); // Activar captura de Echo

                      sprintf(buf, "La distancia es: %lu mm\r\n", distancia);
                      HAL_UART_Transmit(&huart2, (uint8_t *)buf, strlen(buf), 0xFFFFFFFF);

                      //Captura y reinicio
                      captura = 0; //Ya capturado
                      boton = 0; //Reinicia el boton
                      break;
              }
          }
          // Control en modo automático(modoManual == 0)// Si está en modo automático
          else if(modoManual==0)
          {
		  //Movimiento del servo 50=1ms -> -90º
        	  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 50); //50=1ms -90

        	  //Medicion de la distancia
        	  GPIOC->BSRR |= (1 << 4); // Trigger HIGH (PC4)
        	  __HAL_TIM_SET_COUNTER(&htim3, 0);
        	  HAL_TIM_Base_Start_IT(&htim3); // Temporizador para bajar el Trigger después de 10 us
        	  __HAL_TIM_SET_COUNTER(&htim4, 0); //Reseteo para evitar valores residuales
        	  HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1); // Activar captura de Echo


        	  sprintf(buf, "La distancia es: %lu mm\r\n", distancia);
        	  HAL_UART_Transmit(&huart2, (uint8_t *)buf, strlen(buf), 0xFFFFFFFF);

        	  //Captura
        	  captura = 0; //Ya capturado


		  //Movimiento del servo 100=1.3ms -> -30º
        	  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 100); //100=1.3ms -30º

        	  //Medicion de la distancia
        	  GPIOC->BSRR |= (1 << 4); // Trigger HIGH (PC4)
        	  __HAL_TIM_SET_COUNTER(&htim3, 0);
        	  HAL_TIM_Base_Start_IT(&htim3); // Temporizador para bajar el Trigger después de 10 us
        	  __HAL_TIM_SET_COUNTER(&htim4, 0); //Reseteo para evitar valores residuales
        	  HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1); // Activar captura de Echo


        	  sprintf(buf, "La distancia es: %lu mm\r\n", distancia);
        	  HAL_UART_Transmit(&huart2, (uint8_t *)buf, strlen(buf), 0xFFFFFFFF);

        	  //Captura
        	  captura = 0; //Ya capturado


		  //Movimiento del servo 200=1.8ms -> 30º
        	  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 200); //50=1ms 30º


        	  //Medicion de la distancia
        	  GPIOC->BSRR |= (1 << 4); // Trigger HIGH (PC4)
        	  __HAL_TIM_SET_COUNTER(&htim3, 0);
        	  HAL_TIM_Base_Start_IT(&htim3); // Temporizador para bajar el Trigger después de 10 us
        	  __HAL_TIM_SET_COUNTER(&htim4, 0); //Reseteo para evitar valores residuales
        	  HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1); // Activar captura de Echo



        	  sprintf(buf, "La distancia es: %lu mm\r\n", distancia);
        	  HAL_UART_Transmit(&huart2, (uint8_t *)buf, strlen(buf), 0xFFFFFFFF);

        	  //Captura
        	  captura = 0; //Ya capturado


		  //Movimiento del servo 250=1ms -> 90º
        	  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 250); //50=1ms 90º


        	  //Medicion de la distancia
        	  GPIOC->BSRR |= (1 << 4); // Trigger HIGH (PC4)
        	  __HAL_TIM_SET_COUNTER(&htim3, 0);
        	  HAL_TIM_Base_Start_IT(&htim3); // Temporizador para bajar el Trigger después de 10 us
        	  __HAL_TIM_SET_COUNTER(&htim4, 0); //Reseteo para evitar valores residuales
        	  HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1); // Activar captura de Echo



        	  sprintf(buf, "La distancia es: %lu mm\r\n", distancia);
        	  HAL_UART_Transmit(&huart2, (uint8_t *)buf, strlen(buf), 0xFFFFFFFF);

        	  //Captura
        	  captura = 0; //Ya capturado

          }
      }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback (TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM3) //control de pulso
	{
		GPIOC->BSRR |= (1<<20); //reset EL TRIGGER
		//captura =0;
	}
}
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef*htim)
{
	if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)
	{
		if(captura==0) //si captura es 0
		{
			X=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1); //Captiramos el valor en X
			captura=1;                                       //Ya capurado
		}
		else
		{

			Y=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1); // Si captura no es 1 se captura en Y

			if(Y>X)
			{
				Z=Y-X;
				distancia=(Z*10)/58;    //Calculamos la diferencia
				send=1;                //Activamos
			}
			else if(X>Y)
			{				//Significa que hay desbordamiento
				Z=(0xffff-X)+Y;
				distancia=(Z*10)/58;
				send=1;
			}
		}
	}
}
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV3;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
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

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 320-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 2000-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
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
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 31;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 18;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

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
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 31;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void EXTI1_IRQHandler(void)
{
	if(HAL_GetTick()-startTime1 > timeThreshold) //If para el sistema antirrebotes
	{
		ISRCounter++;
		startTime1= HAL_GetTick();
		if (( EXTI->PR & (1<<1) )!=0)
		{
			GPIOC->BSRR = (1<<0)<<16; //Apaga
			GPIOC->BSRR = (1<<1)<<16; //Apaga
			EXTI->PR |= (1<<1);

			boton=1; //indicamos que boton se pulsa -90 grados
			modoManual=1;

		}
	}
}
void EXTI2_IRQHandler(void)
{
	if(HAL_GetTick()-startTime2 > timeThreshold) //If para el sistema antirrebotes
	{
		ISRCounter++;
		startTime2= HAL_GetTick();
		if (( EXTI->PR & (1<<2) )!=0)
		{
			GPIOC->BSRR = (1<<0)<<16; //Apaga
			GPIOC->BSRR = (1<<1);     //Enciende
			EXTI->PR |= (1<<2);

			boton=2;  //-30 grados
			modoManual=1;
		}
	}
}
void EXTI3_IRQHandler(void)
{
	if(HAL_GetTick()-startTime3 > timeThreshold) //If para el sistema antirrebotes
	{
		ISRCounter++;
		startTime3= HAL_GetTick();
		if (( EXTI->PR & (1<<3) )!=0)
		{
			GPIOC->BSRR = (1<<0);     //Enciende
			GPIOC->BSRR = (1<<1)<<16; //Apaga
			EXTI->PR |= (1<<3);

			boton=3;  //30 grados
			modoManual=1;
		}
	}
}
void EXTI4_IRQHandler(void)
{
	if(HAL_GetTick()-startTime4 > timeThreshold) //If para el sistema antirrebotes
	{
		ISRCounter++;
		startTime4= HAL_GetTick();
		if (( EXTI->PR & (1<<4) )!=0)
		{
			GPIOC->BSRR = (1<<0); //Enciende
			GPIOC->BSRR = (1<<1); //Enciende
			EXTI->PR |= (1<<4);

			boton=4; //90 grados
			modoManual=1;
		}
	}
}
void EXTI15_10_IRQHandler(void)  // EXTI13 está en este rango
{
   if (HAL_GetTick() - startTime1 > timeThreshold)  // Si para el sistema antirrebotes
    {
        ISRCounter++;
        //startTime1 = HAL_GetTick();
        if ((EXTI->PR & (1 << 13)) != 0)  // Si la bandera de interrupción EXTI13 está activada
        {
            EXTI->PR |= (1 << 13);  // Limpiar la bandera de la interrupción


            modoManual=!modoManual;
        }
    }
}
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
