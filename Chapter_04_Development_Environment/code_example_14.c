/*
 * Code Example 14
 * Language: C
 * Chapter: Chapter_04_texorpdfstringparboxtextwidth
 * Line: 4781
 * 
 * This code example is extracted from the STM32 Embedded Systems Programming book.
 * Use this code as a reference for your STM32 projects.
 * 
 * Hardware Requirements:
 * - STM32 Development Board (STM32F4 Discovery recommended)
 * - Basic components as specified in the book
 * 
 * Software Requirements:
 * - STM32CubeIDE
 * - STM32 HAL Library
 * - STM32CubeMX (for configuration)
 * 
 * Usage:
 * 1. Copy this file to your STM32 project
 * 2. Include necessary STM32 HAL headers
 * 3. Configure hardware in STM32CubeMX
 * 4. Build and flash to your development board
 */

/* USER CODE BEGIN Header */
/**
  **********************************************
  * @file           : main.c
  * @brief          : STM32 Hello World with LED, Button, UART, and Timer
  *******************************************************
  */
/* USER CODE END Header */

#include "main.h"
#include <stdio.h>
#include <string.h>

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
volatile uint32_t blink_rate = 1000; // Default 1 second
volatile uint8_t button_pressed = 0;
volatile uint32_t system_ticks = 0;
char uart_buffer[100];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART2_UART_Init(void);

/* USER CODE BEGIN PFP */
void send_status_message(void);
void update_blink_rate(void);
/* USER CODE END PFP */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint32_t last_status_time = 0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/
  HAL_Init();
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */
  // Start timer for LED blinking
  HAL_TIM_Base_Start_IT(&htim2);
  
  // Send startup message
  sprintf(uart_buffer, "\r\n=== STM32 Hello World Project ===\r\n");
  HAL_UART_Transmit(&huart2, (uint8_t*)uart_buffer, strlen(uart_buffer), 100);
  
  sprintf(uart_buffer, "System Clock: %lu Hz\r\n", SystemCoreClock);
  HAL_UART_Transmit(&huart2, (uint8_t*)uart_buffer, strlen(uart_buffer), 100);
  
  sprintf(uart_buffer, "Press user button to change blink rate\r\n\r\n");
  HAL_UART_Transmit(&huart2, (uint8_t*)uart_buffer, strlen(uart_buffer), 100);
  /* USER CODE END 2 */

  /* Infinite loop */
  while (1)
  {
    /* USER CODE BEGIN 3 */
    
    // Handle button press
    if (button_pressed) {
        button_pressed = 0;
        update_blink_rate();
        
        sprintf(uart_buffer, "Blink rate changed to: %lu ms\r\n", blink_rate);
        HAL_UART_Transmit(&huart2, (uint8_t*)uart_buffer, strlen(uart_buffer), 100);
    }
    
    // Send periodic status (every 10 seconds)
    if (HAL_GetTick() - last_status_time > 10000) {
        send_status_message();
        last_status_time = HAL_GetTick();
    }
    
    // Enter sleep mode to save power
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
    
    /* USER CODE END 3 */
  }
}

/* USER CODE BEGIN 4 */

/**
  * @brief  Send system status message via UART
  * @retval None
  */
void send_status_message(void)
{
    sprintf(uart_buffer, "Status: Uptime=%lu s, Blink=%lu ms, Ticks=%lu\r\n", 
            HAL_GetTick()/1000, blink_rate, system_ticks);
    HAL_UART_Transmit(&huart2, (uint8_t*)uart_buffer, strlen(uart_buffer), 100);
}

/**
  * @brief  Update LED blink rate (cycle through predefined rates)
  * @retval None
  */
void update_blink_rate(void)
{
    static uint8_t rate_index = 0;
    uint32_t rates[] = {100, 250, 500, 1000, 2000}; // milliseconds
    
    rate_index = (rate_index + 1) % 5;
    blink_rate = rates[rate_index];
    
    // Update timer period
    __HAL_TIM_SET_AUTORELOAD(&htim2, blink_rate - 1);
}

/**
  * @brief  Timer interrupt callback - toggles LED
  * @param  htim: Timer handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2) {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        system_ticks++;
    }
}

/**
  * @brief  External interrupt callback - handles button press
  * @param  GPIO_Pin: Pin number
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_13) {
        // Simple debouncing
        static uint32_t last_press = 0;
        if (HAL_GetTick() - last_press > 200) {
            button_pressed = 1;
            last_press = HAL_GetTick();
        }
    }
}

/* USER CODE END 4 */