/*
 * Code Example 35
 * Language: C
 * Chapter: Chapter_08_Interrupts_and_Exception_Handling
 * Line: 9907
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

#include "main.h"

TIM_HandleTypeDef htim2;  // Timer handle

/**
 * @brief  Initialize timer for 2 Hz interrupts (every 500ms)
 * @retval None
 */
void init_timer_interrupt_example(void)
{
    // Enable timer clock
    __HAL_RCC_TIM2_CLK_ENABLE();
    
    // Configure timer
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 8399;         // Divide 84MHz by 8400 = 10kHz
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 4999;            // Count to 5000 = 500ms at 10kHz
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    
    HAL_TIM_Base_Init(&htim2);
    
    // Configure interrupt
    HAL_NVIC_SetPriority(TIM2_IRQn, 10, 0);  // Low priority
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
    
    // Start the timer with interrupt
    HAL_TIM_Base_Start_IT(&htim2);
    
    printf("Timer interrupt started - LED will blink at exactly 2 Hz\n");
}

/**
 * @brief  Timer interrupt handler - runs every 500ms
 * @retval None
 */
void TIM2_IRQHandler(void)
{
    // Check if this is our timer
    if (__HAL_TIM_GET_IT(&htim2, TIM_IT_UPDATE) != RESET) {
        
        // Clear the interrupt flag
        __HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);
        
        // Toggle LED - this happens exactly every 500ms!
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        
        // Optional: count interrupts
        static uint32_t interrupt_count = 0;
        interrupt_count++;
        
        if (interrupt_count % 10 == 0) {  // Every 5 seconds
            printf("Timer interrupt #%lu (5 seconds elapsed)\n", interrupt_count);
        }
    }
}