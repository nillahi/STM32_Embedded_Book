/*
 * Code Example 43
 * Language: C
 * Chapter: Chapter_09_Timers_and_PWM_Generation
 * Line: 11276
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

TIM_HandleTypeDef htim_servo;

/**
 * @brief  Initialize servo motor control
 * @retval None
 */
void init_servo_control(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // Enable clocks
    __HAL_RCC_TIM3_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    // Configure servo control pin (PA7 = TIM3_CH2)
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // Configure timer for 50Hz PWM (servo standard)
    htim_servo.Instance = TIM3;
    htim_servo.Init.Prescaler = 839;         // 84MHz / 840 = 100kHz
    htim_servo.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim_servo.Init.Period = 1999;           // 100kHz / 2000 = 50Hz (20ms period)
    htim_servo.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    
    HAL_TIM_PWM_Init(&htim_servo);
    
    // Configure PWM channel
    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 150;               // Start at 90 degrees (1.5ms pulse)
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    
    HAL_TIM_PWM_ConfigChannel(&htim_servo, &sConfigOC, TIM_CHANNEL_2);
    
    // Start PWM
    HAL_TIM_PWM_Start(&htim_servo, TIM_CHANNEL_2);
    
    printf("Servo control initialized - servo should move to center position!\n");
}

/**
 * @brief  Set servo position in degrees
 * @param  angle_degrees: 0-180 degrees
 * @retval None
 */
void set_servo_position(uint8_t angle_degrees)
{
    if (angle_degrees > 180) angle_degrees = 180;
    
    // Convert angle to pulse width
    // 0 degrees = 1.0ms = 100 counts, 180 degrees = 2.0ms = 200 counts
    uint32_t pulse_width = 100 + (angle_degrees * 100) / 180;
    
    // Update PWM
    __HAL_TIM_SET_COMPARE(&htim_servo, TIM_CHANNEL_2, pulse_width);
    
    printf("Servo moved to %d degrees (pulse width: %lu counts)\n", 
           angle_degrees, pulse_width);
}

/**
 * @brief  Demo: Servo sweep from 0 to 180 degrees
 * @retval None
 */
void servo_sweep_demo(void)
{
    printf("Starting servo sweep demo...\n");
    
    // Sweep from 0 to 180 degrees
    for (int angle = 0; angle <= 180; angle += 10) {
        set_servo_position(angle);
        HAL_Delay(100);  // Small delay to see movement
    }
    
    HAL_Delay(500);
    
    // Sweep back from 180 to 0 degrees
    for (int angle = 180; angle >= 0; angle -= 10) {
        set_servo_position(angle);
        HAL_Delay(100);
    }
    
    printf("Servo sweep complete!\n");
}