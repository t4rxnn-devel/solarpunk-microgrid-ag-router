/**
 * @file main.cpp
 * @brief Main Execution Entry Point for Universal Solar Energy & Nanogrid Node
 * @repository t4rxnn-devel/solarpunk-microgrid-ag-router
 * @details Initializes the UniversalEnergyNodeEngine core, handles high-speed serial 
 *          telemetry streaming over USB, and manages safe watchdog interrupt triggers.
 */

#include <Arduino.h>
#include "universal_energy_node.h"

// Instantiate the Universal Energy Engine as an MPPT Solar Charger Node by default
// Parameters: Topology, PWM Gate Pin, Voltage In Pin, Current In Pin, Bus Voltage Pin
UniversalEnergyNodeEngine energyNode(
    NodeTopology_t::TOPOLOGY_MPPT_BOOST_CHARGER, 
    6,   // PWM Gate Pin
    A0,  // Solar Input Voltage Analog Pin
    A1,  // Shunt Current Sensor Analog Pin
    A2   // DC Nanogrid Bus Voltage Analog Pin
);

void setup() {
    // Initialize high-speed serial interface for binary telemetry streaming
    Serial.begin(500000);
    while (!Serial && millis() < 2500) {
        // Wait up to 2.5 seconds for serial port enumeration
    }

    Serial.println(F("[BOOT] Initializing Universal Solar Energy Node..."));
    
    // Initialize hardware registers, timers, and pin modes
    energyNode.initializeHardware();
    
    Serial.println(F("[BOOT] Hardware abstraction layer initialized successfully. Entering 100Hz tick loop."));
}

void loop() {
    NodeTelemetry_t telemetry;
    
    // Execute the deterministic 100Hz control loop tick
    energyNode.tick(telemetry);

    // Stream out structured telemetry over serial every loop execution or on event
    static uint32_t last_print_timestamp = 0;
    if (millis() - last_print_timestamp >= 200) { // 5Hz telemetry print rate to avoid serial saturation
        last_print_timestamp = millis();

        Serial.print(F("{\"v_in\":"));
        Serial.print(telemetry.input_voltage_v, 2);
        Serial.print(F(",\"i_in\":"));
        Serial.print(telemetry.input_current_a, 2);
        Serial.print(F(",\"v_bus\":"));
        Serial.print(telemetry.bus_voltage_v, 2);
        Serial.print(F(",\"soc\":"));
        Serial.print(telemetry.battery_soc_percent, 1);
        Serial.print(F(",\"temp\":"));
        Serial.print(telemetry.core_temperature_c, 1);
        Serial.print(F(",\"healthy\":"));
        Serial.print(telemetry.system_healthy ? "true" : "false");
        Serial.print(F(",\"uptime_ms\":"));
        Serial.print(telemetry.uptime_ms);
        Serial.println(F("}"));
    }
}
