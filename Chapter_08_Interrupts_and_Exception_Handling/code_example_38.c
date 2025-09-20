/*
 * Code Example 38
 * Language: C
 * Chapter: Chapter_08_Interrupts_and_Exception_Handling
 * Line: 10183
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

// Shared data structure
typedef struct {
    volatile uint32_t counter;
    volatile float sensor_value;
    volatile bool data_valid;
} shared_data_t;

static shared_data_t shared_data = {0};

/**
 * @brief  Enter critical section (disable interrupts)
 * @retval uint32_t: Previous interrupt state (for restoration)
 */
uint32_t enter_critical_section(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    return primask;
}

/**
 * @brief  Exit critical section (restore interrupt state)
 * @param  primask: Previous interrupt state from enter_critical_section()
 * @retval None
 */
void exit_critical_section(uint32_t primask)
{
    __set_PRIMASK(primask);
}

/**
 * @brief  Thread-safe data update using critical sections
 * @param  new_value: New sensor value
 * @retval None
 */
void update_sensor_data_safe(float new_value)
{
    uint32_t critical_state = enter_critical_section();
    
    // Update shared data atomically
    shared_data.sensor_value = new_value;
    shared_data.counter++;
    shared_data.data_valid = true;
    
    exit_critical_section(critical_state);
}

/**
 * @brief  Thread-safe data reading using critical sections
 * @param  value: Pointer to store sensor value
 * @param  counter: Pointer to store counter value
 * @retval bool: true if data is valid
 */
bool read_sensor_data_safe(float *value, uint32_t *counter)
{
    uint32_t critical_state = enter_critical_section();
    
    bool valid = shared_data.data_valid;
    if (valid) {
        *value = shared_data.sensor_value;
        *counter = shared_data.counter;
    }
    
    exit_critical_section(critical_state);
    
    return valid;
}

/**
 * @brief  High-frequency interrupt handler with minimal processing
 * @retval None
 */
void TIM3_IRQHandler(void)
{
    if (__HAL_TIM_GET_IT_SOURCE(&htim3, TIM_IT_UPDATE) != RESET) {
        if (__HAL_TIM_GET_FLAG(&htim3, TIM_FLAG_UPDATE) != RESET) {
            
            // Clear interrupt flag immediately
            __HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE);
            
            // Minimal processing in ISR
            shared_data.counter++;
            
            // Signal main loop for complex processing
            // Use hardware semaphore or flag
            
        }
    }
}

/**
 * @brief  Demonstrate interrupt latency measurement
 * @retval None
 */
void measure_interrupt_latency(void)
{
    // Configure GPIO pin to toggle in ISR
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    // Enable DWT cycle counter
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    
    printf("Interrupt latency measurement configured\n");
    printf("Use oscilloscope to measure time between trigger and PB0 toggle\n");
}