/*
 * Code Example 6
 * Language: C
 * Chapter: Chapter_04_texorpdfstringparboxtextwidth
 * Line: 4430
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

// Variables marked as volatile for live monitoring
volatile uint32_t system_tick_counter;
volatile float sensor_temperature;
volatile uint8_t system_state;

// ITM printf for trace output
#include <stdio.h>

int _write(int file, char *ptr, int len) {
    for (int i = 0; i < len; i++) {
        ITM_SendChar((*ptr++));
    }
    return len;
}

// Usage in application
printf("Temperature: %.2f\\ensuremath{^\\circ}C, State: %d\\n", 
       sensor_temperature, system_state);