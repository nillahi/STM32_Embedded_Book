/*
 * Code Example 1
 * Language: C
 * Chapter: Chapter_02_Introduction_to_Embedded_Systems
 * Line: 709
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
#include <stdio.h>

// This variable changes when button is pressed (interrupt magic!)
volatile bool button_pressed = false;
volatile uint32_t press_count = 0;

/**
 * @brief  Your first embedded system initialization
 * @retval None
 */
void init_first_project(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // Enable power to GPIO ports
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    
    // Configure LED (PA5 - built-in LED on NUCLEO)
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;    // Output mode
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // Configure button for INTERRUPT (the magic happens here!)
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;   // Interrupt on button press
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    // Tell the CPU to listen for our button interrupt
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
    
    printf("Your first embedded system is ready!\n");
    printf("Press the blue button for instant response!\n");
}

/**
 * @brief  This function runs INSTANTLY when button is pressed!
 * @retval None
 */
void EXTI15_10_IRQHandler(void)
{
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_13) != RESET) {
        
        // LED responds IMMEDIATELY (no delays!)
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        
        // Count button presses
        press_count++;
        button_pressed = true;
        
        // Clear interrupt flag (important!)
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13);
    }
}

/**
 * @brief  Main program - your embedded system in action!
 * @retval None
 */
int main(void)
{
    // Standard STM32 startup
    HAL_Init();
    SystemClock_Config();
    
    // Initialize your first project
    init_first_project();
    
    printf("Welcome to embedded systems programming!\n");
    printf("Watch the LED respond instantly to button presses...\n");
    
    // Main program loop
    while (1) {
        // Check if button was pressed (interrupt sets this flag)
        if (button_pressed) {
            button_pressed = false;
            
            printf("Button press #%lu detected! LED toggled instantly!\n", press_count);
            
            // Special effects for milestone presses
            if (press_count % 10 == 0) {
                printf("Milestone: %lu presses! You're getting the hang of this!\n", press_count);
                
                // Flash LED pattern to celebrate
                for (int i = 0; i < 5; i++) {
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
                    HAL_Delay(100);
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
                    HAL_Delay(100);
                }
            }
        }
        
        // Main program can do other things while waiting for interrupts
        HAL_Delay(10);  // Small delay to prevent overwhelming the output
    }
}