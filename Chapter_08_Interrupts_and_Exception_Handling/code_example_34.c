/*
 * Code Example 34
 * Language: C
 * Chapter: Chapter_08_Interrupts_and_Exception_Handling
 * Line: 9685
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

// Button interrupt configuration
#define BUTTON_PORT     GPIOC
#define BUTTON_PIN      GPIO_PIN_13
#define BUTTON_EXTI_IRQn EXTI15_10_IRQn

// Interrupt statistics
typedef struct {
    uint32_t total_interrupts;
    uint32_t last_interrupt_time;
    uint32_t min_interval_ms;
    uint32_t max_interval_ms;
    uint32_t spurious_interrupts;
} interrupt_stats_t;

static interrupt_stats_t button_stats = {0};

/**
 * @brief  Configure external interrupt for button with comprehensive setup
 * @retval HAL_StatusTypeDef
 */
HAL_StatusTypeDef configure_button_interrupt(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // Enable GPIO clock
    __HAL_RCC_GPIOC_CLK_ENABLE();
    
    // Configure button pin for interrupt
    GPIO_InitStruct.Pin = BUTTON_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;   // Interrupt on falling edge
    GPIO_InitStruct.Pull = GPIO_NOPULL;            // External pull-up on board
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    
    HAL_GPIO_Init(BUTTON_PORT, &GPIO_InitStruct);
    
    // Configure NVIC
    HAL_NVIC_SetPriority(BUTTON_EXTI_IRQn, 5, 0);  // Medium priority
    HAL_NVIC_EnableIRQ(BUTTON_EXTI_IRQn);
    
    // Initialize statistics
    button_stats.min_interval_ms = 0xFFFFFFFF;
    button_stats.max_interval_ms = 0;
    
    printf("Button interrupt configured on PC13\n");
    return HAL_OK;
}

/**
 * @brief  External interrupt handler with debouncing and statistics
 * @retval None
 */
void EXTI15_10_IRQHandler(void)
{
    // Check if our pin caused the interrupt
    if (__HAL_GPIO_EXTI_GET_IT(BUTTON_PIN) != RESET) {
        
        uint32_t current_time = HAL_GetTick();
        
        // Simple debouncing - ignore interrupts too close together
        if (current_time - button_stats.last_interrupt_time > 50) {
            
            // Update statistics
            if (button_stats.total_interrupts > 0) {
                uint32_t interval = current_time - button_stats.last_interrupt_time;
                
                if (interval < button_stats.min_interval_ms) {
                    button_stats.min_interval_ms = interval;
                }
                if (interval > button_stats.max_interval_ms) {
                    button_stats.max_interval_ms = interval;
                }
            }
            
            button_stats.total_interrupts++;
            button_stats.last_interrupt_time = current_time;
            
            // Signal main application
            handle_button_press_interrupt();
            
        } else {
            // Spurious interrupt (within debounce time)
            button_stats.spurious_interrupts++;
        }
        
        // Clear interrupt flag
        __HAL_GPIO_EXTI_CLEAR_IT(BUTTON_PIN);
    }
}

/**
 * @brief  Handle button press in interrupt context
 * @retval None
 */
void handle_button_press_interrupt(void)
{
    // Keep interrupt handler short and fast
    // Set flag for main loop processing
    static volatile bool button_press_pending = true;
    
    // Toggle LED immediately for user feedback
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    
    // Signal main application (use volatile flag or queue)
    button_press_pending = true;
    
    // Optional: Wake up from sleep mode
    // (automatic with EXTI wake-up capability)
}

/**
 * @brief  Process button press in main application context
 * @retval None
 */
void process_button_press_main(void)
{
    static volatile bool button_press_pending = false;
    
    if (button_press_pending) {
        button_press_pending = false;
        
        // Perform complex processing outside interrupt context
        printf("Button pressed - interrupt #%lu\n", button_stats.total_interrupts);
        printf("Statistics: Min interval: %lu ms, Max interval: %lu ms\n",
               button_stats.min_interval_ms, button_stats.max_interval_ms);
        printf("Spurious interrupts: %lu\n", button_stats.spurious_interrupts);
        
        // Implement application-specific button handling
        handle_user_interface_event();
    }
}