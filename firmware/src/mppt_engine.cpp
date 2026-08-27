/**
 * @file mppt_engine.cpp
 * @brief Perturb & Observe Maximum Power Point Tracking (MPPT) Engine
 * @repository t4rxnn-devel/solarpunk-microgrid-ag-router
 */

#include "mppt_engine.h"

MpptEngine::MpptEngine(float initial_voltage, float step_size) {
    last_voltage = initial_voltage;
    last_power = 0.0f;
    current_voltage_setpoint = initial_voltage;
    voltage_step_size = step_size;
}

float MpptEngine::update(float v_pv, float i_pv) {
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

    if (current_voltage_setpoint < min_voltage_limit) current_voltage_setpoint = min_voltage_limit;
    if (current_voltage_setpoint > max_voltage_limit) current_voltage_setpoint = max_voltage_limit;

    last_voltage = v_pv;
    last_power = power;

    return current_voltage_setpoint;
}
