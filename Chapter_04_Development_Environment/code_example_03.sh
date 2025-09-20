/*
 * Code Example 3
 * Language: bash
 * Chapter: Chapter_04_texorpdfstringparboxtextwidth
 * Line: 3772
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

# Make installer executable
chmod +x st-stm32cubeide_1.x.x_xxxx_64.sh

# Run installer
./st-stm32cubeide_1.x.x_xxxx_64.sh

# Configure udev rules for ST-LINK (non-root access)
sudo nano /etc/udev/rules.d/49-stlinkv2.rules

# Add the following content:
SUBSYSTEM=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="3748", MODE="0666"
SUBSYSTEM=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374b", MODE="0666"
SUBSYSTEM=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374a", MODE="0666"

# Reload udev rules
sudo udevadm control --reload-rules
sudo udevadm trigger

# Add user to dialout group (for serial communication)
sudo usermod -a -G dialout $USER