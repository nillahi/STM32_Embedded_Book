/*
 * Code Example 36
 * Language: C
 * Chapter: Chapter_08_Interrupts_and_Exception_Handling
 * Line: 9995
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

// Real-time task structure
typedef struct {
    void (*function)(void);
    uint32_t period_ms;
    uint32_t last_execution;
    bool enabled;
    uint32_t execution_count;
    uint32_t max_execution_time_us;
} rt_task_t;

// Task scheduler
#define MAX_RT_TASKS 8
static rt_task_t rt_tasks[MAX_RT_TASKS];
static uint8_t task_count = 0;

/**
 * @brief  Configure SysTick for 1ms interrupts with scheduling
 * @retval None
 */
void configure_systick_scheduler(void)
{
    // SysTick already configured by HAL_Init() for 1ms
    // We'll use it for task scheduling
    
    printf("SysTick scheduler initialized (1ms tick)\n");
}

/**
 * @brief  Add real-time task to scheduler
 * @param  task_function: Function to execute
 * @param  period_ms: Execution period in milliseconds
 * @retval bool: true if task added successfully
 */
bool add_rt_task(void (*task_function)(void), uint32_t period_ms)
{
    if (task_count >= MAX_RT_TASKS) {
        printf("ERROR: Maximum RT tasks exceeded\n");
        return false;
    }
    
    rt_tasks[task_count].function = task_function;
    rt_tasks[task_count].period_ms = period_ms;
    rt_tasks[task_count].last_execution = HAL_GetTick();
    rt_tasks[task_count].enabled = true;
    rt_tasks[task_count].execution_count = 0;
    rt_tasks[task_count].max_execution_time_us = 0;
    
    printf("RT Task %d added: period=%lu ms\n", task_count, period_ms);
    task_count++;
    
    return true;
}

/**
 * @brief  SysTick interrupt handler with task scheduling
 * @retval None
 */
void SysTick_Handler(void)
{
    // Call HAL SysTick handler first
    HAL_IncTick();
    
    // Execute scheduled tasks
    uint32_t current_time = HAL_GetTick();
    
    for (uint8_t i = 0; i < task_count; i++) {
        if (rt_tasks[i].enabled && 
            (current_time - rt_tasks[i].last_execution) >= rt_tasks[i].period_ms) {
            
            // Measure execution time
            uint32_t start_time = DWT->CYCCNT;
            
            // Execute task
            rt_tasks[i].function();
            
            // Calculate execution time
            uint32_t execution_cycles = DWT->CYCCNT - start_time;
            uint32_t execution_time_us = (execution_cycles * 1000000) / SystemCoreClock;
            
            // Update statistics
            if (execution_time_us > rt_tasks[i].max_execution_time_us) {
                rt_tasks[i].max_execution_time_us = execution_time_us;
            }
            
            rt_tasks[i].last_execution = current_time;
            rt_tasks[i].execution_count++;
        }
    }
}

// Example real-time tasks
void sensor_reading_task(void)
{
    // Read sensors every 100ms
    static uint32_t reading_count = 0;
    
    // Simulate sensor reading
    float temperature = 25.0f + (reading_count % 10);
    reading_count++;
    
    printf("Sensor reading #%lu: %.1f C\n", reading_count, temperature);
}

void communication_task(void)
{
    // Handle communication every 500ms
    static uint32_t comm_count = 0;
    
    comm_count++;
    printf("Communication task #%lu executed\n", comm_count);
}

void housekeeping_task(void)
{
    // Housekeeping every 5 seconds
    printf("Housekeeping: System uptime = %lu seconds\n", HAL_GetTick() / 1000);
    
    // Report task execution statistics
    for (uint8_t i = 0; i < task_count; i++) {
        printf("Task %d: %lu executions, max time: %lu us\n",
               i, rt_tasks[i].execution_count, rt_tasks[i].max_execution_time_us);
    }
}