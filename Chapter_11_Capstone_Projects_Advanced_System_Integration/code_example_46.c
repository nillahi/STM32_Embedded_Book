/*
 * Code Example 46
 * Language: C
 * Chapter: Chapter_11_Capstone_Projects_Advanced_System_Integration
 * Line: 11895
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

// Multi-channel ADC configuration with DMA double-buffering
#define SENSOR_CHANNELS 3
#define BUFFER_SIZE 64
static uint16_t adc_buffer_a[SENSOR_CHANNELS * BUFFER_SIZE];
static uint16_t adc_buffer_b[SENSOR_CHANNELS * BUFFER_SIZE];
static volatile bool buffer_ready = false;
static volatile uint16_t* active_buffer = adc_buffer_a;

// Sensor data structure with statistics
typedef struct {
    float current_value;
    float moving_average;
    float min_value, max_value;
    float std_deviation;
    trend_t trend;
    alarm_state_t alarm_status;
    uint32_t last_update_time;
} sensor_data_t;

static sensor_data_t sensors[SENSOR_CHANNELS];

/**
 * @brief Initialize multi-sensor acquisition system
 */
HAL_StatusTypeDef init_environmental_monitor(void) {
    // Configure timer for 1kHz ADC triggering
    configure_adc_trigger_timer(1000); // 1kHz
    
    // Start ADC in scan mode with DMA
    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*)active_buffer, 
                         SENSOR_CHANNELS * BUFFER_SIZE) != HAL_OK) {
        return HAL_ERROR;
    }
    
    // Initialize processing timers
    HAL_TIM_Base_Start_IT(&htim_display);  // 10Hz display update
    HAL_TIM_Base_Start_IT(&htim_logging);  // 1/60Hz data logging
    HAL_TIM_Base_Start_IT(&htim_alarms);   // 5Hz alarm checking
    
    return HAL_OK;
}

/**
 * @brief DMA complete callback - process sensor data
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) {
        buffer_ready = true;
        
        // Switch to alternate buffer for continuous acquisition
        active_buffer = (active_buffer == adc_buffer_a) ? adc_buffer_b : adc_buffer_a;
        HAL_ADC_Start_DMA(&hadc1, (uint32_t*)active_buffer, 
                         SENSOR_CHANNELS * BUFFER_SIZE);
        
        // Signal main loop to process data
        set_processing_flag();
    }
}

/**
 * @brief Advanced sensor data processing with statistics
 */
void process_sensor_data(void) {
    if (!buffer_ready) return;
    
    uint16_t* processing_buffer = (active_buffer == adc_buffer_a) ? 
                                  adc_buffer_b : adc_buffer_a;
    
    for (int sensor = 0; sensor < SENSOR_CHANNELS; sensor++) {
        // Extract sensor-specific samples from interleaved buffer
        float samples[BUFFER_SIZE];
        for (int i = 0; i < BUFFER_SIZE; i++) {
            samples[i] = adc_to_physical_value(
                processing_buffer[i * SENSOR_CHANNELS + sensor], sensor);
        }
        
        // Update statistics
        update_sensor_statistics(&sensors[sensor], samples, BUFFER_SIZE);
        
        // Check for alarms
        evaluate_alarm_conditions(&sensors[sensor], sensor);
    }
    
    buffer_ready = false;
}

/**
 * @brief Update comprehensive sensor statistics
 */
void update_sensor_statistics(sensor_data_t* sensor, float* samples, int count) {
    // Current value (latest sample)
    sensor->current_value = samples[count - 1];
    
    // Moving average with exponential weighting
    static const float alpha = 0.1f; // Smoothing factor
    sensor->moving_average = alpha * sensor->current_value + 
                            (1.0f - alpha) * sensor->moving_average;
    
    // Min/Max tracking with time-based reset
    if (sensor->current_value < sensor->min_value || 
        HAL_GetTick() - sensor->last_update_time > STATS_RESET_PERIOD) {
        sensor->min_value = sensor->current_value;
    }
    if (sensor->current_value > sensor->max_value || 
        HAL_GetTick() - sensor->last_update_time > STATS_RESET_PERIOD) {
        sensor->max_value = sensor->current_value;
    }
    
    // Standard deviation calculation
    float variance = 0;
    for (int i = 0; i < count; i++) {
        float diff = samples[i] - sensor->moving_average;
        variance += diff * diff;
    }
    sensor->std_deviation = sqrtf(variance / count);
    
    // Trend analysis
    static float previous_average = 0;
    float change_rate = (sensor->moving_average - previous_average) / 
                       (HAL_GetTick() - sensor->last_update_time) * 1000.0f;
    
    if (fabsf(change_rate) < TREND_THRESHOLD) {
        sensor->trend = TREND_STABLE;
    } else if (change_rate > 0) {
        sensor->trend = TREND_RISING;
    } else {
        sensor->trend = TREND_FALLING;
    }
    
    previous_average = sensor->moving_average;
    sensor->last_update_time = HAL_GetTick();
}