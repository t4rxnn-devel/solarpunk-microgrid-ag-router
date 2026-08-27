/**
 * @file mppt_engine.h
 * @brief Perturb & Observe Maximum Power Point Tracking (MPPT) Header
 * @repository t4rxnn-devel/solarpunk-microgrid-ag-router
 * @details Declares the MpptEngine class interface for real-time photovoltaic 
 *          voltage tracking and dynamic power optimization.
 */

#ifndef MPPT_ENGINE_H
#define MPPT_ENGINE_H

#include <Arduino.h>

/**
 * @class MpptEngine
 * @brief Manages perturb-and-observe MPPT algorithms for high-efficiency solar charging.
 */
class MpptEngine {
private:
    float last_voltage;
    float last_power;
    float current_voltage_setpoint;
    float voltage_step_size;
    
    const float min_voltage_limit = 12.0f;
    const float max_voltage_limit = 48.0f;

public:
    /**
     * @brief Construct a new Mppt Engine instance.
     * @param initial_voltage Starting voltage setpoint.
     * @param step_size Voltage increment/decrement step size.
     */
    MpptEngine(float initial_voltage, float step_size);

    /**
     * @brief Compute next optimal voltage setpoint based on live voltage and current readings.
     * @param v_pv Measured photovoltaic input voltage.
     * @param i_pv Measured photovoltaic input current.
     * @return Adjusted target voltage setpoint for the DC-DC stage.
     */
    float update(float v_pv, float i_pv);

    /**
     * @brief Retrieve current target voltage setpoint.
     * @return Current voltage setpoint in volts.
     */
    float getSetpoint() const { return current_voltage_setpoint; }
};

#endif // MPPT_ENGINE_H
