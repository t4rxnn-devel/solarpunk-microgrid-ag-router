/**
 * @file hardware_control.h
 * @brief ISO C99 & Inline ARM Assembly Direct Hardware Control Header
 * @repository t4rxnn-devel/solarpunk-microgrid-ag-router
 * @details Declares public interfaces for direct memory-mapped register writes,
 *          low-level cycle-accurate PWM adjustments, bus voltage safety validation,
 *          hardware watchdogs, and interrupt mask control blocks for Teensy 4.1 nodes.
 */

#ifndef HARDWARE_CONTROL_H
#define HARDWARE_CONTROL_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Memory-Mapped Base Addresses for IMXRT1062 Peripherals
 */
#define IMXRT_GPIO1_BASE_ADDR   (0x401B0000UL)
#define IMXRT_GPIO2_BASE_ADDR   (0x401C0000UL)
#define IMXRT_GPIO3_BASE_ADDR   (0x401D0000UL)
#define IMXRT_GPIO4_BASE_ADDR   (0x401E0000UL)
#define IMXRT_PWM1_BASE_ADDR    (0x403E0000UL)
#define IMXRT_ADC1_BASE_ADDR    (0x40040000UL)

/**
 * @brief Critical Microgrid Safety Guardrail Macros
 */
#define MAX_ALLOWABLE_ADC_SAMPLE (3720U) // Corresponds to safe input voltage ceiling
#define MIN_ALLOWABLE_ADC_SAMPLE (410U)  // Corresponds to brownout lower threshold limit
#define PWM_DUTY_MAX_SCALE       (1000U) // 100.0% duty cycle resolution scaler

/**
 * @brief Execute a direct hardware register write via memory-mapped pointers.
 * @param register_address Target 32-bit physical register address.
 * @param value 32-bit data value to commit to the register.
 */
void write_hardware_register_direct(uint32_t register_address, uint32_t value);

/**
 * @brief Read a 32-bit value directly from a memory-mapped hardware register.
 * @param register_address Target 32-bit physical register address.
 * @return 32-bit unsigned register content.
 */
uint32_t read_hardware_register_direct(uint32_t register_address);

/**
 * @brief Set PWM duty cycle via low-level inline assembly instructions.
 * @param duty_scaled 16-bit scaled duty compare count (0 to PWM_DUTY_MAX_SCALE).
 */
void set_pwm_duty_cycle_assembly(uint16_t duty_scaled);

/**
 * @brief Validate raw ADC sample counts against safety limits using strict C99 logic.
 * @param raw_adc_sample 12-bit ADC register reading.
 * @return true if within safe thresholds, false if fault tripped.
 */
bool validate_bus_voltage_c99(uint16_t raw_adc_sample);

/**
 * @brief Execute a low-level ARM Data Synchronization Barrier (DSB).
 */
static inline void hardware_dsb_barrier(void) {
    __asm__ volatile ("dsb" : : : "memory");
}

/**
 * @brief Execute a low-level ARM Instruction Synchronization Barrier (ISB).
 */
static inline void hardware_isb_barrier(void) {
    __asm__ volatile ("isb" : : : "memory");
}

/**
 * @brief Trigger an immediate software-level hardware fault latch response.
 */
void trigger_hardware_fault_latch(void);

#ifdef __cplusplus
}
#endif

#endif // HARDWARE_CONTROL_H
