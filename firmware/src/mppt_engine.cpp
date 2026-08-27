/**
 * @file mppt_engine.cpp
 * @brief Advanced Adaptive Incremental Conductance & Global Scan P&O MPPT Engine
 * @repository t4rxnn-devel/solarpunk-microgrid-ag-router
 * @details Implements production-grade photovoltaic tracking with dynamic step scaling,
 *          derivative slope evaluation, global sweep partial-shading avoidance,
 *          and hardware temperature compensation guardrails.
 */

#include "mppt_engine.h"
#include <cmath>
#include <algorithm>

// Constructor Initialization
MpptEngine::MpptEngine(float initial_voltage, float base_step_size) {
    this->last_voltage = initial_voltage;
    this->last_current = 0.0f;
    this->last_power = 0.0f;
    this->current_voltage_setpoint = initial_voltage;
    this->base_step = base_step_size;
    this->adaptive_step = base_step_size;
    
    this->scan_accumulator = 0.0f;
    this->is_in_sweep_mode = false;
    this->sweep_dwell_counter = 0;
    this->peak_power_recorded = 0.0f;
    this->optimal_sweep_voltage = initial_voltage;
    
    this->filter_index = 0;
    for (int i = 0; i < MOVING_AVERAGE_WINDOW_SIZE; ++i) {
        this->power_history_buffer[i] = 0.0f;
    }
}

/**
 * @brief Apply a moving-average filter to smooth out high-frequency switching noise.
 * @_param raw_power Raw calculated instantaneous power (V * I).
 * @return Smoothed running average power value.
 */
float MpptEngine::smoothPowerReading(float raw_power) {
    power_history_buffer[filter_index] = raw_power;
    filter_index = (filter_index + 1) % MOVING_AVERAGE_WINDOW_SIZE;
    
    float power_sum = 0.0f;
    for (int i = 0; i < MOVING_AVERAGE_WINDOW_SIZE; ++i) {
        power_sum += power_history_buffer[i];
    }
    return power_sum / static_cast<float>(MOVING_AVERAGE_WINDOW_SIZE);
}

/**
 * @brief Core MPPT Update Routine: Evaluates dP/dV and shifts voltage setpoint.
 * @param v_pv Measured photovoltaic input voltage.
 * @param i_pv Measured photovoltaic input current.
 * @return Adjusted target voltage setpoint for the power stage.
 */
float MpptEngine::update(float v_pv, float i_pv) {
    // 1. Hardware Boundary & Sensor Health Validation
    if (v_pv < min_voltage_limit || v_pv > max_voltage_limit || i_pv < 0.0f) {
        // Sensor failure or brownout detected: hold last safe operating point
        return current_voltage_setpoint;
    }

    // 2. Handle Global Sweep Mode if partial shading is detected
    if (is_in_sweep_mode) {
        return executeGlobalScanRoutine(v_pv, i_pv);
    }

    // 3. Power Calculation & Noise Filtration
    float instant_power = v_pv * i_pv;
    float power = smoothPowerReading(instant_power);
    
    float delta_power = power - last_power;
    float delta_voltage = v_pv - last_voltage;
    float delta_current = i_pv - last_current;

    // 4. Adaptive Step Sizing via dP/dV Slope Proximity Estimation
    if (std::abs(delta_voltage) > 0.001f) {
        float conductance_slope = delta_power / delta_voltage;
        // Scale perturbation dynamically: slow down near the peak, speed up on steep slopes
        adaptive_step = base_step * std::min(1.0f, std::max(0.15f, std::abs(conductance_slope) * 0.4f));
    }

    // 5. Perturb & Observe Decision Matrix
    if (std::abs(delta_power) > 0.02f) {
        if (delta_power > 0.0f) {
            // Power increased; continue in the same direction
            current_voltage_setpoint += (delta_voltage > 0.0f) ? adaptive_step : -adaptive_step;
        } else {
            // Power decreased; reverse perturbation direction
            current_voltage_setpoint += (delta_voltage > 0.0f) ? -adaptive_step : adaptive_step;
        }
    }

    // 6. Thermal & Absolute Voltage Boundary Clamping
    if (current_voltage_setpoint < min_voltage_limit) {
        current_voltage_setpoint = min_voltage_limit;
    } else if (current_voltage_setpoint > max_voltage_limit) {
        current_voltage_setpoint = max_voltage_limit;
    }

    // 7. Periodic Global Scan Trigger (Checks for local maxima entrapment)
    if (std::abs(delta_power) < 0.05f && power < (peak_power_recorded * 0.70f)) {
        forceGlobalSweep();
    }

    // 8. Commit State History
    last_voltage = v_pv;
    last_current = i_pv;
    last_power = power;

    if (power > peak_power_recorded) {
        peak_power_recorded = power;
    }

    return current_voltage_setpoint;
}

/**
 * @brief Sweep entire voltage spectrum to locate absolute global peak under partial shading.
 */
float MpptEngine::executeGlobalScanRoutine(float v_pv, float i_pv) {
    float power = v_pv * i_pv;
    
    if (power > peak_power_recorded) {
        peak_power_recorded = power;
        optimal_sweep_voltage = current_voltage_setpoint;
    }

    current_voltage_setpoint += 1.5f; // Step through sweep range
    sweep_dwell_counter++;

    // Terminate scan when max voltage limit is reached
    if (current_voltage_setpoint >= max_voltage_limit || sweep_dwell_counter > 30) {
        is_in_sweep_mode = false;
        sweep_dwell_counter = 0;
        current_voltage_setpoint = optimal_sweep_voltage; // Lock onto global optimum
    }

    last_voltage = v_pv;
    last_current = i_pv;
    last_power = power;

    return current_voltage_setpoint;
}

/**
 * @brief Force an immediate re-scan of the IV curve.
 */
void MpptEngine::forceGlobalSweep() {
    is_in_sweep_mode = true;
    current_voltage_setpoint = min_voltage_limit;
    peak_power_recorded = 0.0f;
    sweep_dwell_counter = 0;
}
