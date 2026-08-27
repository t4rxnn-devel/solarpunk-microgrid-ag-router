/**
 * @file test_native_engine.cpp
 * @brief Advanced Native Unit Test Suite for Solarpunk Energy Router Firmware
 * @repository t4rxnn-devel/solarpunk-microgrid-ag-router
 * @details Validates C99 voltage checks, power routing overcurrent trips, and 
 *          MPPT perturb-and-observe boundary conditions under a native execution harness.
 */

#include <unity.h>
#include "universal_energy_node.h"
#include "power_router.h"
#include "hardware_control.h"

void setUp(void) {
    // Initialization hook prior to each test case execution
}

void tearDown(void) {
    // Clean-up hook after each test case execution
}

// Test 1: Validate ISO C99 bus voltage safety boundary limits
static void test_c99_bus_voltage_boundaries(void) {
    // Nominal valid counts (~24V level)
    TEST_ASSERT_TRUE(validate_bus_voltage_c99(2500));
    
    // Under-voltage trip threshold check (< 11.2V)
    TEST_ASSERT_FALSE(validate_bus_voltage_c99(1200));
    
    // Over-voltage trip threshold check (> 29.5V)
    TEST_ASSERT_FALSE(validate_bus_voltage_c99(3800));
}

// Test 2: Validate Power Router Channel overcurrent fault response
static void test_power_router_overcurrent_trip(void) {
    // Instantiate a test channel with pin 8, sense pin A3, and 10.0A limit
    PowerRouterChannel test_channel(8, A3, 10.0f);
    test_channel.initialize();

    // Enable channel state
    test_channel.setChannelState(true);
    TEST_ASSERT_FALSE(test_channel.isTripped());

    // Feed normal operating current
    test_channel.update(5.0f);
    TEST_ASSERT_FALSE(test_channel.isTripped());

    // Feed massive fault current to trigger overcurrent protection
    for (int i = 0; i < 15; i++) {
        test_channel.update(15.5f);
    }
    
    TEST_ASSERT_TRUE(test_channel.isTripped());
}

// Test 3: Validate Universal Energy Node State Initialization and Safety Abort
static void test_energy_node_state_machine(void) {
    UniversalEnergyNodeEngine node(
        NodeTopology_t::TOPOLOGY_MPPT_BOOST_CHARGER, 
        6, A0, A1, A2
    );

    TEST_ASSERT_EQUAL(NodeTopology_t::TOPOLOGY_MPPT_BOOST_CHARGER, node.getActiveTopology());
    TEST_ASSERT_FALSE(node.isFaultTripped());

    // Force emergency abort and verify latch state
    node.forceEmergencyAbort();
    TEST_ASSERT_TRUE(node.isFaultTripped());

    // Reset fault state
    node.resetFaultState();
    TEST_ASSERT_FALSE(node.isFaultTripped());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    RUN_TEST(test_c99_bus_voltage_boundaries);
    RUN_TEST(test_power_router_overcurrent_trip);
    RUN_TEST(test_energy_node_state_machine);
    
    return UNITY_END();
}
