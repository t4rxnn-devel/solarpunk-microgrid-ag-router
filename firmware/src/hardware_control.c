/**
 * @file hardware_control.c
 * @brief ISO C99 & Inline ARM Assembly Direct Hardware Control Register Module
 * @repository t4rxnn-devel/solarpunk-microgrid-ag-router
 * @details Implements low-level register manipulation, memory-mapped I/O safety barriers,
 *          and cycle-accurate PWM duty cycle modulation via inline assembly.
 */

#include <stdint.h>
#include <stdbool.h>

// Define hardware register base addresses for direct memory-mapped access
#define GPIO_BASE_REG       0x401b0000UL
#define PWM_TIMER_OFFSET    0x00004000UL

/**
 * @brief Force memory barrier and synchronization pipeline flush using inline assembly.
 */
static inline void inline_memory_barrier(void) {
    __asm__ volatile("dsb" ::: "memory");
    __asm__ volatile("isb" ::: "memory");
}

/**
 * @brief Execute a cycle-accurate low-level hardware register write using direct pointer mapping.
 * @param register_address Target memory-mapped hardware register address.
 * @param value 32-bit configuration value to write.
 */
void write_hardware_register_direct(uint32_t register_address, uint32_t value) {
    inline_memory_barrier();
    volatile uint32_t *target_reg = (volatile uint32_t *)register_address;
    *target_reg = value;
    inline_memory_barrier();
}

/**
 * @brief High-frequency PWM duty cycle setter using inline assembly bit manipulation.
 * @param duty_scaled Scaled 16-bit PWM compare register value.
 */
void set_pwm_duty_cycle_assembly(uint16_t duty_scaled) {
    register uint32_t base_addr = GPIO_BASE_REG + PWM_TIMER_OFFSET;
    register uint32_t duty_val = (uint32_t)duty_scaled;

    // Inline assembly execution block for fast hardware latching
    __asm__ volatile (
        "str %[duty], [%[base], #0x10]\n\t"
        : 
        : [base] "r" (base_addr), [duty] "r" (duty_val)
        : "memory"
    );
}

/**
 * @brief ISO C99 compliant validation of bus voltage limits against hardware registers.
 * @param raw_adc_sample Unsigned 12-bit ADC register reading.
 * @return true if voltage is within safe limits, false if overvoltage tripped.
 */
bool validate_bus_voltage_c99(uint16_t raw_adc_sample) {
    const uint16_t upper_limit_counts = 3720; // Corresponds to ~29.5V threshold
    const uint16_t lower_limit_counts = 1410; // Corresponds to ~11.2V threshold

    if (raw_adc_sample > upper_limit_counts || raw_adc_sample < lower_limit_counts) {
        return false;
    }
    return true;
}
