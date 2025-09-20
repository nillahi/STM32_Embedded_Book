/*
 * Code Example 23
 * Language: C
 * Chapter: Chapter_06_Clock_Systems_and_Power_Management
 * Line: 7287
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

// Power monitoring structure
typedef struct {
    uint32_t run_time_ms;
    uint32_t sleep_time_ms;
    uint32_t stop_time_ms;
    uint32_t wake_events;
    float estimated_battery_life_hours;
} power_monitor_t;

static power_monitor_t power_stats = {0};

/**
 * @brief  Enter optimized sleep mode with wake-up monitoring
 * @param  wake_source: Expected wake-up source
 * @retval None
 */
void enter_optimized_sleep(uint32_t expected_sleep_ms)
{
    uint32_t sleep_start = HAL_GetTick();
    
    // Disable unnecessary peripherals
    __HAL_RCC_GPIOB_CLK_DISABLE();
    __HAL_RCC_GPIOC_CLK_DISABLE();
    __HAL_RCC_TIM3_CLK_DISABLE();
    
    // Configure wake-up sources
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
    
    // Enter sleep mode
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
    
    // Calculate actual sleep time
    uint32_t actual_sleep = HAL_GetTick() - sleep_start;
    power_stats.sleep_time_ms += actual_sleep;
    power_stats.wake_events++;
    
    // Re-enable peripherals
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    
    printf("Sleep: expected %lu ms, actual %lu ms\n", 
           expected_sleep_ms, actual_sleep);
}

/**
 * @brief  Calculate estimated battery life based on usage patterns
 * @param  battery_capacity_mah: Battery capacity in mAh
 * @retval Estimated battery life in hours
 */
float calculate_battery_life(uint16_t battery_capacity_mah)
{
    // Power consumption estimates (STM32F4 at 3.3V)
    const float run_current_ma = 50.0f;     // 50mA at full speed
    const float sleep_current_ma = 1.5f;    // 1.5mA in sleep mode
    const float stop_current_ma = 0.01f;    // 10$\mu$A in stop mode
    
    // Calculate total runtime
    uint32_t total_time = power_stats.run_time_ms + 
                         power_stats.sleep_time_ms + 
                         power_stats.stop_time_ms;
    
    if (total_time == 0) return 0;
    
    // Calculate weighted average current
    float avg_current = (run_current_ma * power_stats.run_time_ms +
                        sleep_current_ma * power_stats.sleep_time_ms +
                        stop_current_ma * power_stats.stop_time_ms) / total_time;
    
    // Battery life calculation
    float battery_life_hours = (float)battery_capacity_mah / avg_current;
    
    power_stats.estimated_battery_life_hours = battery_life_hours;
    
    printf("Power Analysis:\n");
    printf("  Average current: %.2f mA\n", avg_current);
    printf("  Estimated battery life: %.1f hours (%.1f days)\n", 
           battery_life_hours, battery_life_hours / 24);
    
    return battery_life_hours;
}

/**
 * @brief  Implement intelligent power management
 * @retval None
 */
void intelligent_power_management(void)
{
    static uint32_t last_activity = 0;
    uint32_t idle_time = HAL_GetTick() - last_activity;
    
    // Activity detection
    if (/* sensor activity or user input */) {
        last_activity = HAL_GetTick();
        
        // Ensure full performance for active periods
        if (SystemCoreClock < 168000000) {
            // Switch to high-performance mode
            SystemClock_Config_168MHz();
        }
    }
    
    // Progressive power reduction based on idle time
    if (idle_time > 10000) {  // 10 seconds idle
        // Enter stop mode
        printf("Entering stop mode due to inactivity\n");
        HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
        
        // Wake-up handling
        SystemClock_Config();  // Restore clock after wake-up
        printf("Woke up from stop mode\n");
        
    } else if (idle_time > 1000) {  // 1 second idle
        // Reduce clock frequency
        if (SystemCoreClock > 16000000) {
            SystemClock_Config_16MHz();
        }
        
        // Enter sleep mode
        HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
    }
}