/**
 * @file power_router.cpp
 * @brief Solid-State MOSFET High-Side Switching & Bus Protection Implementation
 * @repository t4rxnn-devel/solarpunk-microgrid-ag-router
 * @details Implements the PowerRouterChannel member functions for dynamic overcurrent
 *          monitoring, transient spike filtering, and gate control state validation.
 */

#include "power_router.h"

// Constructor implementation initializing control pins, trip thresholds, and safety flags
PowerRouterChannel::PowerRouterChannel(uint8_t pin_gate, uint8_t pin_sense, float trip_amps) {
    control_pin = pin_gate;
    current_sense_pin = pin_sense;
    current_trip_threshold_amps = trip_amps;
    
    debounce_current_sum = 0.0f;
    sample_count = 0;
    
    channel_active = false;
    overcurrent_fault = false;
    fault_timestamp = 0;
}

// Configure pin directions and set safe initial low state
void PowerRouterChannel::initialize() {
    pinMode(control_pin, OUTPUT);
    digitalWrite(control_pin, LOW);
    
    channel_active = false;
    overcurrent_fault = false;
}

// Evaluate live current metrics, apply moving average filter, and trip on threshold breach
void PowerRouterChannel::update(float measured_current_a) {
    if (overcurrent_fault) {
        // If already tripped, ensure control gate remains locked low
        digitalWrite(control_pin, LOW);
        channel_active = false;
        return;
    }

    // Accumulate rolling samples for transient current filtering
    debounce_current_sum += measured_current_a;
    sample_count++;

    if (sample_count >= 10) {
        float averaged_current = debounce_current_sum / 10.0f;
        debounce_current_sum = 0.0f;
        sample_count = 0;

        // Check if continuous current exceeds safe operational limits
        if (averaged_current > current_trip_threshold_amps) {
            digitalWrite(control_pin, LOW);
            channel_active = false;
            overcurrent_fault = true;
            fault_timestamp = millis();
        }
    }
}

// Manually command the MOSFET gate state if no faults are latched
void PowerRouterChannel::setChannelState(bool state) {
    if (overcurrent_fault) {
        return; // Reject state changes while fault protection is active
    }

    channel_active = state;
    digitalWrite(control_pin, state ? HIGH : LOW);
}

// Populate channel telemetry struct with real-time operational status
void PowerRouterChannel::getTelemetry(ChannelTelemetry_t &telemetry_out) const {
    telemetry_out.current_amps = debounce_current_sum / (sample_count > 0 ? sample_count : 1);
    telemetry_out.voltage_drop_mv = telemetry_out.current_amps * shunt_resistance_ohms * 1000.0f;
    telemetry_out.is_active = channel_active;
    telemetry_out.fault_tripped = overcurrent_fault;
    telemetry_out.trip_timestamp_ms = fault_timestamp;
}

// Clear overcurrent fault registers after a cooling or inspection period
void PowerRouterChannel::clearFault() {
    if (millis() - fault_timestamp > trip_holdoff_ms) {
        overcurrent_fault = false;
        debounce_current_sum = 0.0f;
        sample_count = 0;
    }
}
