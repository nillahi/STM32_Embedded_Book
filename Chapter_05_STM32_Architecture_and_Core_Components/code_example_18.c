/*
 * Code Example 18
 * Language: C
 * Chapter: Chapter_05_STM32_Architecture_and_Core_Components
 * Line: 5809
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

// MSI: 4MHz internal oscillator
// No PLL, direct MSI to system clock
// Optimized for battery-powered applications

RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
RCC_OscInitStruct.MSIState = RCC_MSI_ON;
RCC_OscInitStruct.MSICalibrationValue = 0;
RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6; // 4MHz
RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;
HAL_RCC_OscConfig(&RCC_OscInitStruct);