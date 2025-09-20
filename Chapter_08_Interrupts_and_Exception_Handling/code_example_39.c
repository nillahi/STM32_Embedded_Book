/*
 * Code Example 39
 * Language: C
 * Chapter: Chapter_08_Interrupts_and_Exception_Handling
 * Line: 10419
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
#include <stdio.h>

// Fault information structure
typedef struct {
    uint32_t fault_type;
    uint32_t fault_address;
    uint32_t stack_pointer;
    uint32_t program_counter;
    uint32_t link_register;
    uint32_t fault_status;
    uint32_t timestamp;
} fault_info_t;

static fault_info_t last_fault_info = {0};

/**
 * @brief  Hard Fault handler with comprehensive debugging
 * @retval None
 */
void HardFault_Handler(void)
{
    // Disable interrupts to prevent further issues
    __disable_irq();
    
    // Capture fault information
    last_fault_info.fault_type = 1;  // Hard fault
    last_fault_info.timestamp = HAL_GetTick();
    
    // Get stack pointer (depends on which stack was in use)
    if (__get_CONTROL() & 2) {
        last_fault_info.stack_pointer = __get_PSP();
    } else {
        last_fault_info.stack_pointer = __get_MSP();
    }
    
    // Extract fault address and PC from stack frame
    uint32_t *stack_frame = (uint32_t*)last_fault_info.stack_pointer;
    last_fault_info.program_counter = stack_frame[6];
    last_fault_info.link_register = stack_frame[5];
    
    // Read fault status registers
    last_fault_info.fault_status = SCB->HFSR;
    
    // Log fault information
    log_fault_information(&last_fault_info);
    
    // Attempt recovery or safe shutdown
    handle_hard_fault_recovery();
    
    // If recovery fails, reset system
    NVIC_SystemReset();
}

/**
 * @brief  Memory Management Fault handler
 * @retval None
 */
void MemManage_Handler(void)
{
    // Capture specific memory management fault info
    last_fault_info.fault_type = 2;
    last_fault_info.fault_status = SCB->MMFSR;
    last_fault_info.fault_address = SCB->MMFAR;
    
    printf("Memory Management Fault at address: 0x%08lX\n", 
           last_fault_info.fault_address);
    
    // Clear fault flags
    SCB->MMFSR |= 0xFF;
    
    // Attempt to continue (if safe)
    // Otherwise escalate to Hard Fault
}

/**
 * @brief  Bus Fault handler
 * @retval None
 */
void BusFault_Handler(void)
{
    last_fault_info.fault_type = 3;
    last_fault_info.fault_status = SCB->BFSR;
    
    if (SCB->BFSR & SCB_BFSR_BFARVALID_Msk) {
        last_fault_info.fault_address = SCB->BFAR;
        printf("Bus Fault at address: 0x%08lX\n", last_fault_info.fault_address);
    } else {
        printf("Bus Fault (address not valid)\n");
    }
    
    // Clear fault flags
    SCB->BFSR |= 0xFF;
    
    // Implement recovery strategy
}

/**
 * @brief  Usage Fault handler
 * @retval None
 */
void UsageFault_Handler(void)
{
    last_fault_info.fault_type = 4;
    last_fault_info.fault_status = SCB->UFSR;
    
    printf("Usage Fault: ");
    
    if (SCB->UFSR & SCB_UFSR_UNDEFINSTR_Msk) {
        printf("Undefined instruction\n");
    }
    if (SCB->UFSR & SCB_UFSR_INVSTATE_Msk) {
        printf("Invalid state\n");
    }
    if (SCB->UFSR & SCB_UFSR_INVPC_Msk) {
        printf("Invalid PC\n");
    }
    if (SCB->UFSR & SCB_UFSR_NOCP_Msk) {
        printf("No coprocessor\n");
    }
    
    // Clear fault flags
    SCB->UFSR |= 0xFFFF;
    
    // Attempt recovery
}

/**
 * @brief  Log fault information for debugging
 * @param  fault_info: Pointer to fault information structure
 * @retval None
 */
void log_fault_information(fault_info_t *fault_info)
{
    printf("\n=== FAULT INFORMATION ===\n");
    printf("Fault Type: %lu\n", fault_info->fault_type);
    printf("Timestamp: %lu ms\n", fault_info->timestamp);
    printf("Program Counter: 0x%08lX\n", fault_info->program_counter);
    printf("Link Register: 0x%08lX\n", fault_info->link_register);
    printf("Stack Pointer: 0x%08lX\n", fault_info->stack_pointer);
    printf("Fault Status: 0x%08lX\n", fault_info->fault_status);
    
    // Store in non-volatile memory for post-mortem analysis
    // write_fault_to_flash(fault_info);
}

/**
 * @brief  Attempt system recovery from hard fault
 * @retval None
 */
void handle_hard_fault_recovery(void)
{
    // Disable all peripherals
    __HAL_RCC_AHB1_FORCE_RESET();
    __HAL_RCC_AHB2_FORCE_RESET();
    __HAL_RCC_APB1_FORCE_RESET();
    __HAL_RCC_APB2_FORCE_RESET();
    
    HAL_Delay(10);
    
    __HAL_RCC_AHB1_RELEASE_RESET();
    __HAL_RCC_AHB2_RELEASE_RESET();
    __HAL_RCC_APB1_RELEASE_RESET();
    __HAL_RCC_APB2_RELEASE_RESET();
    
    // Reinitialize critical systems only
    SystemClock_Config();
    MX_GPIO_Init();
    
    printf("Attempted fault recovery\n");
}