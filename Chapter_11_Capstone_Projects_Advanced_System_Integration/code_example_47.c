/*
 * Code Example 47
 * Language: C
 * Chapter: Chapter_11_Capstone_Projects_Advanced_System_Integration
 * Line: 12131
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

#define MAX_VOICES 3
#define SAMPLE_RATE 48000
#define AUDIO_BUFFER_SIZE 128
#define EFFECTS_BUFFER_SIZE 1024

// Voice structure with complete synthesis parameters
typedef struct {
    float frequency;
    float phase;
    float amplitude;
    waveform_t waveform;
    adsr_state_t envelope;
    bool active;
    uint8_t midi_note;
    uint8_t velocity;
} voice_t;

// Effects parameters
typedef struct {
    float reverb_mix;
    float reverb_time;
    float filter_cutoff;
    float filter_resonance;
    float distortion_drive;
} effects_params_t;

static voice_t voices[MAX_VOICES];
static float audio_buffer_a[AUDIO_BUFFER_SIZE];
static float audio_buffer_b[AUDIO_BUFFER_SIZE];
static volatile float* active_audio_buffer = audio_buffer_a;
static effects_params_t effects;

// Circular delay buffer for reverb
static float reverb_buffer[EFFECTS_BUFFER_SIZE];
static uint16_t reverb_index = 0;

/**
 * @brief Initialize high-performance audio synthesis system
 */
HAL_StatusTypeDef init_audio_synthesizer(void) {
    // Configure high-speed timer for 48kHz DAC triggering
    configure_audio_timer(SAMPLE_RATE);
    
    // Initialize voices
    for (int i = 0; i < MAX_VOICES; i++) {
        voices[i].active = false;
        voices[i].waveform = WAVEFORM_SINE;
        init_adsr_envelope(&voices[i].envelope);
    }
    
    // Start audio processing
    HAL_TIM_Base_Start_IT(&htim_audio);
    HAL_DAC_Start_DMA(&hdac1, (uint32_t*)active_audio_buffer, AUDIO_BUFFER_SIZE);
    
    return HAL_OK;
}

/**
 * @brief High-priority audio processing interrupt (48kHz)
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM_AUDIO) {
        static uint32_t buffer_index = 0;
        
        // Generate audio sample
        float sample = generate_audio_sample();
        
        // Apply effects chain
        sample = apply_effects_chain(sample);
        
        // Convert to DAC format and store
        uint16_t dac_value = (uint16_t)((sample + 1.0f) * 2047.5f);
        active_audio_buffer[buffer_index] = dac_value;
        
        buffer_index++;
        if (buffer_index >= AUDIO_BUFFER_SIZE) {
            buffer_index = 0;
            // Switch buffers
            active_audio_buffer = (active_audio_buffer == audio_buffer_a) ? 
                                  audio_buffer_b : audio_buffer_a;
        }
    }
}

/**
 * @brief Generate polyphonic audio sample
 */
float generate_audio_sample(void) {
    float mixed_sample = 0.0f;
    int active_voice_count = 0;
    
    for (int i = 0; i < MAX_VOICES; i++) {
        if (voices[i].active) {
            // Generate oscillator sample
            float voice_sample = generate_oscillator_sample(&voices[i]);
            
            // Apply ADSR envelope
            float envelope_value = process_adsr_envelope(&voices[i].envelope);
            voice_sample *= envelope_value;
            
            // Mix with other voices
            mixed_sample += voice_sample * voices[i].amplitude;
            active_voice_count++;
            
            // Update voice parameters
            update_voice_parameters(&voices[i]);
        }
    }
    
    // Normalize by active voice count to prevent clipping
    if (active_voice_count > 0) {
        mixed_sample /= active_voice_count;
    }
    
    return mixed_sample;
}

/**
 * @brief Apply real-time effects chain
 */
float apply_effects_chain(float input) {
    float output = input;
    
    // State-variable filter
    static float filter_state[2] = {0, 0};
    output = state_variable_filter(output, effects.filter_cutoff, 
                                  effects.filter_resonance, filter_state);
    
    // Soft-clipping distortion
    if (effects.distortion_drive > 0.1f) {
        output = soft_clip(output * effects.distortion_drive) / effects.distortion_drive;
    }
    
    // Digital reverb
    float delayed_sample = reverb_buffer[reverb_index];
    reverb_buffer[reverb_index] = output + delayed_sample * effects.reverb_time;
    reverb_index = (reverb_index + 1) % EFFECTS_BUFFER_SIZE;
    
    output = output * (1.0f - effects.reverb_mix) + 
             delayed_sample * effects.reverb_mix;
    
    return output;
}

/**
 * @brief MIDI note on handler with voice allocation
 */
void handle_midi_note_on(uint8_t note, uint8_t velocity) {
    // Find available voice or steal oldest
    int voice_index = find_available_voice();
    if (voice_index == -1) {
        voice_index = find_oldest_voice();
    }
    
    // Configure voice
    voices[voice_index].midi_note = note;
    voices[voice_index].velocity = velocity;
    voices[voice_index].frequency = midi_note_to_frequency(note);
    voices[voice_index].amplitude = velocity / 127.0f;
    voices[voice_index].active = true;
    
    // Trigger envelope
    trigger_adsr_envelope(&voices[voice_index].envelope);
}

/**
 * @brief Generate oscillator waveform sample
 */
float generate_oscillator_sample(voice_t* voice) {
    float sample = 0.0f;
    
    switch (voice->waveform) {
        case WAVEFORM_SINE:
            sample = sinf(voice->phase * 2.0f * M_PI);
            break;
        case WAVEFORM_SQUARE:
            sample = (voice->phase < 0.5f) ? 1.0f : -1.0f;
            break;
        case WAVEFORM_TRIANGLE:
            sample = (voice->phase < 0.5f) ? 
                     (4.0f * voice->phase - 1.0f) : 
                     (3.0f - 4.0f * voice->phase);
            break;
        case WAVEFORM_SAWTOOTH:
            sample = 2.0f * voice->phase - 1.0f;
            break;
    }
    
    // Update phase
    voice->phase += voice->frequency / SAMPLE_RATE;
    if (voice->phase >= 1.0f) {
        voice->phase -= 1.0f;
    }
    
    return sample;
}