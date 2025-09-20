/*
 * Code Example 22
 * Language: C
 * Chapter: Chapter_06_Clock_Systems_and_Power_Management
 * Line: 6970
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

/**
 * @brief  Power-optimized peripheral clock management
 * @retval None
 */
typedef struct {
    uint32_t active_peripherals;
    uint32_t ahb1_clocks;
    uint32_t ahb2_clocks;
    uint32_t apb1_clocks;
    uint32_t apb2_clocks;
} clock_manager_t;

static clock_manager_t clock_mgr = {0};

/**
 * @brief  Enable peripheral clock with usage tracking
 * @param  peripheral: Peripheral identifier
 * @retval None
 */
void enable_peripheral_clock(uint32_t peripheral)
{
    switch (peripheral) {
        case RCC_AHB1Periph_GPIOA:
            __HAL_RCC_GPIOA_CLK_ENABLE();
            clock_mgr.ahb1_clocks |= RCC_AHB1Periph_GPIOA;
            break;
            
        case RCC_APB1Periph_TIM2:
            __HAL_RCC_TIM2_CLK_ENABLE();
            clock_mgr.apb1_clocks |= RCC_APB1Periph_TIM2;
            break;
            
        case RCC_APB2Periph_USART1:
            __HAL_RCC_USART1_CLK_ENABLE();
            clock_mgr.apb2_clocks |= RCC_APB2Periph_USART1;
            break;
            
        default:
            // Handle other peripherals
            break;
    }
    
    clock_mgr.active_peripherals++;
}

/**
 * @brief  Disable unused peripheral clocks for power saving
 * @retval None
 */
void optimize_peripheral_clocks(void)
{
    // Disable unused AHB1 peripherals
    if (!(clock_mgr.ahb1_clocks & RCC_AHB1Periph_GPIOB)) {
        __HAL_RCC_GPIOB_CLK_DISABLE();
    }
    if (!(clock_mgr.ahb1_clocks & RCC_AHB1Periph_GPIOC)) {
        __HAL_RCC_GPIOC_CLK_DISABLE();
    }
    
    // Disable unused APB peripherals
    if (!(clock_mgr.apb1_clocks & RCC_APB1Periph_TIM3)) {
        __HAL_RCC_TIM3_CLK_DISABLE();
    }
    
    printf("Clock optimization: %lu peripherals active\n", 
           clock_mgr.active_peripherals);
}

/**
 * @brief  Implement adaptive clock scaling based on system load
 * @param  cpu_load_percent: Current CPU utilization (0-100)
 * @retval None
 */
void adaptive_clock_scaling(uint8_t cpu_load_percent)
{
    static uint8_t current_scale = 100; // 100% = full speed
    uint8_t target_scale;
    
    // Determine target clock scale based on load
    if (cpu_load_percent > 80) {
        target_scale = 100;  // Full speed
    } else if (cpu_load_percent > 50) {
        target_scale = 50;   // Half speed
    } else if (cpu_load_percent > 20) {
        target_scale = 25;   // Quarter speed
    } else {
        target_scale = 12;   // Eighth speed (minimum)
    }
    
    // Only change if significant difference
    if (abs(target_scale - current_scale) > 10) {
        switch (target_scale) {
            case 100:
                // Switch to full PLL speed (168MHz)
                HAL_RCC_ClockConfig(&RCC_ClkInitStruct_168MHz, FLASH_LATENCY_5);
                break;
                
            case 50:
                // Reduce AHB prescaler (84MHz)
                HAL_RCC_ClockConfig(&RCC_ClkInitStruct_84MHz, FLASH_LATENCY_2);
                break;
                
            case 25:
                // Further reduce (42MHz)
                HAL_RCC_ClockConfig(&RCC_ClkInitStruct_42MHz, FLASH_LATENCY_1);
                break;
                
            case 12:
                // Switch to HSI direct (16MHz)
                HAL_RCC_ClockConfig(&RCC_ClkInitStruct_16MHz, FLASH_LATENCY_0);
                break;
        }
        
        current_scale = target_scale;
        printf("Clock scaled to %d%% (%lu MHz)\n", 
               target_scale, SystemCoreClock / 1000000);
    }
}