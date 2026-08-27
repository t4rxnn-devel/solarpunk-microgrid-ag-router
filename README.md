# ⚡ Solarpunk Microgrid AG Router
## Repository: [t4rxnn-devel/solarpunk-microgrid-ag-router](https://github.com/t4rxnn-devel/solarpunk-microgrid-ag-router)

> *"Because saving the world from grid collapse takes more than hoping for the best—it demands real MPPT, honest math, and code that doesn't melt your MOSFETs."*

---

## 🧭 Architectural Overview
The **Solarpunk Microgrid AG Router** is an industrial-grade power management and maximum power point tracking (MPPT) routing node built for off-grid greenhouses, agricultural microgrids, and anyone tired of commercial solar gear locking down their firmware.
```markdown
┌────────────────────────────────────────────────────────┐
│               Solar Photovoltaic Input                 │
└───────────────────────────┬────────────────────────────┘
▼
┌────────────────────────────────────────────────────────┐
│     Power Router & High-Side Shunt Protection Node     │
└───────────────────────────┬────────────────────────────┘
▼
┌────────────────────────────────────────────────────────┤
│    Adaptive Incremental Conductance & P&O MPPT Engine  │
└───────────────────────────┬────────────────────────────┘
▼
┌────────────────────────────────────────────────────────┐
│     Teensy 4.1 ARM Cortex-M7 Core & C99 Registers      │
└────────────────────────────────────────────────────────┘
```
---

## 📂 Repository Directory Architecture

```text
solarpunk-microgrid-ag-router/
├── 📁 .github/
│   └── workflows/
│       └── run_pipeline.yml          # Automated CI/CD build & unit testing pipeline
├── 📁 firmware/
│   ├── include/
│   │   ├── hardware_control.h        # ISO C99 direct memory-mapped register maps & barriers
│   │   ├── mppt_engine.h             # Adaptive P&O and incremental conductance class interface
│   │   └── power_router.h            # Solid-state MOSFET gate control & overcurrent latching
│   ├── src/
│   │   ├── advanced_switching.S      # Cycle-accurate ARM assembly for gate modulation
│   │   ├── baseline_core.S           # Low-level Cortex-M7 initialization bootstrap
│   │   ├── mppt_engine.cpp           # Advanced MPPT math engine with global sweep scanning
│   │   └── power_router.cpp          # Shunt voltage evaluation & safety trip state machine
│   ├── test/
│   │   └── test_native_engine.cpp    # Native Unity hardware-in-the-loop simulation tests
│   └── platformio.ini                # Exhaustive multi-target production build manifest
├── 📁 hardware/
│   ├── bill_of_materials.csv         # Complete component sourcing manifesto
│   └── schematics/                   # KiCad multi-layer PCB design files
├── 📁 scripts/
│   └── validate_environment.py       # Pre-build Python toolchain and dependency verifier
└── 📁 utilities/
    ├── hardware_ping_test.py         # Local serial telemetry link & CRC validation tool
    └── watchdog_daemon.py            # Greenhouse thermal safety and supervisor daemon

```

---

## 🔬 Core Theory & Mathematical Engines

### 1. Adaptive Perturb & Observe (P&O) with Global Scanning

Traditional MPPT algorithms oscillate around the maximum power point or get trapped in local maxima under partial shading. This engine handles it by:

* **Dynamic Step Sizing:** Scaling perturbation magnitude proportionally to the $\frac{dP}{dV}$ derivative slope. When nearing the true peak ($\frac{dP}{dV} \to 0$), the step sizes shrink automatically to stop jittering.
* **Global Sweep Scans:** Periodically dropping the voltage setpoint to sweep the entire I-V curve, ensuring you lock onto the absolute global maximum instead of a lazy local hump.

### 2. High-Side Solid-State Shunt Protection

* Uses precision $5\text{m}\Omega$ high-side shunt sensing.
* Evaluates real-time current loads against strict trip limits, isolating channels instantly via direct memory-mapped GPIO register writes (`DSB`/`ISB` memory barriers) so you don't have to wait for an interrupt scheduler to panic.

---

## ❓ Frequently Asked Questions (FAQ)

### Q: Why Teensy 4.1? Can I run this on a regular Arduino Uno?

**A:** Because a Teensy 4.1 runs at 600MHz and has a hardware FPU, whereas an Uno would melt trying to compute floating-point derivatives while maintaining a 50kHz PWM loop. Don't torture an Uno like that.

### Q: What happens if my solar panels get partially shaded?

**A:** The engine detects the power drop discrepancy, triggers a global sweep scan across the I-V curve, finds the actual highest energy peak, and locks back in without requiring a manual reset.

### Q: Do I need a full Codespace just to check if my code compiles?

**A:** No. We set up an automated GitHub Actions CI pipeline (`.github/workflows/run_pipeline.yml`) that builds the firmware, runs native unit tests, and validates scripts on every push automatically.

### Q: Why are there assembly files (`.S`) mixed in with C++?

**A:** For cycle-accurate gate modulation and low-level register synchronization where compiler abstractions just get in the way.

---

## 🎯 Where to Use

* **Decentralized Agricultural Microgrids:** Powering remote irrigation pumps and telemetry hubs autonomously.
* **Off-Grid Greenhouses:** Squeezing every last watt out of variable weather conditions.
* **Industrial Edge Solar Routers:** Deploying on ARM Cortex-M7 platforms where runtime reliability is non-negotiable.

---

## 📜 License

Distributed under the **Apache-2.0 License**. See [LICENSE](https://github.com/t4rxnn-devel/solarpunk-microgrid-ag-router/blob/main/LICENSE) for full details.

```

```
