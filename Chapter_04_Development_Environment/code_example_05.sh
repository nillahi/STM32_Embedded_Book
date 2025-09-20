/*
 * Code Example 5
 * Language: bash
 * Chapter: Chapter_04_texorpdfstringparboxtextwidth
 * Line: 4273
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

arm-none-eabi-gcc -o "HelloWorld_STM32.elf" @"objects.list" 
  -mcpu=cortex-m4 -T"STM32F401RETX_FLASH.ld" 
  --specs=nosys.specs -Wl,-Map="HelloWorld_STM32.map" 
  -Wl,--gc-sections -static --specs=nano.specs 
  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb 
  -Wl,--start-group -lc -lm -Wl,--end-group

Memory Usage Analysis:
   text    data     bss     dec     hex filename
   1980      20    1572    3572     df4 HelloWorld_STM32.elf

Flash Usage: 2000 bytes (0.4% of 512KB)
SRAM Usage:  1592 bytes (1.7% of 96KB)

Build completed successfully in 2.3 seconds.