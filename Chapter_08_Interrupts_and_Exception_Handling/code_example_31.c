/*
 * Code Example 31
 * Language: C
 * Chapter: Chapter_08_Interrupts_and_Exception_Handling
 * Line: 9081
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

// LED and button definitions
#define MAIN_LED_PORT     GPIOA
#define MAIN_LED_PIN      GPIO_PIN_5   // Built-in LED on NUCLEO
#define INTERRUPT_LED_PORT GPIOA  
#define INTERRUPT_LED_PIN GPIO_PIN_6   // Additional LED for interrupt

#define BUTTON_PORT       GPIOC
#define BUTTON_PIN        GPIO_PIN_13  // Built-in button on NUCLEO

// Global flag to communicate between interrupt and main program
volatile bool button_was_pressed = false;

/**
 * @brief  Initialize all GPIOs for our interrupt example
 * @retval None
 */
void init_interrupt_example(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // Enable GPIO clocks
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    
    // Configure LEDs as outputs
    GPIO_InitStruct.Pin = MAIN_LED_PIN | INTERRUPT_LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // Configure button for interrupt (this is the magic!)
    GPIO_InitStruct.Pin = BUTTON_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;  // Interrupt on button press
    GPIO_InitStruct.Pull = GPIO_NOPULL;           // Button has external pull-up
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BUTTON_PORT, &GPIO_InitStruct);
    
    // Configure the interrupt in NVIC (this tells the CPU to listen)
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);  // Medium priority
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);          // Enable the interrupt
    
    // Turn off both LEDs initially
    HAL_GPIO_WritePin(MAIN_LED_PORT, MAIN_LED_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(INTERRUPT_LED_PORT, INTERRUPT_LED_PIN, GPIO_PIN_RESET);
    
    printf("Interrupt example initialized!\n");
    printf("Main LED will blink slowly, press button for instant response\n");
}

/**
 * @brief  This is the interrupt handler - it runs when button is pressed
 * @retval None
 */
void EXTI15_10_IRQHandler(void)
{
    // Check if our button caused this interrupt
    if (__HAL_GPIO_EXTI_GET_IT(BUTTON_PIN) != RESET) {
        
        // RULE 1: Keep interrupt handlers SHORT and FAST
        
        // Toggle the interrupt LED immediately
        HAL_GPIO_TogglePin(INTERRUPT_LED_PORT, INTERRUPT_LED_PIN);
        
        // Set a flag to tell main program what happened
        button_was_pressed = true;
        
        // RULE 2: Always clear the interrupt flag
        __HAL_GPIO_EXTI_CLEAR_IT(BUTTON_PIN);
        
        // That's it! Keep it simple and fast
    }
}

/**
 * @brief  Main program with interrupt handling
 * @retval None
 */
int main(void)
{
    // Standard STM32 initialization
    HAL_Init();
    SystemClock_Config();
    
    // Initialize our interrupt example
    init_interrupt_example();
    
    uint32_t last_blink_time = 0;
    uint32_t button_press_count = 0;
    
    printf("Starting interrupt demonstration...\n");
    
    // Main program loop
    while (1) {
        // Main program: blink LED slowly (every 1 second)
        if (HAL_GetTick() - last_blink_time > 1000) {
            HAL_GPIO_TogglePin(MAIN_LED_PORT, MAIN_LED_PIN);
            last_blink_time = HAL_GetTick();
            printf("Main program running... (press button for interrupt)\n");
        }
        
        // Check if button was pressed (interrupt set this flag)
        if (button_was_pressed) {
            button_was_pressed = false;  // Clear the flag
            button_press_count++;
            
            // Do complex processing here (outside interrupt)
            printf("*** BUTTON INTERRUPT DETECTED! Count: %lu ***\n", button_press_count);
            printf("This message is printed in main program, not interrupt!\n");
            
            // Maybe do something based on button press
            if (button_press_count % 5 == 0) {
                printf("Special action every 5th press!\n");
            }
        }
        
        // Main program can do other things here
        // The interrupt will still work instantly!
    }
}