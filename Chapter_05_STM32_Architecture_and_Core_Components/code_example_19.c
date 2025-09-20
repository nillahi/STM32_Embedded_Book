/*
 * Code Example 19
 * Language: C
 * Chapter: Chapter_05_STM32_Architecture_and_Core_Components
 * Line: 6101
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
#include <stdio.h>

// Test variables in different memory regions
__attribute__((section(".ccmram"))) volatile uint32_t ccm_data[1000];
volatile uint32_t sram_data[1000];
const uint32_t flash_data[1000] = {0}; // Initialized in Flash

/**
 * @brief  Measure memory access timing for different regions
 * @retval None
 */
void measure_memory_timing(void)
{
    uint32_t start_time, end_time, cycles;
    
    // Enable DWT cycle counter
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    DWT->CYCCNT = 0;
    
    printf("Memory Access Timing Analysis (168MHz STM32F4)\n");
    printf("===========================================\n");
    
    // Test CCM SRAM access
    DWT->CYCCNT = 0;
    for (int i = 0; i < 1000; i++) {
        ccm_data[i] = i;
    }
    cycles = DWT->CYCCNT;
    printf("CCM SRAM write (1000 words): %lu cycles (%.2f cycles/word)\n", 
           cycles, (float)cycles/1000);
    
    // Test main SRAM access
    DWT->CYCCNT = 0;
    for (int i = 0; i < 1000; i++) {
        sram_data[i] = i;
    }
    cycles = DWT->CYCCNT;
    printf("Main SRAM write (1000 words): %lu cycles (%.2f cycles/word)\n", 
           cycles, (float)cycles/1000);
    
    // Test Flash read access
    DWT->CYCCNT = 0;
    uint32_t sum = 0;
    for (int i = 0; i < 1000; i++) {
        sum += flash_data[i];
    }
    cycles = DWT->CYCCNT;
    printf("Flash read (1000 words): %lu cycles (%.2f cycles/word)\n", 
           cycles, (float)cycles/1000);
    
    // Test cross-memory transfer
    DWT->CYCCNT = 0;
    for (int i = 0; i < 1000; i++) {
        sram_data[i] = ccm_data[i];
    }
    cycles = DWT->CYCCNT;
    printf("CCM to SRAM transfer: %lu cycles (%.2f cycles/word)\n", 
           cycles, (float)cycles/1000);
}