/*
 * Code Example 17
 * Language: C
 * Chapter: Chapter_05_STM32_Architecture_and_Core_Components
 * Line: 5790
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

// HSE: 8MHz external crystal
// PLL: M=8, N=336, P=2, Q=7
// SYSCLK: 168MHz, AHB: 168MHz, APB1: 42MHz, APB2: 84MHz

RCC_OscInitTypeDef RCC_OscInitStruct = {0};
RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
RCC_OscInitStruct.HSEState = RCC_HSE_ON;
RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
RCC_OscInitStruct.PLL.PLLM = 8;   // Input: 8MHz/8 = 1MHz
RCC_OscInitStruct.PLL.PLLN = 336; // VCO: 1MHz*336 = 336MHz
RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2; // Output: 336MHz/2 = 168MHz
HAL_RCC_OscConfig(&RCC_OscInitStruct);