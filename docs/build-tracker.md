# Bindu — Build plan and pending work

Last updated: 27 October 2026.

This is the current execution tracker for Bindu, our standalone 4G safety and accessibility watch. It records the agreed build sequence and outstanding work. `[x]` means confirmed complete; `[ ]` means pending. Ordering a part does not mean it has arrived or passed testing.

## 1. Current position

- [x] Project name: Bindu — Standalone Safety and Accessibility Wearable.
- [x] Initial controller and motion sensor ordered.
- [x] User confirmed that all eight items in the latest shopping list are ordered.
- [x] Firmware scaffold exists in the repository.
- [x] Frontend, backend, deployment and SMS approach selected.
- [x] Submission HTML and PDF prepared.
- [x] Receipt confirmed for all listed parts except the MPU6050 and Adafruit DRV2605L driver (27 October 2026).
- [ ] Receive MPU6050 and DRV2605L; verify invoice quantities, exact variants and condition of all parts.
- [x] Build, flash and boot Bindu firmware on the XIAO ESP32-S3 Plus (27 October 2026; user-provided serial log).
- [ ] Demonstrate real sensor readings and local alerts.
- [ ] Implement backend, dashboard and caregiver notifications.
- [ ] Complete standalone LTE operation and compact watch integration.

Hardware integrations are not yet verified. Existing firmware sensor, connectivity and other component stubs must be replaced with working implementations. Accuracy, final dimensions and battery runtime have not been measured.

## 2. Ordered hardware

All items below are user-confirmed ordered. Quantities are the requested quantities from the shopping list; confirm them against invoices on arrival. All except the MPU6050 and Adafruit DRV2605L driver are confirmed received as of 27 October 2026. Prices paid and exact delivery dates have not been recorded. The XIAO has passed initial build/flash/boot and PSRAM checks; peripheral tests remain pending.

| Item | Requested quantity | Ordered | Received | Tested | Purpose |
| --- | --- | --- | --- | --- | --- |
| XIAO ESP32-S3 Plus — HUBTRONICS | 1 | [x] | [x] | [x] | Main controller; initial boot and memory checks passed |
| MPU6050 — Quartz Components | 1 | [x] | [ ] | [ ] | Motion and gesture experiments |
| VL53L1X — Robokits | 1 | [x] | [x] | [ ] | Obstacle distance sensing |
| Adafruit DRV2605L driver — DigiKey India | 1 | [x] | [ ] | [ ] | Vibration motor control |
| Coin vibration motor — Robocraze | 1 | [x] | [x] | [ ] | Tactile alerts |
| Active buzzer module — Robocraze | 1 | [x] | [x] | [ ] | Audible alerts |
| 830-point breadboard — Robocraze | 1 | [x] | [x] | [ ] | Bench wiring |
| M-M / M-F / F-F jumper set — Robocraze | 1 set | [x] | [x] | [ ] | Prototype connections |
| 40-pin male header strips — Robocraze | 2 strips | [x] | [x] | [ ] | Board connections |
| 6 × 6 mm pushbuttons, 5 mm height — ThinkRobotics | 1 pack of 2 | [x] | [x] | [ ] | SOS and cancel/test controls |

Existing equipment: mobile charger labelled 5 V / 3 A and a cable. USB data connection verified by successful flashing on local COM17. XIAO headers are loose and unsoldered; the user has no soldering iron/solder available yet. Arrange soldering equipment or experienced help before wired tests. Availability of a multimeter and wrist mounting materials still needs checking.

These breakout boards support bench testing and a temporary wrist mockup. Final watch size requires a separate PCB, battery, antenna and enclosure fit review. See `parts-list.md` for the matching order and receipt record; do not order duplicate parts.

## 3. What we will build first

The first local demonstration will combine a physical SOS button, real motion/distance readings, vibration and sound. Next, connect events to the caregiver dashboard and SMS route. The standalone milestone requires successful event delivery over the watch's LTE connection with watch Wi-Fi disabled.

### Step 1 — Receive, inspect and prepare

- [ ] Record received boards, exact variants, quantities and any damage.
- [ ] Confirm USB data cable, multimeter and soldering access.
- [x] ESP-IDF v5.5.6 activated on the local computer; XIAO detected on COM17.
- [x] Boot verified with 16 MB flash and 8 MB octal PSRAM; SPI SRAM memory test passed.
- [ ] Finalise GPIO allocation before connecting peripherals.
- [ ] Make a pin allocation and wiring diagram before connecting modules.
- [ ] Check every module's supply voltage and I2C/UART logic levels.

Completion evidence recorded: on 27 October 2026, user-provided local build/serial output showed ESP-IDF v5.5.6, firmware revision `9c8b2c4`, 16 MB flash, 8 MB PSRAM, `SPI SRAM memory test OK`, `BinduOS started` and `Bindu ready`. This verifies initial boot, not sensor operation, long-term stability or alert delivery. `Returned from app_main()` is expected after creating the background tasks.

### Step 2 — Test each component separately

- [ ] Read stable MPU6050 acceleration and gyroscope samples.
- [ ] Read VL53L1X distance and validity/status values.
- [ ] Operate the coin motor through DRV2605L with a suitable supply and configuration.
- [ ] Test buzzer control and both physical buttons.
- [ ] Record wiring and simple component test results.

Completion evidence: logged real readings and repeatable output/button tests. Never drive the motor directly from a GPIO or assume a module accepts 5 V logic.

### Step 3 — Implement local watch behaviour

- [ ] Physical SOS activation with debounce and clear feedback.
- [ ] Cancel/test control with distinct feedback patterns.
- [ ] Obstacle scanning with distance-based vibration and optional sound.
- [ ] Distinct activation/deactivation feedback and a deliberate exit action.
- [ ] Test the intended interaction: forearm raised, sensor facing forward, then wrist bend.
- [ ] Log false activations during ordinary movement and tune the gesture.
- [ ] Evaluate motion-based fall detection with controlled recordings and a cancel window.

A watch-mounted MPU6050 senses watch movement. Bending the hand while the watch stays still may require an additional sensor across the wrist; this remains an experiment, not a confirmed capability. A single forward-facing distance sensor will also have coverage and material/lighting limitations. Obstacle assistance supplements existing mobility aids.

Completion evidence: a recorded local demonstration plus a written list of missed detections, false triggers and sensor limitations. Use supervised obstacle tests and controlled fall simulations; do not ask people to fall.

### Step 4 — Build the backend and caregiver dashboard

- [ ] Agree on event IDs, timestamps, payloads and device identities.
- [ ] Implement Node.js + TypeScript + Express API and PostgreSQL persistence.
- [ ] Implement device authentication, caregiver sign-in and device ownership checks.
- [ ] Build React + TypeScript + Vite dashboard with SSE updates.
- [ ] Show SOS, experimental fall events, connectivity, last update and alert history.
- [ ] Add caregiver acknowledgement and distinguish stale/unknown from current state.
- [ ] Implement durable SMS outbox, deduplication and bounded retries.
- [ ] Start with mock telemetry and a fake SMS adapter; label mock data clearly.

Completion evidence: authenticated event ingestion, persistent alert history, live dashboard updates and acknowledgement using test data.

### Step 5 — Connect httpSMS and deploy

Selected delivery path:

```text
Watch (Wi-Fi initially; LTE for standalone milestone)
  -> HTTPS backend on existing Kubernetes VM
  -> httpSMS API
  -> dedicated Android gateway phone with internet and SMS-capable SIM
  -> caregiver SMS
```

- [ ] Arrange and configure the dedicated Android gateway phone and its SIM.
- [ ] Configure httpSMS credentials on the server only.
- [ ] Record current service quota and carrier SMS allowance before real tests.
- [ ] Separate API acceptance, handset send/delivery and caregiver acknowledgement.
- [ ] Confirm Kubernetes access, ingress/TLS, persistent storage and backups.
- [ ] Containerise frontend, API and SMS worker and deploy them.
- [ ] Test backend, gateway-phone and network outages without duplicate alert storms.

The gateway phone can stay with the team; the wearer does not need a nearby phone. Remote delivery depends on internet, backend availability and the gateway. Local obstacle vibration must continue without those services. Direct modem SMS is a later possible fallback.

Completion evidence: one button event produces a stored alert, dashboard update and caregiver SMS, with measured latency and failure behaviour.

### Step 6 — Add standalone 4G

- [ ] Select and order a documented LTE development board after checking bands, UART levels and power requirements.
- [ ] Match the LTE antenna, RF connector/cable and activated data SIM.
- [ ] Provide a regulated supply sized for documented transmit-current peaks.
- [ ] Implement and test cellular AT-command integration, HTTPS and reconnect behaviour.
- [ ] Queue/retry events across connectivity loss with event deduplication.
- [ ] Demonstrate SOS-to-caregiver delivery with watch Wi-Fi disabled.

The modem and its accessories are not confirmed ordered. Do not power the modem from the XIAO's 3.3 V pin or assume that all modem boards accept the same supply voltage. The development board is for integration testing; final wrist packaging is a later gate.

### Step 7 — Implement watch-removal detection

- [ ] Evaluate insulated contact-pad experiments and/or strap-open sensing.
- [ ] Select the final removal sensor based on evidence.
- [ ] Confirm removal only after a delay; test loose straps and brief contact loss.
- [ ] Add caregiver-approved charging/removal handling.
- [ ] Send a worn-again update and show worn/removed/unknown states.
- [ ] Test notification behaviour during network loss and reconnection.

Missing heart-rate readings alone must not be interpreted as confirmed removal. No dedicated removal sensor is confirmed ordered.

### Step 8 — Make the compact wearable

- [ ] Measure idle, sensing, vibration and LTE transmit power consumption.
- [ ] Select protected LiPo capacity, dimensions and peak-current capability.
- [ ] Design charging, protection, power-path and regulators together.
- [ ] Select final modem, antennas and sensor orientation before PCB layout.
- [ ] Produce a mechanical layout with battery space, antenna clearance and accessible buttons.
- [ ] Design, fabricate, assemble and test the compact PCB.
- [ ] Build a 3D-printed case and adjustable strap; insulate all wrist-facing conductors.
- [ ] Test fit, heat, charging behaviour, radio reception and actual runtime.

Completion evidence: a self-contained battery-powered wrist prototype, measured dimensions/runtime and successful LTE alert delivery. Do not claim waterproofing, medical accuracy or production readiness without the corresponding validation.

## 4. Still pending to source or arrange

| Pending item | Decision depends on |
| --- | --- |
| LTE modem development board, antenna, RF cable and watch data SIM | Board documentation, network bands, power and UART compatibility |
| Android phone with httpSMS app, internet and SMS-capable SIM | Gateway availability and account setup |
| Multimeter and soldering equipment | Check existing team/lab equipment first |
| Temporary wrist strap, mount and insulated contact materials | Prototype dimensions and removal experiments |
| Additional wrist-bend sensor, if required | Results of IMU gesture tests |
| Final removal/strap sensor | Contact reliability experiments |
| Protected LiPo, charger, power-path, regulators and connectors | Measured current and final mechanical layout |
| Compact PCB, assembly, case and final strap | Electrical and mechanical design review |

Do not buy the final battery or generic charger/boost modules as a complete power solution before this review.

## 5. Later extensions

- [ ] GNSS/location sharing after confirming actual modem GNSS capability.
- [ ] Wrist vitals sensing after optical placement and signal-quality tests.
- [ ] Telugu/Hindi voice interaction after microphone, audio and interaction design decisions.
- [ ] Display, if useful within the space and power budget.

These are outside the first core demonstration and have no confirmed hardware orders in this tracker.

## 6. Cost and demonstration rules

- Use open-source application software and the existing Kubernetes VM.
- Use httpSMS within the available free allowance; verify current limits during setup.
- Track hardware, fabrication, SIM/data, carrier SMS and infrastructure costs separately. Free software does not make all operating costs zero.
- Keep credentials out of Git, browser code and public documents.
- Label simulated events and mock readings in the dashboard and presentation.
- Capture a short demonstration, wiring diagram, test evidence and known limitations for submission.
- Update this file as parts arrive and each milestone is demonstrated.

## 7. Immediate next action

The XIAO and other listed parts have arrived except the MPU6050 and DRV2605L driver. Initial build, flash and boot are verified. Arrange soldering of the loose XIAO headers and prepare the pin allocation from exact board documentation, then test the buttons and VL53L1X separately. MPU6050 motion tests must wait for delivery. Defer vibration-motor integration until the driver arrives; do not connect the motor directly to a GPIO. See [Phase 0 context](../Phases/Phase%200/context.md) for the verified local setup and boot evidence.
