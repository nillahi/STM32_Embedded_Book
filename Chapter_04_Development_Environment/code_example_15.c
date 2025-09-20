/*
 * Code Example 15
 * Language: C
 * Chapter: Chapter_04_texorpdfstringparboxtextwidth
 * Line: 5018
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

// Constants: ALL_CAPS with underscores
#define MAX_BUFFER_SIZE     256
#define UART_TIMEOUT_MS     1000

// Variables: lowercase with underscores
uint32_t system_tick_counter;
float sensor_temperature_celsius;

// Functions: descriptive names with verb_noun pattern
void initialize_system_peripherals(void);
uint8_t read_temperature_sensor(float *temperature);
bool validate_configuration_parameters(void);

// Types: PascalCase with _t suffix
typedef struct {
    uint32_t frequency_hz;
    uint8_t duty_cycle_percent;
    bool enabled;
} PWM_Config_t;