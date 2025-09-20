/*
 * Code Example 30
 * Language: C
 * Chapter: Chapter_07_GPIO_Programming_and_Digital_IO
 * Line: 8575
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

// LED configuration structure
typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
    bool state;
    uint32_t last_toggle;
    uint16_t blink_rate_ms;
} led_config_t;

// System status enumeration
typedef enum {
    STATUS_INITIALIZING = 0,
    STATUS_READY = 1,
    STATUS_RUNNING = 2,
    STATUS_ERROR = 3,
    STATUS_MAINTENANCE = 4,
    STATUS_SHUTDOWN = 5
} system_status_t;

// LED array configuration
static led_config_t status_leds[8] = {
    {GPIOA, GPIO_PIN_0, false, 0, 0},    // Power LED
    {GPIOA, GPIO_PIN_1, false, 0, 500},  // Status LED
    {GPIOA, GPIO_PIN_2, false, 0, 250},  // Activity LED
    {GPIOA, GPIO_PIN_3, false, 0, 100},  // Error LED
    {GPIOA, GPIO_PIN_4, false, 0, 1000}, // Communication LED
    {GPIOA, GPIO_PIN_5, false, 0, 0},    // User LED
    {GPIOA, GPIO_PIN_6, false, 0, 2000}, // Maintenance LED
    {GPIOA, GPIO_PIN_7, false, 0, 0}     // Reserved LED
};

static system_status_t current_status = STATUS_INITIALIZING;

/**
 * @brief  Initialize all status LEDs
 * @retval None
 */
void status_display_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // Enable GPIO clock
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    // Configure all LED pins as outputs
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                         GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;  // LEDs don't need high speed
    
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // Initialize all LEDs to OFF state
    for (int i = 0; i < 8; i++) {
        HAL_GPIO_WritePin(status_leds[i].port, status_leds[i].pin, GPIO_PIN_RESET);
        status_leds[i].state = false;
    }
    
    printf("Status display initialized\n");
}

/**
 * @brief  Update LED display based on system status
 * @param  status: Current system status
 * @retval None
 */
void update_status_display(system_status_t status)
{
    uint32_t current_time = HAL_GetTick();
    current_status = status;
    
    // Status-specific LED patterns
    switch (status) {
        case STATUS_INITIALIZING:
            // Sequential LED chase during initialization
            {
                static uint8_t chase_position = 0;
                static uint32_t last_chase_time = 0;
                
                if (current_time - last_chase_time > 100) {
                    // Turn off all LEDs
                    for (int i = 0; i < 8; i++) {
                        HAL_GPIO_WritePin(status_leds[i].port, status_leds[i].pin, GPIO_PIN_RESET);
                    }
                    
                    // Turn on current position
                    HAL_GPIO_WritePin(status_leds[chase_position].port, 
                                     status_leds[chase_position].pin, GPIO_PIN_SET);
                    
                    chase_position = (chase_position + 1) % 8;
                    last_chase_time = current_time;
                }
            }
            break;
            
        case STATUS_READY:
            // Power LED solid ON, others OFF
            HAL_GPIO_WritePin(status_leds[0].port, status_leds[0].pin, GPIO_PIN_SET);
            for (int i = 1; i < 8; i++) {
                HAL_GPIO_WritePin(status_leds[i].port, status_leds[i].pin, GPIO_PIN_RESET);
            }
            break;
            
        case STATUS_RUNNING:
            // Power LED ON, Activity LED blinking
            HAL_GPIO_WritePin(status_leds[0].port, status_leds[0].pin, GPIO_PIN_SET);
            
            // Blink activity LED
            if (current_time - status_leds[2].last_toggle > status_leds[2].blink_rate_ms) {
                HAL_GPIO_TogglePin(status_leds[2].port, status_leds[2].pin);
                status_leds[2].last_toggle = current_time;
            }
            break;
            
        case STATUS_ERROR:
            // All LEDs blink rapidly in error pattern
            for (int i = 0; i < 8; i++) {
                if (current_time - status_leds[i].last_toggle > 100) {
                    HAL_GPIO_TogglePin(status_leds[i].port, status_leds[i].pin);
                    status_leds[i].last_toggle = current_time;
                }
            }
            break;
            
        case STATUS_MAINTENANCE:
            // Alternating LED pattern
            {
                static bool alternate_state = false;
                static uint32_t last_alternate_time = 0;
                
                if (current_time - last_alternate_time > 500) {
                    for (int i = 0; i < 8; i++) {
                        if ((i % 2) == (alternate_state ? 1 : 0)) {
                            HAL_GPIO_WritePin(status_leds[i].port, status_leds[i].pin, GPIO_PIN_SET);
                        } else {
                            HAL_GPIO_WritePin(status_leds[i].port, status_leds[i].pin, GPIO_PIN_RESET);
                        }
                    }
                    alternate_state = !alternate_state;
                    last_alternate_time = current_time;
                }
            }
            break;
            
        case STATUS_SHUTDOWN:
            // Fade out all LEDs
            for (int i = 0; i < 8; i++) {
                HAL_GPIO_WritePin(status_leds[i].port, status_leds[i].pin, GPIO_PIN_RESET);
            }
            break;
    }
}

/**
 * @brief  Display binary value on LED array
 * @param  value: 8-bit value to display
 * @retval None
 */
void display_binary_value(uint8_t value)
{
    for (int i = 0; i < 8; i++) {
        if (value & (1 << i)) {
            HAL_GPIO_WritePin(status_leds[i].port, status_leds[i].pin, GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(status_leds[i].port, status_leds[i].pin, GPIO_PIN_RESET);
        }
    }
}