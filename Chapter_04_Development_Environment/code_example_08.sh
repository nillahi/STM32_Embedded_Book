/*
 * Code Example 8
 * Language: bash
 * Chapter: Chapter_04_texorpdfstringparboxtextwidth
 * Line: 4569
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

# Install ARM GCC toolchain (Ubuntu/Debian)
sudo apt update
sudo apt install gcc-arm-none-eabi

# Install OpenOCD for programming and debugging
sudo apt install openocd

# Install build tools
sudo apt install make cmake

# Verify installation
arm-none-eabi-gcc --version
openocd --version

# Example Makefile structure
PROJECT = stm32_project
SOURCES = main.c system.c peripherals.c
OBJECTS = $(SOURCES:.c=.o)

MCU = cortex-m4
CFLAGS = -mcpu=$(MCU) -mthumb -std=c99 -Wall -O2

$(PROJECT).elf: $(OBJECTS)
	arm-none-eabi-gcc $(CFLAGS) -o $@ $^ -T linker_script.ld

%.o: %.c
	arm-none-eabi-gcc $(CFLAGS) -c -o $@ $<

flash: $(PROJECT).elf
	openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
	        -c "program $(PROJECT).elf verify reset exit"

clean:
	rm -f $(OBJECTS) $(PROJECT).elf