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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "stm32f1xx.h"
#include <stdio.h>

void timer_init(void)
{
    RCC->APB1ENR |= (1<<0);
    TIM2->PSC = 7;
    TIM2->ARR = 0xFFFF;
    TIM2->CR1 |= (1<<0);
}

void delay_us(uint16_t us)
{
    TIM2->CNT = 0;
    while(TIM2->CNT < us);
}

void delay_ms(uint16_t ms)
{
    for(uint16_t i = 0; i < ms; i++)
        delay_us(1000);
}

void uart_init(void)
{
    RCC->APB2ENR |= (1<<0);
    AFIO->MAPR |= (2<<24);
    RCC->APB1ENR |= (1<<17);
    RCC->APB2ENR |= (1<<2);

    GPIOA->CRL &= ~(0xF << 8);
    GPIOA->CRL |=  (0xB << 8);
    GPIOA->CRL &= ~(0xF << 12);
    GPIOA->CRL |=  (0x4 << 12);

    USART2->BRR = 69;
    USART2->CR1 |= (1<<3);
    USART2->CR1 |= (1<<2);
    USART2->CR1 |= (1<<13);
}

void uart_send_char(char c)
{
    while(!(USART2->SR & (1<<7)));
    USART2->DR = c;
}

void uart_send_string(char *str)
{
    while(*str)
    {
        uart_send_char(*str);
        str++;
    }
}

void OW_SetOutput(void)
{
    GPIOA->CRL &= ~(0xF << 24);
    GPIOA->CRL |=  (0x7 << 24);
}

void OW_SetInput(void)
{
    GPIOA->CRL &= ~(0xF << 24);
    GPIOA->CRL |=  (0x4 << 24);
}

void OW_Low(void)  { GPIOA->ODR &= ~(1<<6); }
void OW_High(void) { GPIOA->ODR |=  (1<<6); }
uint8_t OW_Read_Pin(void) { return (GPIOA->IDR & (1<<6)) ? 1 : 0; }

uint8_t OW_Reset(void)
{
    OW_SetOutput();
    OW_Low();
    delay_us(500);
    OW_SetInput();
    delay_us(60);
    uint8_t presence = OW_Read_Pin();
    delay_us(430);
    return !presence;
}

void OW_Write_Bit(uint8_t bit)
{
    OW_SetOutput();
    if(bit == 0)
    {
        OW_Low();
        delay_us(65);
        OW_SetInput();
    }
    else
    {
        OW_Low();
        delay_us(3);
        OW_SetInput();
        delay_us(65);
    }
}

void OW_Write_Byte(uint8_t byte)
{
    for(int i = 0; i < 8; i++)
        OW_Write_Bit((byte >> i) & 1);
}

uint8_t OW_Read_Bit(void)
{
    OW_SetOutput();
    OW_Low();
    delay_us(3);
    OW_SetInput();
    delay_us(8);
    uint8_t isRead = OW_Read_Pin();
    delay_us(55);
    return isRead;
}

uint8_t OW_Read_Byte(void)
{
    uint8_t byte = 0;
    for(int i = 0; i < 8; i++)
        byte |= (OW_Read_Bit() << i);
    return byte;
}

void DS18B20_Start(void)
{
    OW_Reset();
    delay_us(100);
    OW_Write_Byte(0xCC);
    OW_Write_Byte(0x44);
}

uint16_t DS18B20_Read(void)
{
    OW_Reset();
    delay_us(100);
    OW_Write_Byte(0xCC);
    OW_Write_Byte(0xBE);
    uint8_t byte0 = OW_Read_Byte();
    uint8_t byte1 = OW_Read_Byte();
    return (byte1 << 8) | byte0;
}

int main(void)
{
    uart_init();
    timer_init();

    while(1)
    {
    	DS18B20_Start();
    	    delay_ms(750);
    	    float temp = DS18B20_Read() * 0.0625;
    	    char buffer[40];
    	    sprintf(buffer, "Temp: %.2f C\r\n", temp);
    	    uart_send_string(buffer);

    }
}
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
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */


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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
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

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
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

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

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

