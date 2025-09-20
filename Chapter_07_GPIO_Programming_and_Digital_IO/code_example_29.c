/*
 * Code Example 29
 * Language: C
 * Chapter: Chapter_07_GPIO_Programming_and_Digital_IO
 * Line: 8424
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

// High-speed GPIO definitions
#define SPI_CLK_PORT    GPIOA
#define SPI_CLK_PIN     GPIO_PIN_5
#define SPI_MOSI_PORT   GPIOA
#define SPI_MOSI_PIN    GPIO_PIN_7
#define SPI_MISO_PORT   GPIOA
#define SPI_MISO_PIN    GPIO_PIN_6
#define SPI_CS_PORT     GPIOA
#define SPI_CS_PIN      GPIO_PIN_4

/**
 * @brief  Initialize high-speed GPIO for bit-banged SPI
 * @retval None
 */
void high_speed_spi_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // Enable GPIO clock
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    // Configure output pins (CLK, MOSI, CS)
    GPIO_InitStruct.Pin = SPI_CLK_PIN | SPI_MOSI_PIN | SPI_CS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  // Maximum speed
    
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // Configure input pin (MISO)
    GPIO_InitStruct.Pin = SPI_MISO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // Initialize to idle state
    HAL_GPIO_WritePin(SPI_CS_PORT, SPI_CS_PIN, GPIO_PIN_SET);    // CS high
    HAL_GPIO_WritePin(SPI_CLK_PORT, SPI_CLK_PIN, GPIO_PIN_RESET); // CLK low
    HAL_GPIO_WritePin(SPI_MOSI_PORT, SPI_MOSI_PIN, GPIO_PIN_RESET); // MOSI low
}

/**
 * @brief  High-speed SPI byte transfer using GPIO bit-banging
 * @param  data: Data byte to transmit
 * @retval uint8_t: Received data byte
 */
uint8_t spi_transfer_byte(uint8_t data)
{
    uint8_t received = 0;
    
    // Assert chip select
    HAL_GPIO_WritePin(SPI_CS_PORT, SPI_CS_PIN, GPIO_PIN_RESET);
    
    // Transfer 8 bits
    for (int bit = 7; bit >= 0; bit--) {
        // Set MOSI data bit
        if (data & (1 << bit)) {
            HAL_GPIO_WritePin(SPI_MOSI_PORT, SPI_MOSI_PIN, GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(SPI_MOSI_PORT, SPI_MOSI_PIN, GPIO_PIN_RESET);
        }
        
        // Clock high (data setup)
        HAL_GPIO_WritePin(SPI_CLK_PORT, SPI_CLK_PIN, GPIO_PIN_SET);
        
        // Small delay for signal propagation (adjust based on requirements)
        __NOP(); __NOP(); __NOP(); __NOP();
        
        // Read MISO
        if (HAL_GPIO_ReadPin(SPI_MISO_PORT, SPI_MISO_PIN) == GPIO_PIN_SET) {
            received |= (1 << bit);
        }
        
        // Clock low (data hold)
        HAL_GPIO_WritePin(SPI_CLK_PORT, SPI_CLK_PIN, GPIO_PIN_RESET);
        
        // Hold time
        __NOP(); __NOP(); __NOP(); __NOP();
    }
    
    // Deassert chip select
    HAL_GPIO_WritePin(SPI_CS_PORT, SPI_CS_PIN, GPIO_PIN_SET);
    
    return received;
}

/**
 * @brief  Optimized GPIO operations using direct register access
 * @param  data: 8-bit data to output on GPIOA pins 0-7
 * @retval None
 */
void fast_parallel_output(uint8_t data)
{
    // Method 1: HAL function (slower but safe)
    HAL_GPIO_WritePin(GPIOA, 0xFF, GPIO_PIN_RESET);  // Clear all pins
    HAL_GPIO_WritePin(GPIOA, data, GPIO_PIN_SET);    // Set required pins
    
    // Method 2: Direct register access (faster)
    GPIOA->ODR = (GPIOA->ODR & 0xFF00) | data;  // Update lower 8 bits only
    
    // Method 3: Atomic bit set/reset (fastest and safe)
    uint32_t set_bits = data;
    uint32_t reset_bits = (~data) & 0xFF;
    GPIOA->BSRR = (reset_bits << 16) | set_bits;  // Atomic operation
}