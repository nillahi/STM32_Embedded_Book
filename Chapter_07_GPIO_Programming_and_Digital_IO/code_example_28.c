/*
 * Code Example 28
 * Language: C
 * Chapter: Chapter_07_GPIO_Programming_and_Digital_IO
 * Line: 8332
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
 * @brief  Configure GPIO pin for UART alternate function
 * @param  port: GPIO port (e.g., GPIOA)
 * @param  pin: GPIO pin (e.g., GPIO_PIN_9)
 * @param  af_number: Alternate function number (e.g., GPIO_AF7_USART1)
 * @retval None
 */
void configure_uart_gpio(GPIO_TypeDef *port, uint16_t pin, uint8_t af_number)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;        // Alternate function push-pull
    GPIO_InitStruct.Pull = GPIO_NOPULL;            // No pull resistors
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  // High speed for clean signals
    GPIO_InitStruct.Alternate = af_number;         // Specific AF selection
    
    HAL_GPIO_Init(port, &GPIO_InitStruct);
}

/**
 * @brief  Configure multiple pins for SPI interface
 * @retval None
 */
void configure_spi_gpio(void)
{
    // Enable GPIO clocks
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // Configure SPI1 pins (PA5=SCK, PA6=MISO, PA7=MOSI)
    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  // Maximum speed for SPI
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // Configure chip select pin as regular GPIO output
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // Initialize CS high (inactive)
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}

/**
 * @brief  Dynamic alternate function switching
 * @param  port: GPIO port
 * @param  pin: GPIO pin
 * @param  new_af: New alternate function
 * @retval None
 */
void switch_alternate_function(GPIO_TypeDef *port, uint16_t pin, uint8_t new_af)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // Read current configuration
    uint32_t current_mode = (port->MODER >> (POSITION_VAL(pin) * 2)) & 0x3;
    
    if (current_mode == 0x2) {  // Currently in AF mode
        // Reconfigure with new alternate function
        GPIO_InitStruct.Pin = pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = new_af;
        
        HAL_GPIO_Init(port, &GPIO_InitStruct);
        
        printf("Pin AF switched to AF%d\n", new_af);
    } else {
        printf("ERROR: Pin not in AF mode\n");
    }
}