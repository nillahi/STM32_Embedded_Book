/*
 * Code Example 41
 * Language: C
 * Chapter: Chapter_09_Timers_and_PWM_Generation
 * Line: 10725
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

TIM_HandleTypeDef htim2;  // Our timer handle

/**
 * @brief  Initialize timer for LED blinking (much better than delays!)
 * @retval None
 */
void init_led_blink_timer(void)
{
    // Enable timer clock
    __HAL_RCC_TIM2_CLK_ENABLE();
    
    // Configure timer for 2 Hz (blink every 500ms)
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 8399;         // 84MHz / 8400 = 10kHz
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 4999;            // 10kHz / 5000 = 2Hz
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    
    HAL_TIM_Base_Init(&htim2);
    
    // Set up interrupt (this is the magic!)
    HAL_NVIC_SetPriority(TIM2_IRQn, 10, 0);  // Low priority
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
    
    // Start the timer
    HAL_TIM_Base_Start_IT(&htim2);
    
    printf("LED blink timer started - no more blocking delays!\n");
}

/**
 * @brief  Timer interrupt - automatically called every 500ms
 * @retval None
 */
void TIM2_IRQHandler(void)
{
    if (__HAL_TIM_GET_IT(&htim2, TIM_IT_UPDATE) != RESET) {
        __HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);
        
        // Toggle LED automatically every 500ms!
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        
        static uint32_t blink_count = 0;
        blink_count++;
        
        if (blink_count % 10 == 0) {  // Every 5 seconds
            printf("LED blinked %lu times - timer working perfectly!\n", blink_count);
        }
    }
}

/**
 * @brief  Main program - now free to do other things!
 * @retval None
 */
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    
    // Initialize LED and timer
    init_led_gpio();
    init_led_blink_timer();
    
    printf("Timer-based LED blink started!\n");
    printf("Notice: main program is free to do other tasks!\n");
    
    // Main loop can now do other important work
    while (1) {
        // The LED blinks automatically thanks to the timer!
        // Meanwhile, we can do other tasks:
        
        printf("Main program running... LED still blinking automatically!\n");
        HAL_Delay(2000);  // This delay doesn't affect LED blinking!
        
        // Could read sensors, handle communication, process data, etc.
        // The timer interrupt handles LED blinking independently!
    }
}