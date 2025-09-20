/*
 * Code Example 37
 * Language: C
 * Chapter: Chapter_08_Interrupts_and_Exception_Handling
 * Line: 10145
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

// Pattern 1: Flag-based communication
volatile bool data_ready_flag = false;

void USART2_IRQHandler(void) {
    if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE)) {
        // Read data quickly
        received_data = huart2.Instance->DR;
        data_ready_flag = true;  // Signal main loop
    }
}

// Pattern 2: Circular buffer for high-rate data
#define BUFFER_SIZE 256
volatile uint8_t circular_buffer[BUFFER_SIZE];
volatile uint16_t buffer_head = 0;
volatile uint16_t buffer_tail = 0;

void ADC_IRQHandler(void) {
    if (__HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_EOC)) {
        uint16_t adc_value = HAL_ADC_GetValue(&hadc1);
        
        // Add to circular buffer
        uint16_t next_head = (buffer_head + 1) % BUFFER_SIZE;
        if (next_head != buffer_tail) {
            circular_buffer[buffer_head] = adc_value;
            buffer_head = next_head;
        }
        // If buffer full, oldest data is overwritten
    }
}