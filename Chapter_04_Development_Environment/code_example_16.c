/*
 * Code Example 16
 * Language: C
 * Chapter: Chapter_04_texorpdfstringparboxtextwidth
 * Line: 5056
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
 * @brief  Configure UART peripheral for debug communication
 * @param  huart: Pointer to UART handle structure
 * @param  baudrate: Communication speed in bits per second
 * @retval HAL_StatusTypeDef: HAL_OK if successful, error code otherwise
 *
 * @note   This function assumes UART pins are already configured
 * @warning Ensure adequate system clock frequency for desired baudrate
 */
HAL_StatusTypeDef configure_debug_uart(UART_HandleTypeDef *huart,
                                      uint32_t baudrate)
{
    // Implementation with error checking
    if (huart == NULL) {
        return HAL_ERROR;
    }

    // Configure UART parameters
    huart->Init.BaudRate = baudrate;
    huart->Init.WordLength = UART_WORDLENGTH_8B;
    huart->Init.StopBits = UART_STOPBITS_1;
    huart->Init.Parity = UART_PARITY_NONE;

    return HAL_UART_Init(huart);
}