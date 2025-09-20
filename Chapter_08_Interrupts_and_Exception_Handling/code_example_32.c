/*
 * Code Example 32
 * Language: C
 * Chapter: Chapter_08_Interrupts_and_Exception_Handling
 * Line: 9423
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

// Configure priority grouping (4 bits preemption, 0 bits sub-priority)
HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

// Set interrupt priorities
HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);      // Highest priority
HAL_NVIC_SetPriority(TIM2_IRQn, 4, 0);       // High priority
HAL_NVIC_SetPriority(USART2_IRQn, 8, 0);     // Medium priority
HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);   // Lowest priority