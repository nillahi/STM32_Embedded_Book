/*
 * Code Example 45
 * Language: C
 * Chapter: Chapter_10_ADC_and_DAC_Programming
 * Line: 11658
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

DAC_HandleTypeDef hdac;

/**
 * @brief  Initialize DAC for simple voltage output
 * @retval None
 */
void init_simple_dac(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // Enable clocks
    __HAL_RCC_DAC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    // Configure DAC pin (PA4 = DAC_OUT1)
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;     // Analog mode
    GPIO_InitStruct.Pull = GPIO_NOPULL;          // No pull resistors
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // Configure DAC
    hdac.Instance = DAC;
    HAL_DAC_Init(&hdac);
    
    // Configure DAC channel
    DAC_ChannelConfTypeDef sConfig = {0};
    sConfig.DAC_Trigger = DAC_TRIGGER_NONE;           // Software trigger
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE; // Enable output buffer
    
    HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1);
    
    // Start DAC
    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
    
    printf("Simple DAC initialized - voltage output on PA4!\n");
    printf("Output range: 0V to 3.3V\n");
}

/**
 * @brief  Set DAC output voltage (simple version)
 * @param  voltage: Desired voltage (0.0 to 3.3V)
 * @retval None
 */
void set_dac_voltage(float voltage)
{
    // Clamp voltage to safe range
    if (voltage < 0.0f) voltage = 0.0f;
    if (voltage > 3.3f) voltage = 3.3f;
    
    // Convert voltage to 12-bit DAC value
    uint16_t dac_value = (uint16_t)(voltage * 4095.0f / 3.3f);
    
    // Set DAC output
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_value);
    
    printf("DAC output set to %.3f V (DAC value: %d)\n", voltage, dac_value);
}

/**
 * @brief  Demo: Programmable voltage source
 * @retval None
 */
void voltage_source_demo(void)
{
    printf("=== Programmable Voltage Source Demo ===\n");
    printf("Watch PA4 output voltage change!\n");
    printf("Use a multimeter to verify the voltages.\n\n");
    
    float test_voltages[] = {0.0f, 0.5f, 1.0f, 1.65f, 2.5f, 3.0f, 3.3f};
    int num_voltages = sizeof(test_voltages) / sizeof(test_voltages[0]);
    
    for (int i = 0; i < num_voltages; i++) {
        set_dac_voltage(test_voltages[i]);
        printf("Set voltage to %.1f V - measure with multimeter!\n", test_voltages[i]);
        HAL_Delay(3000);  // Hold voltage for 3 seconds
    }
    
    printf("\nVoltage source demo complete!\n");
    
    // Create a slow sine wave
    printf("Bonus: Creating slow sine wave...\n");
    for (int i = 0; i < 100; i++) {
        float angle = 2.0f * 3.14159f * i / 100.0f;
        float voltage = 1.65f + 1.0f * sinf(angle);  // 1.65V +/- 1V
        set_dac_voltage(voltage);
        HAL_Delay(100);  // 10Hz sine wave
    }
    
    printf("Sine wave complete!\n");
}