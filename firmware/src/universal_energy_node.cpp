/**
 * @file universal_energy_node.h
 * @brief Universal Multi-Topology Solar Energy & DC Nanogrid Hardware Abstraction Header
 * @repository t4rxnn-devel/solarpunk-microgrid-ag-router
 * @details Declares the hardware-agnostic class interface for dynamic MPPT perturb-and-observe,
 *          bidirectional DC bus voltage regulation, and agricultural dosing node architectures.
 */

#ifndef UNIVERSAL_ENERGY_NODE_H
#define UNIVERSAL_ENERGY_NODE_H

#include <Arduino.h>

// Abstract Power Topology Enumerations
enum class NodeTopology_t {
    TOPOLOGY_MPPT_BOOST_CHARGER = 0,
    TOPOLOGY_DC_NANOGRID_ROUTER,
    TOPOLOGY_BIDIRECTIONAL_INVERTER_GATE,
    TOPOLOGY_PUMP_AGRICULTURAL_NODE
};

// Comprehensive System Telemetry Metrics Struct
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

// Universal Energy Node Engine Class Definition
class UniversalEnergyNodeEngine {
private:
    NodeTopology_t active_topology;
    uint32_t loop_interval_ms;
    uint32_t last_tick_timestamp;
    
    // Internal Control Variables
    float current_pwm_duty_cycle;
    float target_voltage_v;
    bool emergency_shutoff_tripped;
    uint32_t fault_recovery_counter;

    // Hardware Pin Mapping Configuration Registers
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
    // Constructor with explicit pin and topology bindings
    UniversalEnergyNodeEngine(NodeTopology_t topology, uint8_t pwm_pin, uint8_t vin_pin, uint8_t iin_pin, uint8_t bus_pin);

    // Hardware setup and GPIO pin initialization
    void initializeHardware();

    // Core deterministic tick loop (executed at configured frequency)
    void tick(NodeTelemetry_t &telemetry_out);

    // Configuration and override control methods
    void setTargetVoltage(float target_v);
    void forceEmergencyAbort();
    void resetFaultState();
    
    // Inline state getters
    bool isFaultTripped() const { return emergency_shutoff_tripped; }
    NodeTopology_t getActiveTopology() const { return active_topology; }
};

#endif // UNIVERSAL_ENERGY_NODE_H
