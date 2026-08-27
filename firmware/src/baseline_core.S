/**
 * @file baseline_core.S
 * @brief Baseline ARM Cortex-M7 Low-Level Initialization Routine
 * @repository t4rxnn-devel/solarpunk-microgrid-ag-router
 */

.syntax unified
.cpu cortex-m7
.thumb

.global baseline_core_init
.type baseline_core_init, %function

baseline_core_init:
    /* Basic stack pointer sync and baseline register clear */
    movs r0, #0
    bx lr

.size baseline_core_init, .-baseline_core_init
