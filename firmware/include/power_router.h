/**
 * @file power_router.h
 * @brief Solid-State MOSFET High-Side Switching & Bus Protection Header
 * @repository t4rxnn-devel/solarpunk-microgrid-ag-router
 * @details Declares the industrial power routing channel manager, real-time overcurrent
 *          trip protection, thermal fault monitoring, gate driver interlocks, and
 *          dynamic shunt compensation routines for agricultural greenhouse grids.
 */

#ifndef POWER_ROUTER_H
#define POWER_ROUTER_H

#include <Arduino.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @namespace SolarpunkRouter
 * @brief Encapsulates microgrid safety constants and hardware limits.
 */
namespace SolarpunkRouter {
    constexpr float DEFAULT_SHUNT_RESISTANCE_OHMS = 0.005f; // 5 milli-ohm precision current shunt
    constexpr uint32_t DEFAULT_TRIP_HOLDOFF_MS = 500;       // 500ms grace period for transient inrush spikes
    constexpr uint8_t DEFAULT_DEBOUNCE_SAMPLES = 8;         // Moving average window for current filter
    constexpr float THERMAL_SHUTDOWN_THRESHOLD_C = 85.0f;   // MOSFET die thermal trip threshold
}

/**
 * @enum RouterState_t
 * @brief Explicit operational states for each power routing channel.
 */
enum class RouterState_t : uint8_t {
    STATE_IDLE = 0x00,
    STATE_ENERGIZED = 0x01,
    STATE_OVERCURRENT_TRIPPED = 0x02,
    STATE_THERMAL_FAULT = 0x03,
    STATE_EMERGENCY_LATCHED = 0x04
};

/**
 * @struct ChannelTelemetry_t
 * @brief Telemetry metrics specific to individual power routing channels.
 */
struct ChannelTelemetry_t {
    float current_amps;
    float voltage_drop_mv;
    float calculated_power_watts;
    RouterState_t operational_state;
    bool is_active;
    bool fault_tripped;
    uint32_t trip_timestamp_ms;
    uint32_t uptime_seconds;
};

/**
 * @class PowerRouterChannel
 * @brief Manages high-side power MOSFET channels with autonomous overcurrent protection.
 */
class PowerRouterChannel {
private:
    uint8_t control_pin;
    uint8_t current_sense_pin;
    float current_trip_threshold_amps;
    float debounce_current_sum;
    uint8_t sample_count;
    bool channel_active;
    bool overcurrent_fault;
    uint32_t fault_timestamp;
    
    RouterState_t current_state;
    float shunt_resistance_ohms;
    uint32_t trip_holdoff_ms;
    unsigned long channel_init_timestamp;

    // Internal safety calibration helper methods
    bool evaluateCurrentThreshold(float sample);
    void transitionState(RouterState_t new_state);

public:
    /**
     * @brief Construct a new Power Router Channel instance.
     * @param pin_gate GPIO pin mapped to the high-side MOSFET gate driver or relay latch.
     * @param pin_sense Analog feedback pin for current shunt differential amplifier.
     * @param trip_amps Maximum allowable continuous current threshold before emergency cut-off.
     */
    PowerRouterChannel(uint8_t pin_gate, uint8_t pin_sense, float trip_amps);

    /**
     * @brief Destructor to ensure safe gate shutdown on teardown.
     */
    ~PowerRouterChannel();

    /**
     * @brief Initialize hardware GPIO configuration and baseline pin states.
     */
    void initialize();

    /**
     * @brief Update channel state against live current feedback and evaluate fault limits.
     * @param measured_current_a Live filtered current reading in amperes.
     */
    void update(float measured_current_a);

    /**
     * @brief Manually command the power routing channel state.
     * @param state True to energize channel, false to disconnect load.
     */
    void setChannelState(bool state);

    /**
     * @brief Retrieve comprehensive channel telemetry metrics.
     * @param telemetry_out Reference to ChannelTelemetry_t struct to populate.
     */
    void getTelemetry(ChannelTelemetry_t &telemetry_out) const;

    /**
     * @brief Clear existing overcurrent fault flags and attempt automatic recovery.
     */
    void clearFault();

    /**
     * @brief Force an emergency override trip across the routing channel.
     */
    void forceEmergencyTrip();

    /**
     * @brief Check whether the channel is currently tripped due to overcurrent.
     * @return true if fault is active, false otherwise.
     */
    bool isTripped() const { 
        return (overcurrent_fault || current_state == RouterState_t::STATE_OVERCURRENT_TRIPPED); 
    }

    /**
     * @brief Check whether the channel output is currently energized.
     * @return true if active, false otherwise.
     */
    bool isActive() const { 
        return (channel_active && current_state == RouterState_t::STATE_ENERGIZED); 
    }

    /**
     * @brief Return the current active operational state enum.
     * @return RouterState_t active state.
     */
    RouterState_t getOperationalState() const {
        return current_state;
    }
};

#endif // POWER_ROUTER_H
