/*
 * Code Example 25
 * Language: C
 * Chapter: Chapter_06_Clock_Systems_and_Power_Management
 * Line: 7715
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

/**
 * @brief  Configure Clock Security System for robust operation
 * @retval None
 */
void configure_clock_security(void)
{
    // Enable Clock Security System
    HAL_RCC_EnableCSS();
    
    // Configure CSS interrupt
    HAL_NVIC_SetPriority(RCC_IRQn, 0, 0);  // Highest priority
    HAL_NVIC_EnableIRQ(RCC_IRQn);
    
    printf("Clock Security System enabled\n");
}

/**
 * @brief  RCC interrupt handler - handles clock failures
 * @retval None
 */
void RCC_IRQHandler(void)
{
    if (__HAL_RCC_GET_IT(RCC_IT_CSS)) {
        // HSE failure detected
        printf("ERROR: HSE clock failure detected!\n");
        
        // Clear the interrupt flag
        __HAL_RCC_CLEAR_IT(RCC_IT_CSS);
        
        // System automatically switches to HSI
        // Implement recovery strategy
        handle_clock_failure();
    }
}

/**
 * @brief  Handle clock failure with graceful degradation
 * @retval None
 */
void handle_clock_failure(void)
{
    // System is now running on HSI (16MHz)
    SystemCoreClock = 16000000;
    
    // Notify application of reduced performance
    printf("WARNING: Operating on backup HSI clock\n");
    printf("Performance reduced to 16MHz\n");
    
    // Adjust application behavior
    // - Reduce communication speeds
    // - Disable non-essential features
    // - Attempt HSE recovery
    
    // Try to restart HSE
    for (int attempts = 0; attempts < 3; attempts++) {
        HAL_Delay(1000);  // Wait 1 second
        
        if (attempt_hse_restart() == HAL_OK) {
            printf("HSE recovery successful\n");
            SystemClock_Config();  // Restore full performance
            return;
        }
    }
    
    printf("HSE recovery failed - continuing on HSI\n");
    // Log failure for maintenance
    // Continue operation at reduced performance
}

/**
 * @brief  Attempt to restart HSE oscillator
 * @retval HAL_StatusTypeDef
 */
HAL_StatusTypeDef attempt_hse_restart(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    
    // Disable HSE first
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);
    
    HAL_Delay(100);  // Allow oscillator to stop
    
    // Try to restart HSE
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    return HAL_RCC_OscConfig(&RCC_OscInitStruct);
}