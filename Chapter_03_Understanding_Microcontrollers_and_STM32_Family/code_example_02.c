/*
 * Code Example 2
 * Language: C
 * Chapter: Chapter_03_Understanding_Microcontrollers_and_STM32_Family
 * Line: 3195
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

Address offset: 0x14
Reset value: 0x0000 0000
Access: Read/Write

Bits 31:16 Reserved, must be kept at reset value
Bits 15:0 ODRy: Port output data (y = 0..15)
  0: Output low level
  1: Output high level

Note: Writing to ODR affects all 16 bits simultaneously.
For atomic bit operations, use BSRR register instead.