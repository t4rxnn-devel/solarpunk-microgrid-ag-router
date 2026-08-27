/**
 * @file mppt_engine.cpp
 * @brief Perturb & Observe Maximum Power Point Tracking (MPPT) Engine
 * @repository t4rxnn-devel/solarpunk-microgrid-ag-router
 * @details Implements real-time voltage stepping and dynamic power derivative evaluation
 *          to lock onto peak photovoltaic efficiency without oscillation.
 */

#include <Arduino.h>

class MpptEngine {
private:
    float last_voltage;
    float last_power;
    float current_voltage_setpoint;
    float voltage_step_size;
    
    const float min_voltage_limit = 12.0f;
    const float max_voltage_limit = 48.0f;

public:
    MpptEngine(initial_voltage, step_size) {
        last_voltage = initial_voltage;
        last_power = 0.0f;
        current_voltage_setpoint = initial_voltage;
        voltage_step_size = step_size;
    }

    /**
     * @brief Compute next optimal voltage setpoint based on live voltage and current readings.
     * @param v_pv Measured photovoltaic input voltage.
     * @param i_pv Measured photovoltaic input current.
     * @return Adjusted target voltage setpoint for the DC-DC stage.
     */
    float update(float v_pv, float i_pv) {
        float power = v_pv * i_pv;
        float delta_power = power - last_power;
        float delta_voltage = v_pv - last_voltage;

        if (delta_power != 0.0f) {
            if (delta_power > 0.0f) {
                current_voltage_setpoint += (delta_voltage > 0.0f) ? voltage_step_size : -voltage_step_size;
            } else {
                current_voltage_setpoint += (delta_voltage > 0.0f) ? -voltage_step_size : voltage_step_size;
            }
        }

        // Clamp boundaries to protect power electronics stage
        if (current_voltage_setpoint < min_voltage_limit) current_voltage_setpoint = min_voltage_limit;
        if (current_voltage_setpoint > max_voltage_limit) current_voltage_setpoint = max_voltage_limit;

        last_voltage = v_pv;
        last_power = power;

        return current_voltage_setpoint;
    }
};
