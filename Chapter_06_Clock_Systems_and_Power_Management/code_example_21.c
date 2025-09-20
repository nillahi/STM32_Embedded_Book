/*
 * Code Example 21
 * Language: C
 * Chapter: Chapter_06_Clock_Systems_and_Power_Management
 * Line: 6881
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

// HSE = 8MHz, Target = 168MHz
// M = 8 -> 8MHz/8 = 1MHz (input to PLL)
// N = 336 -> 1MHz * 336 = 336MHz (VCO)
// P = 2 -> 336MHz/2 = 168MHz (system clock)

RCC->PLLCFGR = (8 << RCC_PLLCFGR_PLLM_Pos) |     // M = 8
               (336 << RCC_PLLCFGR_PLLN_Pos) |    // N = 336
               (0 << RCC_PLLCFGR_PLLP_Pos) |      // P = 2
               (7 << RCC_PLLCFGR_PLLQ_Pos) |      // Q = 7 (for USB)
               RCC_PLLCFGR_PLLSRC_HSE;             // HSE source