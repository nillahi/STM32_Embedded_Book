/*
 * Code Example 10
 * Language: bash
 * Chapter: Chapter_04_texorpdfstringparboxtextwidth
 * Line: 4711
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

# Error: region 'FLASH' overflowed by X bytes
# Solution: Check memory definitions in linker script
# Verify FLASH and RAM sizes match your MCU
# Consider optimization or code size reduction