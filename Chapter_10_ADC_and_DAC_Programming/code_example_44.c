/*
 * Code Example 44
 * Language: C
 * Chapter: Chapter_10_ADC_and_DAC_Programming
 * Line: 11535
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

ADC_HandleTypeDef hadc1;

/**
 * @brief  Initialize ADC for simple voltage reading
 * @retval None
 */
void init_simple_adc(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // Enable clocks
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    // Configure ADC pin (PA0 = ADC1_IN0)
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;     // Analog mode
    GPIO_InitStruct.Pull = GPIO_NOPULL;          // No pull resistors
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // Configure ADC
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;  // Slower for accuracy
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;            // 12-bit = 4096 levels
    hadc1.Init.ScanConvMode = DISABLE;                     // Single channel
    hadc1.Init.ContinuousConvMode = DISABLE;               // Single conversion
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;            // Right-aligned data
    hadc1.Init.NbrOfConversion = 1;                        // One channel
    
    HAL_ADC_Init(&hadc1);
    
    // Configure the channel
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = ADC_CHANNEL_0;                       // PA0
    sConfig.Rank = 1;                                      // First (and only)
    sConfig.SamplingTime = ADC_SAMPLETIME_144CYCLES;       // Slow for accuracy
    
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    
    printf("Simple ADC initialized - connect voltage to PA0!\n");
    printf("Voltage range: 0V to 3.3V\n");
}

/**
 * @brief  Read voltage from ADC (simple version)
 * @retval float: Voltage in volts (0.0 to 3.3V)
 */
float read_voltage_simple(void)
{
    // Start ADC conversion
    HAL_ADC_Start(&hadc1);
    
    // Wait for conversion to complete (blocking - simple version)
    HAL_ADC_PollForConversion(&hadc1, 100);
    
    // Get the result
    uint16_t adc_value = HAL_ADC_GetValue(&hadc1);
    
    // Stop ADC
    HAL_ADC_Stop(&hadc1);
    
    // Convert to voltage
    float voltage = (float)adc_value * 3.3f / 4095.0f;
    
    return voltage;
}

/**
 * @brief  Demo: Simple digital voltmeter
 * @retval None
 */
void digital_voltmeter_demo(void)
{
    printf("=== Digital Voltmeter Demo ===\n");
    printf("Connect different voltages to PA0 and watch the readings!\n");
    printf("Safe range: 0V to 3.3V only!\n\n");
    
    for (int i = 0; i < 20; i++) {  // Take 20 readings
        float voltage = read_voltage_simple();
        uint16_t raw_value = (uint16_t)(voltage * 4095.0f / 3.3f);
        
        printf("Reading %d: %.3f V (raw ADC value: %d)\n", 
               i+1, voltage, raw_value);
        
        HAL_Delay(500);  // Reading every 500ms
    }
    
    printf("\nVoltmeter demo complete!\n");
}