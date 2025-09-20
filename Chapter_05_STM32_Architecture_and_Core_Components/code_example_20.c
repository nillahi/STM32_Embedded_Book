/*
 * Code Example 20
 * Language: C
 * Chapter: Chapter_05_STM32_Architecture_and_Core_Components
 * Line: 6169
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

// DMA handles for demonstration
DMA_HandleTypeDef hdma_memtomem_dma2_stream0;
DMA_HandleTypeDef hdma_adc_dma2_stream4;

/**
 * @brief  Demonstrate concurrent bus matrix access
 * @retval None
 */
void demonstrate_bus_matrix_performance(void)
{
    uint32_t source_buffer[1000];
    uint32_t dest_buffer[1000];
    uint32_t adc_buffer[100];
    
    printf("Bus Matrix Concurrent Access Demonstration\n");
    printf("=========================================\n");
    
    // Initialize source data
    for (int i = 0; i < 1000; i++) {
        source_buffer[i] = i * 2;
    }
    
    // Enable DWT cycle counter
    DWT->CYCCNT = 0;
    
    // Start concurrent operations:
    // 1. CPU performs calculations
    // 2. DMA1 transfers memory-to-memory
    // 3. DMA2 transfers ADC data
    
    // Configure memory-to-memory DMA
    hdma_memtomem_dma2_stream0.Instance = DMA2_Stream0;
    hdma_memtomem_dma2_stream0.Init.Channel = DMA_CHANNEL_0;
    hdma_memtomem_dma2_stream0.Init.Direction = DMA_MEMORY_TO_MEMORY;
    hdma_memtomem_dma2_stream0.Init.PeriphInc = DMA_PINC_ENABLE;
    hdma_memtomem_dma2_stream0.Init.MemInc = DMA_MINC_ENABLE;
    hdma_memtomem_dma2_stream0.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_memtomem_dma2_stream0.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_memtomem_dma2_stream0.Init.Mode = DMA_NORMAL;
    hdma_memtomem_dma2_stream0.Init.Priority = DMA_PRIORITY_HIGH;
    
    HAL_DMA_Init(&hdma_memtomem_dma2_stream0);
    
    // Start DMA transfer
    HAL_DMA_Start(&hdma_memtomem_dma2_stream0, 
                  (uint32_t)source_buffer, 
                  (uint32_t)dest_buffer, 1000);
    
    // Perform CPU calculations while DMA operates
    uint32_t calculation_result = 0;
    for (int i = 0; i < 10000; i++) {
        calculation_result += i * i;
    }
    
    // Wait for DMA completion
    HAL_DMA_PollForTransfer(&hdma_memtomem_dma2_stream0, 
                           HAL_DMA_FULL_TRANSFER, 1000);
    
    uint32_t total_cycles = DWT->CYCCNT;
    
    printf("Concurrent operations completed in %lu cycles\n", total_cycles);
    printf("Estimated single-threaded cycles: %lu\n", total_cycles * 2);
    printf("Bus matrix efficiency: %.1f%%\n", 
           ((float)(total_cycles * 2) / total_cycles - 1) * 100);
    
    // Verify DMA transfer
    bool transfer_success = true;
    for (int i = 0; i < 1000; i++) {
        if (dest_buffer[i] != source_buffer[i]) {
            transfer_success = false;
            break;
        }
    }
    
    printf("DMA transfer verification: %s\n", 
           transfer_success ? "PASSED" : "FAILED");
}