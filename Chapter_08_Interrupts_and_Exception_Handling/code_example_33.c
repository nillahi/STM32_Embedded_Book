/*
 * Code Example 33
 * Language: C
 * Chapter: Chapter_08_Interrupts_and_Exception_Handling
 * Line: 9532
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

// System states
typedef enum {
    ALARM_DISARMED,
    ALARM_ARMED,
    ALARM_TRIGGERED
} alarm_state_t;

// Global variables (volatile because they're used in interrupts)
volatile alarm_state_t alarm_state = ALARM_DISARMED;
volatile bool motion_detected = false;
volatile uint32_t last_button_press = 0;

/**
 * @brief  Initialize the alarm system
 * @retval None
 */
void init_alarm_system(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // Enable clocks
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    
    // Configure status LEDs
    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;  // 3 LEDs
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // Configure arm/disarm button (PC13 - built-in button)
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;  // Interrupt on press
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    // Configure motion sensor (PB0 - simulate with external button)
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;   // Interrupt on motion
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;         // Pull down, button pulls up
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    // Configure interrupts in NVIC
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 8, 0);  // Button - medium priority
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
    
    HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);      // Motion - high priority!
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
    
    printf("Alarm system initialized!\n");
    printf("PC13 button: arm/disarm alarm\n");
    printf("PB0 button: simulate motion sensor\n");
}

/**
 * @brief  Button interrupt handler (arm/disarm)
 * @retval None
 */
void EXTI15_10_IRQHandler(void)
{
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_13) != RESET) {
        
        // Simple debouncing
        uint32_t current_time = HAL_GetTick();
        if (current_time - last_button_press > 200) {  // 200ms debounce
            
            // Toggle alarm state
            if (alarm_state == ALARM_DISARMED) {
                alarm_state = ALARM_ARMED;
                printf("ALARM ARMED!\n");
            } else {
                alarm_state = ALARM_DISARMED;
                motion_detected = false;  // Clear any motion
                printf("ALARM DISARMED\n");
            }
            
            last_button_press = current_time;
        }
        
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13);
    }
}

/**
 * @brief  Motion sensor interrupt handler (HIGH PRIORITY!)
 * @retval None
 */
void EXTI0_IRQHandler(void)
{
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_0) != RESET) {
        
        // Only trigger if alarm is armed
        if (alarm_state == ALARM_ARMED) {
            alarm_state = ALARM_TRIGGERED;
            motion_detected = true;
            
            printf("*** MOTION DETECTED - ALARM TRIGGERED! ***\n");
        }
        
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
    }
}

/**
 * @brief  Update LED display based on alarm state
 * @retval None
 */
void update_alarm_display(void)
{
    switch (alarm_state) {
        case ALARM_DISARMED:
            // Green LED on, others off
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);    // Green
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);  // Yellow
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);  // Red
            break;
            
        case ALARM_ARMED:
            // Yellow LED blinking
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);  // Green
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);                 // Yellow blink
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);  // Red
            break;
            
        case ALARM_TRIGGERED:
            // Red LED flashing rapidly
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);  // Green
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);  // Yellow
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);                 // Red flash
            break;
    }
}