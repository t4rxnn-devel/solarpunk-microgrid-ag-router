/**
 * @file power_router.h
 * @brief Solid-State MOSFET High-Side Switching & Bus Protection Header
 * @repository t4rxnn-devel/solarpunk-microgrid-ag-router
 * @details Declares the industrial power routing channel manager, real-time overcurrent 
 *          trip protection, thermal fault monitoring, and gate driver interlocks.
 */

#ifndef POWER_ROUTER_H
#define POWER_ROUTER_H

#include <Arduino.h>

/**
 * @struct ChannelTelemetry_t
 * @brief Telemetry metrics specific to individual power routing channels.
 */
struct ChannelTelemetry_t {
    float current_amps;
    float voltage_drop_mv;
    bool is_active;
    bool fault_tripped;
    uint32_t trip_timestamp_ms;
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

    // Internal safety calibration parameters
    const float shunt_resistance_ohms = 0.005f; // 5 milli-ohm current shunt resistor
    const uint32_t trip_holdoff_ms = 500;       // 500ms grace period for transient inrush spikes

public:
    /**
     * @brief Construct a new Power Router Channel instance.
     * @param pin_gate GPIO pin mapped to the high-side MOSFET gate driver or relay latch.
     * @param pin_sense Analog feedback pin for current shunt differential amplifier.
     * @param trip_amps Maximum allowable continuous current threshold before emergency cut-off.
     */
    PowerRouterChannel(uint8_t pin_gate, uint8_t pin_sense, float trip_amps);

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
     * @brief Check whether the channel is currently tripped due to overcurrent.
     * @return true if fault is active, false otherwise.
     */
    bool isTripped() const { return overcurrent_fault; }

    /**
     * @brief Check whether the channel output is currently energized.
     * @return true if active, false otherwise.
     */
    bool isActive() const { return channel_active; }
};

#endif // POWER_ROUTER_H
