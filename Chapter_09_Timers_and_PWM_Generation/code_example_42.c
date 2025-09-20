/*
 * Code Example 42
 * Language: C
 * Chapter: Chapter_09_Timers_and_PWM_Generation
 * Line: 11096
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

TIM_HandleTypeDef htim3;
volatile uint8_t brightness_level = 0;  // 0-10 brightness levels

/**
 * @brief  Initialize PWM for LED dimming
 * @retval None
 */
void init_led_dimmer(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // Enable clocks
    __HAL_RCC_TIM3_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    // Configure PWM pin (PA6 = TIM3_CH1)
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;      // Alternate function
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;   // Connect to TIM3
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // Configure timer for PWM
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 83;           // 84MHz / 84 = 1MHz
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 999;             // 1MHz / 1000 = 1kHz PWM
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    
    HAL_TIM_PWM_Init(&htim3);
    
    // Configure PWM channel
    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;                 // Start with 0% brightness
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    
    HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1);
    
    // Start PWM
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    
    printf("LED dimmer initialized - press button to change brightness!\n");
}

/**
 * @brief  Set LED brightness (0-100%)
 * @param  brightness_percent: 0 = off, 100 = full brightness
 * @retval None
 */
void set_led_brightness(uint8_t brightness_percent)
{
    if (brightness_percent > 100) brightness_percent = 100;
    
    // Calculate PWM value
    uint32_t pwm_value = (brightness_percent * 999) / 100;
    
    // Update PWM duty cycle
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pwm_value);
    
    printf("LED brightness set to %d%% (PWM value: %lu)\n", 
           brightness_percent, pwm_value);
}

/**
 * @brief  Button press handler - cycles through brightness levels
 * @retval None
 */
void handle_dimmer_button_press(void)
{
    brightness_level++;
    if (brightness_level > 10) brightness_level = 0;
    
    uint8_t brightness_percent = brightness_level * 10;  // 0%, 10%, 20%, ... 100%
    set_led_brightness(brightness_percent);
    
    printf("Button pressed - brightness level %d (%d%%)\n", 
           brightness_level, brightness_percent);
}