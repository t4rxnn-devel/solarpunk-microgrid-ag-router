/**
 * @file universal_energy_node.h
 * @brief Universal Multi-Topology Solar Energy & DC Nanogrid Hardware Abstraction Header
 * @repository t4rxnn-devel/solarpunk-microgrid-ag-router
 * @details Declares the complete hardware-agnostic class interface for dynamic MPPT perturb-and-observe,
 *          bidirectional DC bus voltage regulation, fault latching registers, and agricultural dosing nodes.
 */

#ifndef UNIVERSAL_ENERGY_NODE_H
#define UNIVERSAL_ENERGY_NODE_H

#include <Arduino.h>

/**
 * @enum NodeTopology_t
 * @brief Defines the operational topology mode compiled into the hardware abstraction layer.
 */
enum class NodeTopology_t {
    TOPOLOGY_MPPT_BOOST_CHARGER = 0,
    TOPOLOGY_DC_NANOGRID_ROUTER,
    TOPOLOGY_BIDIRECTIONAL_INVERTER_GATE,
    TOPOLOGY_PUMP_AGRICULTURAL_NODE
};

/**
 * @struct NodeTelemetry_t
 * @brief Comprehensive real-time system metrics captured during every core loop tick.
 */
struct NodeTelemetry_t {
    float input_voltage_v;
    float input_current_a;
    float bus_voltage_v;
    float battery_soc_percent;
    float core_temperature_c;
    uint32_t active_fault_flags;
    bool system_healthy;
    uint32_t uptime_ms;
};

/**
 * @class UniversalEnergyNodeEngine
 * @brief Core state machine and execution controller managing power routing and solar energy harvesting.
 */
class UniversalEnergyNodeEngine {
private:
    NodeTopology_t active_topology;
    uint32_t loop_interval_ms;
    uint32_t last_tick_timestamp;
    
    // Internal Control Registers
    float current_pwm_duty_cycle;
    float target_voltage_v;
    bool emergency_shutoff_tripped;
    uint32_t fault_recovery_counter;

    // Hardware Pin Mapping Registers
    uint8_t pin_pwm_gate;
    uint8_t pin_adc_vin;
    uint8_t pin_adc_iin;
    uint8_t pin_adc_bus;
    uint8_t pin_fault_latch;

    // Private Subroutine Dispatch Handlers
    void executePerturbAndObserveMPPT(float v_in, float i_in);
    void executeBusLoadShedding(float bus_v);
    void executePrecisionPulseDosing(float bus_v);
    void executeSafeShutdown();

public:
    /**
     * @brief Construct a new Universal Energy Node Engine instance.
     * @param topology Active operational topology mode.
     * @param pwm_pin GPIO pin mapped to high-frequency PWM gate driver.
     * @param vin_pin Analog input pin for solar photovoltaic voltage sensing.
     * @param iin_pin Analog input pin for shunt current amplification feedback.
     * @param bus_pin Analog input pin for DC nanogrid bus monitoring.
     */
    UniversalEnergyNodeEngine(NodeTopology_t topology, uint8_t pwm_pin, uint8_t vin_pin, uint8_t iin_pin, uint8_t bus_pin);

    /**
     * @brief Initialize hardware GPIO directions, PWM frequencies, and baseline states.
     */
    void initializeHardware();

    /**
     * @brief Core deterministic tick loop executing at high frequency to prevent jitter.
     * @param telemetry_out Reference to telemetry struct populated with latest sensor values.
     */
    void tick(NodeTelemetry_t &telemetry_out);

    /**
     * @brief Set the target regulation voltage for the active power stage.
     * @param target_v Desired bus or battery charging voltage.
     */
    void setTargetVoltage(float target_v);

    /**
     * @brief Force an immediate software-level emergency shutdown and latch fault lines.
     */
    void forceEmergencyAbort();

    /**
     * @brief Clear existing fault registers and resume normal operational state.
     */
    void resetFaultState();
    
    /**
     * @brief Check whether an emergency fault latch has been tripped.
     * @return true if fault is active, false otherwise.
     */
    bool isFaultTripped() const { return emergency_shutoff_tripped; }

    /**
     * @brief Retrieve the currently active hardware topology mode.
     * @return NodeTopology_t enum value.
     */
    NodeTopology_t getActiveTopology() const { return active_topology; }
};

#endif // UNIVERSAL_ENERGY_NODE_H
