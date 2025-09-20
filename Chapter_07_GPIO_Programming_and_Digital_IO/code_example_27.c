/*
 * Code Example 27
 * Language: C
 * Chapter: Chapter_07_GPIO_Programming_and_Digital_IO
 * Line: 8083
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

// Button definitions for NUCLEO-F401RE
#define USER_BUTTON_PORT    GPIOC
#define USER_BUTTON_PIN     GPIO_PIN_13

// Button state machine
typedef enum {
    BUTTON_IDLE,
    BUTTON_PRESSED,
    BUTTON_HELD,
    BUTTON_RELEASED
} button_state_t;

typedef struct {
    button_state_t state;
    uint32_t press_time;
    uint32_t release_time;
    uint32_t debounce_time;
    bool long_press_detected;
    uint8_t press_count;
} button_t;

static button_t user_button = {
    .state = BUTTON_IDLE,
    .debounce_time = 50,  // 50ms debounce
    .long_press_detected = false,
    .press_count = 0
};

/**
 * @brief  Initialize button GPIO with proper configuration
 * @retval HAL_StatusTypeDef
 */
HAL_StatusTypeDef button_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // Enable GPIO clock
    __HAL_RCC_GPIOC_CLK_ENABLE();
    
    // Configure button pin
    GPIO_InitStruct.Pin = USER_BUTTON_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;  // External pull-up on NUCLEO board
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    
    HAL_GPIO_Init(USER_BUTTON_PORT, &GPIO_InitStruct);
    
    return HAL_OK;
}

/**
 * @brief  Read button state with debouncing
 * @retval bool: true if button is pressed (debounced)
 */
bool button_read_debounced(void)
{
    static uint32_t last_read_time = 0;
    static bool last_state = false;
    static bool stable_state = false;
    
    uint32_t current_time = HAL_GetTick();
    bool current_reading = (HAL_GPIO_ReadPin(USER_BUTTON_PORT, USER_BUTTON_PIN) == GPIO_PIN_RESET);
    
    // Check if state changed
    if (current_reading != last_state) {
        last_read_time = current_time;
        last_state = current_reading;
    }
    
    // Check if state has been stable for debounce time
    if ((current_time - last_read_time) > user_button.debounce_time) {
        stable_state = current_reading;
    }
    
    return stable_state;
}

/**
 * @brief  Button state machine with advanced features
 * @retval button_state_t: Current button state
 */
button_state_t button_update_state_machine(void)
{
    bool button_pressed = button_read_debounced();
    uint32_t current_time = HAL_GetTick();
    
    switch (user_button.state) {
        case BUTTON_IDLE:
            if (button_pressed) {
                user_button.state = BUTTON_PRESSED;
                user_button.press_time = current_time;
                user_button.press_count++;
                printf("Button pressed (count: %d)\n", user_button.press_count);
            }
            break;
            
        case BUTTON_PRESSED:
            if (!button_pressed) {
                user_button.state = BUTTON_RELEASED;
                user_button.release_time = current_time;
                
                uint32_t press_duration = current_time - user_button.press_time;
                printf("Button released after %lu ms\n", press_duration);
                
            } else if ((current_time - user_button.press_time) > 2000) {
                // Long press detected (2 seconds)
                user_button.state = BUTTON_HELD;
                user_button.long_press_detected = true;
                printf("Long press detected\n");
            }
            break;
            
        case BUTTON_HELD:
            if (!button_pressed) {
                user_button.state = BUTTON_RELEASED;
                user_button.release_time = current_time;
                printf("Long press released\n");
            }
            break;
            
        case BUTTON_RELEASED:
            // Transition back to idle after processing
            user_button.state = BUTTON_IDLE;
            user_button.long_press_detected = false;
            break;
    }
    
    return user_button.state;
}

/**
 * @brief  Handle button events based on press patterns
 * @retval None
 */
void handle_button_events(void)
{
    button_state_t current_state = button_update_state_machine();
    
    switch (current_state) {
        case BUTTON_RELEASED:
            if (user_button.long_press_detected) {
                // Handle long press event
                printf("Action: Long press detected - entering configuration mode\n");
                // Implement configuration mode logic
                
            } else {
                // Handle short press event
                uint32_t press_duration = user_button.release_time - user_button.press_time;
                
                if (press_duration < 100) {
                    printf("Action: Quick press - toggle LED\n");
                    led_control(LED_TOGGLE);
                    
                } else if (press_duration < 500) {
                    printf("Action: Normal press - change blink rate\n");
                    // Cycle through blink rates
                    
                } else {
                    printf("Action: Medium press - send status\n");
                    // Send system status
                }
            }
            break;
            
        default:
            // No action needed for other states
            break;
    }
}