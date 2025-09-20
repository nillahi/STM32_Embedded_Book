/*
 * Code Example 26
 * Language: C
 * Chapter: Chapter_07_GPIO_Programming_and_Digital_IO
 * Line: 7960
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

// LED definitions for NUCLEO-F401RE
#define USER_LED_PORT    GPIOA
#define USER_LED_PIN     GPIO_PIN_5

// LED state enumeration
typedef enum {
    LED_OFF = 0,
    LED_ON = 1,
    LED_TOGGLE = 2
} led_state_t;

/**
 * @brief  Initialize LED GPIO with proper configuration
 * @retval HAL_StatusTypeDef
 */
HAL_StatusTypeDef led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // Enable GPIO clock
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    // Configure LED pin
    GPIO_InitStruct.Pin = USER_LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;    // Push-pull output
    GPIO_InitStruct.Pull = GPIO_NOPULL;            // No pull resistors
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;   // Low speed sufficient for LED
    
    HAL_GPIO_Init(USER_LED_PORT, &GPIO_InitStruct);
    
    // Initialize to known state (OFF)
    HAL_GPIO_WritePin(USER_LED_PORT, USER_LED_PIN, GPIO_PIN_RESET);
    
    return HAL_OK;
}

/**
 * @brief  Control LED with multiple options
 * @param  state: LED_OFF, LED_ON, or LED_TOGGLE
 * @retval None
 */
void led_control(led_state_t state)
{
    switch (state) {
        case LED_OFF:
            HAL_GPIO_WritePin(USER_LED_PORT, USER_LED_PIN, GPIO_PIN_RESET);
            break;
            
        case LED_ON:
            HAL_GPIO_WritePin(USER_LED_PORT, USER_LED_PIN, GPIO_PIN_SET);
            break;
            
        case LED_TOGGLE:
            HAL_GPIO_TogglePin(USER_LED_PORT, USER_LED_PIN);
            break;
            
        default:
            // Invalid state - turn off for safety
            HAL_GPIO_WritePin(USER_LED_PORT, USER_LED_PIN, GPIO_PIN_RESET);
            break;
    }
}

/**
 * @brief  Create LED blinking pattern with timing control
 * @param  on_time_ms: LED on duration in milliseconds
 * @param  off_time_ms: LED off duration in milliseconds
 * @param  cycles: Number of blink cycles (0 = infinite)
 * @retval None
 */
void led_blink_pattern(uint16_t on_time_ms, uint16_t off_time_ms, uint16_t cycles)
{
    uint16_t cycle_count = 0;
    
    while ((cycles == 0) || (cycle_count < cycles)) {
        // LED ON phase
        led_control(LED_ON);
        HAL_Delay(on_time_ms);
        
        // LED OFF phase
        led_control(LED_OFF);
        HAL_Delay(off_time_ms);
        
        cycle_count++;
        
        // Allow other tasks to run
        if (cycles == 0 && (cycle_count % 10 == 0)) {
            // Yield to other tasks every 10 cycles
            HAL_Delay(1);
        }
    }
}

/**
 * @brief  Advanced LED effects using PWM-like software control
 * @param  brightness: LED brightness (0-100%)
 * @retval None
 */
void led_set_brightness(uint8_t brightness)
{
    if (brightness > 100) brightness = 100;
    
    // Software PWM implementation
    for (int cycle = 0; cycle < 100; cycle++) {
        if (cycle < brightness) {
            led_control(LED_ON);
        } else {
            led_control(LED_OFF);
        }
        
        // Small delay for visible effect (adjust for desired frequency)
        for (volatile int i = 0; i < 1000; i++);
    }
}