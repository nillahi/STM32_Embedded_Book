/*
 * Code Example 13
 * Language: C
 * Chapter: Chapter_04_texorpdfstringparboxtextwidth
 * Line: 4758
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

/*
 * CubeMX Configuration Settings:
 * 
 * System Core:
 * - RCC: HSE crystal, PLL to 84MHz
 * - SYS: Serial Wire Debug enabled
 * - NVIC: SysTick and EXTI interrupts enabled
 * 
 * Peripherals:
 * - GPIO: PA5 (LED), PC13 (Button with interrupt)
 * - TIM2: 1Hz timer for LED blinking
 * - USART2: 115200 baud for debug output
 * 
 * Clock Configuration:
 * - HSE: 8MHz external crystal
 * - PLL: M=8, N=336, P=4 (84MHz system clock)
 * - AHB: 84MHz, APB1: 42MHz, APB2: 84MHz
 */