/*
 * Code Example 48
 * Language: C
 * Chapter: Chapter_11_Capstone_Projects_Advanced_System_Integration
 * Line: 12424
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

#define CONTROL_FREQUENCY 10000  // 10kHz control loop
#define PWM_FREQUENCY 20000      // 20kHz PWM switching
#define ENCODER_PPR 4096         // Pulses per revolution

// Control system state
typedef struct {
    // Setpoints
    float position_setpoint;
    float velocity_setpoint;
    float current_setpoint;
    
    // Feedback
    int32_t encoder_count;
    float position;
    float velocity;
    float current;
    
    // Control outputs
    float position_output;
    float velocity_output;
    float pwm_duty[3];  // 3-phase PWM
    
    // Control parameters
    pid_controller_t position_pid;
    pid_controller_t velocity_pid;
    pid_controller_t current_pid;
    
    // Safety limits
    float max_velocity;
    float max_current;
    float position_limit_min;
    float position_limit_max;
    
    // System status
    motor_state_t state;
    fault_flags_t faults;
} motor_control_t;

static motor_control_t motor;
static volatile bool control_update_flag = false;

/**
 * @brief Initialize comprehensive motor control system
 */
HAL_StatusTypeDef init_motor_control_system(void) {
    // Initialize encoder interface (Timer in encoder mode)
    configure_encoder_interface(ENCODER_PPR);
    
    // Initialize 3-phase PWM generation
    configure_3phase_pwm(PWM_FREQUENCY);
    
    // Initialize control loop timer
    configure_control_timer(CONTROL_FREQUENCY);
    
    // Initialize PID controllers
    init_pid_controller(&motor.position_pid, 10.0f, 0.1f, 0.05f);
    init_pid_controller(&motor.velocity_pid, 0.5f, 0.05f, 0.01f);
    init_pid_controller(&motor.current_pid, 2.0f, 20.0f, 0.0f);
    
    // Set initial safety limits
    motor.max_velocity = 1000.0f;  // RPM
    motor.max_current = 5.0f;      // Amperes
    motor.position_limit_min = -180.0f;  // Degrees
    motor.position_limit_max = 180.0f;
    
    // Start control system
    HAL_TIM_Base_Start_IT(&htim_control);
    motor.state = MOTOR_STATE_READY;
    
    return HAL_OK;
}

/**
 * @brief High-frequency control loop interrupt (10kHz)
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM_CONTROL) {
        // Read encoder position
        int32_t encoder_raw = __HAL_TIM_GET_COUNTER(&htim_encoder);
        motor.encoder_count = encoder_raw;
        motor.position = (float)encoder_raw * 360.0f / (4 * ENCODER_PPR);
        
        // Estimate velocity using high-resolution differentiation
        static int32_t last_encoder_count = 0;
        static float velocity_filter_state = 0.0f;
        
        float velocity_raw = (float)(encoder_raw - last_encoder_count) * 
                            CONTROL_FREQUENCY * 60.0f / (4 * ENCODER_PPR);
        
        // Apply low-pass filter to velocity estimate
        const float velocity_filter_alpha = 0.1f;
        velocity_filter_state = velocity_filter_alpha * velocity_raw + 
                               (1.0f - velocity_filter_alpha) * velocity_filter_state;
        motor.velocity = velocity_filter_state;
        last_encoder_count = encoder_raw;
        
        // Read current feedback (ADC via DMA)
        motor.current = read_motor_current();
        
        // Safety checks
        if (check_safety_limits(&motor)) {
            // Execute control loops
            execute_cascaded_control(&motor);
            
            // Update PWM outputs
            update_3phase_pwm(motor.pwm_duty);
        } else {
            // Safety violation - disable outputs
            disable_motor_outputs();
            motor.state = MOTOR_STATE_FAULT;
        }
        
        control_update_flag = true;
    }
}

/**
 * @brief Execute cascaded position/velocity/current control
 */
void execute_cascaded_control(motor_control_t* ctrl) {
    // Position loop (outer loop)
    if (ctrl->state == MOTOR_STATE_POSITION_CONTROL) {
        float position_error = ctrl->position_setpoint - ctrl->position;
        ctrl->velocity_setpoint = pid_compute(&ctrl->position_pid, position_error);
        
        // Velocity feedforward
        ctrl->velocity_setpoint += get_velocity_feedforward(ctrl->position_setpoint);
    }
    
    // Velocity loop (middle loop)
    float velocity_error = ctrl->velocity_setpoint - ctrl->velocity;
    ctrl->current_setpoint = pid_compute(&ctrl->velocity_pid, velocity_error);
    
    // Current loop (inner loop - highest bandwidth)
    float current_error = ctrl->current_setpoint - ctrl->current;
    float voltage_command = pid_compute(&ctrl->current_pid, current_error);
    
    // Convert voltage command to PWM duties using space vector modulation
    space_vector_modulation(voltage_command, ctrl->position, ctrl->pwm_duty);
}

/**
 * @brief Advanced PID controller with anti-windup
 */
float pid_compute(pid_controller_t* pid, float error) {
    static uint32_t last_time = 0;
    uint32_t current_time = HAL_GetTick();
    float dt = (float)(current_time - last_time) / 1000.0f;
    
    if (dt <= 0.0f) dt = 1.0f / CONTROL_FREQUENCY;
    
    // Proportional term
    float output = pid->kp * error;
    
    // Integral term with anti-windup
    pid->integral += error * dt;
    
    // Clamp integral term
    if (pid->integral > pid->integral_max) {
        pid->integral = pid->integral_max;
    } else if (pid->integral < pid->integral_min) {
        pid->integral = pid->integral_min;
    }
    
    output += pid->ki * pid->integral;
    
    // Derivative term with filtering
    float derivative = (error - pid->last_error) / dt;
    pid->derivative_filtered = 0.8f * pid->derivative_filtered + 0.2f * derivative;
    output += pid->kd * pid->derivative_filtered;
    
    pid->last_error = error;
    last_time = current_time;
    
    return output;
}

/**
 * @brief Space Vector Modulation for 3-phase PWM generation
 */
void space_vector_modulation(float voltage_magnitude, float angle, float* duties) {
    // Convert to 3-phase voltages
    float va = voltage_magnitude * cosf(angle);
    float vb = voltage_magnitude * cosf(angle - 2.0f * M_PI / 3.0f);
    float vc = voltage_magnitude * cosf(angle + 2.0f * M_PI / 3.0f);
    
    // Find min and max for centering
    float vmin = fminf(fminf(va, vb), vc);
    float vmax = fmaxf(fmaxf(va, vb), vc);
    float vcom = (vmin + vmax) / 2.0f;
    
    // Generate centered PWM duties
    duties[0] = (va - vcom + 1.0f) / 2.0f;  // Phase A
    duties[1] = (vb - vcom + 1.0f) / 2.0f;  // Phase B
    duties[2] = (vc - vcom + 1.0f) / 2.0f;  // Phase C
    
    // Clamp to valid range
    for (int i = 0; i < 3; i++) {
        if (duties[i] < 0.0f) duties[i] = 0.0f;
        if (duties[i] > 1.0f) duties[i] = 1.0f;
    }
}

/**
 * @brief Comprehensive safety monitoring
 */
bool check_safety_limits(motor_control_t* ctrl) {
    bool safe = true;
    
    // Current limit check
    if (fabsf(ctrl->current) > ctrl->max_current) {
        ctrl->faults.overcurrent = true;
        safe = false;
    }
    
    // Velocity limit check
    if (fabsf(ctrl->velocity) > ctrl->max_velocity) {
        ctrl->faults.overspeed = true;
        safe = false;
    }
    
    // Position limit check
    if (ctrl->position < ctrl->position_limit_min || 
        ctrl->position > ctrl->position_limit_max) {
        ctrl->faults.position_limit = true;
        safe = false;
    }
    
    // Temperature monitoring (if available)
    float temperature = read_motor_temperature();
    if (temperature > MAX_MOTOR_TEMPERATURE) {
        ctrl->faults.overtemperature = true;
        safe = false;
    }
    
    return safe;
}