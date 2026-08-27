/**
 * @file hardware_control.h
 * @brief ISO C99 & Inline ARM Assembly Direct Hardware Control Header
 * @repository t4rxnn-devel/solarpunk-microgrid-ag-router
 * @details Declares public interfaces for direct memory-mapped register writes,
 *          low-level cycle-accurate PWM adjustments, and bus voltage validation routines.
 */

#ifndef HARDWARE_CONTROL_H
#define HARDWARE_CONTROL_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Execute a direct hardware register write via memory-mapped pointers.
 * @param register_address Target 32-bit register address.
 * @param value Value to write.
 */
void write_hardware_register_direct(uint32_t register_address, uint32_t value);

/**
 * @brief Set PWM duty cycle via low-level inline assembly instructions.
 * @param duty_scaled 16-bit scaled duty compare count.
 */
void set_pwm_duty_cycle_assembly(uint16_t duty_scaled);

/**
 * @brief Validate raw ADC sample counts against safety limits using C99 logic.
 * @param raw_adc_sample 12-bit ADC register reading.
 * @return true if within safe thresholds, false if fault tripped.
 */
bool validate_bus_voltage_c99(uint16_t raw_adc_sample);

#ifdef __cplusplus
}
#endif

#endif // HARDWARE_CONTROL_H
