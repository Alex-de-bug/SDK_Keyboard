/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"
#include "i2c.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "fonts.h"
#include "kb.h"
#include "oled.h"
#include "pca9538.h"
#include "sdk_uart.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAX_16 65535
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
void KB_Test(uint32_t *first, uint32_t *second, int8_t *mode);
void OLED_KB(uint8_t OLED_Keys[]);
void oled_Reset(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
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
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
  oled_Init();
  oled_Reset();

  // Display startup messages
  oled_Reset();
  oled_WriteString("7 8 9", Font_7x10, White);
  oled_SetCursor(0, 10);
  oled_WriteString("4 5 6", Font_7x10, White);
  oled_SetCursor(0, 20);
  oled_WriteString("1 2 3", Font_7x10, White);
  oled_SetCursor(0, 30);
  oled_WriteString("0 S =", Font_7x10, White);
  oled_UpdateScreen();       // Ensure the messages are rendered on the OLED
  HAL_Delay(3000);           // Wait for 3 seconds (3000 milliseconds)
  oled_Reset();
  /* USER CODE END 2 */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint32_t firstElem = 0, secondElem = 0;
  int8_t mode = -1; // 0 - +, 1 - -, 2 - *

  while (1) {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    KB_Test(&firstElem, &secondElem, &mode);
    HAL_Delay(150);
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/*
 * 789            0x09
 * 456
 * 123	0x01
 * 0+=  0x00 0x0A 0x0B
 */


void printNumber(int32_t a) {
	uint8_t i = 0;
	if (a<0){
		oled_SetCursor(i, 0);
		i+=8;
		oled_WriteChar('-', Font_7x10, White);
		a *=-1;
	}

    char buffer[10];  // Assuming the maximum number of digits in a uint32_t is 10
    uint8_t index = 0;

    // Extract digits and store them in the buffer
    do {
        buffer[index++] = a % 10;  // Convert the digit to a character and store it
        a /= 10;
    } while (a);

    // Print the number in the correct order
    while (index > 0) {
        oled_SetCursor(i, 0);
        i += 8;

        oled_WriteChar(0x30 + buffer[--index], Font_7x10, White);
    }
}


void KB_Test(uint32_t *first, uint32_t *second, int8_t *mode) {
  uint8_t Key, Row[4] = {ROW4, ROW3, ROW2, ROW1};

  for (int i = 0; i < 4; i++) {
    Key = Check_Row(Row[i]);
    if(Key != 0xC){

      switch (Key) {
      case 0x0A: {
        *mode += 1;
        if (*mode == 3) {
          *mode = 0;
        }
        oled_Reset();
        switch (*mode) {
        case 0: {
          oled_WriteChar('+', Font_7x10, White);
          break;
        }
        case 1: {
          oled_WriteChar('-', Font_7x10, White);
          break;
        }
        case 2: {
          oled_WriteChar('*', Font_7x10, White);
          break;
        }
        }

        break;
      }

      case 0x0B: {
        if (*mode != -1) {
          int32_t result = 0;
          switch (*mode) {
          case 0: {
            result = *first + *second;
            break;
          }
          case 1: {
            result = *first - *second;
            break;
          }
          case 2: {
            result = *first * (*second);
            break;
          }
          }

          if (result < MAX_16 && result >= -MAX_16) {
            oled_Reset();
            printNumber(result);
            *first = result; // Assign result to first operand for continuous operations
            *second = 0;
            *mode = -1;
          }
          else{
            oled_Reset();
            oled_WriteString("Perepolnenie", Font_7x10, White);
            oled_SetCursor(0, 8);
            oled_WriteString("Bratuxa!", Font_7x10, White);
          }

        }
        break;
      }
      default: {
        if (*mode != -1) {
          uint32_t new_n = *second*10 + (uint32_t)Key;
          if (new_n <= MAX_16) {
            *second = new_n;
          }
          oled_Reset();
          printNumber(*second);

        } else {
          uint32_t new_n = *first*10 + (uint32_t)Key;
          if (new_n <= MAX_16) {
            *first = new_n;
          }
          oled_Reset();
          printNumber(*first);
        }

        break;
      }
      }
    }
  }
  oled_UpdateScreen();
}

  void oled_Reset(void) {
    oled_Fill(Black);
    oled_SetCursor(0, 0);
    oled_UpdateScreen();
  }
  /* USER CODE END 4 */

  /**
   * @brief  This function is executed in case of error occurrence.
   * @retval None
   */
  void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state
     */

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
  void assert_failed(uint8_t * file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line
       number, tex: printf("Wrong parameters value: file %s on line %d\r\n",
       file, line) */
    /* USER CODE END 6 */
  }
#endif /* USE_FULL_ASSERT */

  /************************ (C) COPYRIGHT STMicroelectronics *****END OF
   * FILE****/
