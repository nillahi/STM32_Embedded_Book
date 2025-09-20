/*
 * Code Example 12
 * Language: C
 * Chapter: Chapter_04_texorpdfstringparboxtextwidth
 * Line: 4727
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

// Problem: Variables optimized away in Release build
// Solution: Use volatile keyword for hardware registers
volatile uint32_t *GPIO_ODR = (uint32_t*)0x40020014;

// Or use proper HAL functions that handle optimization
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);