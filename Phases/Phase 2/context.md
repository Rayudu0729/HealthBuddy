# Phase 2 — Local safety and obstacle assistance

Planned: 13 November 2026 (Asia/Calcutta). Status: in progress — Fall + SOS firmware written and building (22 November); not yet demonstrated on hardware.

Build the local watch behaviours for blind/low-vision users and older adults after Phase 1 sensor validation. Obstacle work waits on the Phase 1 ToF accuracy issue.

## Requirements and work

- [ ] Import the validated local VL53L1X diagnostic into the main firmware, preserving the ST license and version.
- [~] Integrate MPU6050 readings, DRV2605L/motor, buzzer and debounced physical buttons. (Code builds; hardware run pending.)
- [~] Implement physical SOS activation and clear acknowledgement; reserve the SOS control for emergencies. (1.5 s hold; hardware run pending.)
- [~] Implement the cancel/test interaction without ambiguous button actions. (Separate CANCEL button; hardware run pending.)
- [ ] Implement forearm-forward obstacle scanning and evaluate the proposed wrist-bending activation; add a sensor only if evidence shows the IMU cannot detect the interaction.
- [ ] Provide distinct activation/deactivation patterns, distance-dependent vibration and optional sound.
- [ ] Reject invalid/stale distance readings and signal sensing failure instead of implying a clear path.
- [ ] Evaluate experimental fall detection using controlled recordings and a cancellation window.
- [ ] Prioritise SOS and urgent obstacle feedback over informational notifications.

## 22 November 2026 Fall + SOS firmware

New components `i2c_bus`, `feedback` (buzzer + DRV2605L patterns) and `buttons`; `fall_detect` now reads the MPU-6050 at ±8 g, 100 Hz; `alerts` adds the cancel window. `idf.py -B build-demo build` in `firmware/` passes with no warnings. Sending is a log stub; no network upload yet.

| Part | XIAO pin | GPIO |
| --- | --- | --- |
| Buzzer (active high) | D0 | 1 |
| SOS button to GND | D1 | 2 |
| CANCEL button to GND | D2 | 3 |
| I²C SDA (MPU 0x68, DRV2605L 0x5A) | D4 | 5 |
| I²C SCL, 100 kHz | D5 | 6 |

Behaviour: SOS held 1.5 s → confirm pattern, alert sent. Impact > 2.5 g then 2 s still → 10 s countdown with a beep each second; CANCEL inside the window sends nothing. A missing MPU or DRV2605L disables only fall detection or haptics. Known gaps: mode stays ALERT after sending; SOS during a fall countdown replaces it; thresholds are untuned.

First flash on the personal PC (`D:\Bindu\fallsos-flash`) produced a 24-byte capture with only a bootloader fragment, so no runtime evidence yet. Re-capture with button and fall tests is next.

### 22 November 2026 bench wiring progress

The DRV2605L header is soldered and the board is in the breadboard at the end away from the XIAO, with pins in row E (INT, SDA, SCL, GND, VIN). It is not wired yet. The vibration motor, buzzer and buttons are not connected yet. The MPU-6050 and VL53L1X keep their existing wiring.

Breadboard notes: the two halves are numbered in opposite directions and the power rails break in the middle, so use pin names rather than hole numbers. With USB-C pointing right, the XIAO pins on the purple/blue-wire side are D0–D6 starting from the USB end; the other side is 5V, GND, 3V3 starting from the USB end. The XIAO GND, 3V3, D4 and D5 strips have no free holes.

Agreed wiring plan:

1. Move the VL53L1X GND (orange) and 3V3 (yellow) jumpers to the − and + rails, and wire the freed XIAO holes to the same rails. Bridge both rails across the middle break.
2. Move the VL53L1X SDA (dark blue) and SCL (green) jumpers to the DRV SDA/SCL strips, and wire the freed XIAO holes to the same strips (daisy chain). Wire DRV GND to −, VIN to +; leave INT open. Attach the motor to the +/− pads.
3. Buzzer: − to −, middle to +, S to D0.
4. SOS and CANCEL buttons across the centre gap: one leg to −, the diagonally opposite leg to D1 (SOS) or D2 (CANCEL).

Next: photo check of the wiring before USB power, then a serial capture of SOS, fall and cancel tests.

### 23 November 2026 bench wiring update

[Complete bench wiring SVG](../../docs/bench-wiring.svg). The user reported the DRV2605L signals, power and coin motor connected after installing the SOS and CANCEL buttons and wiring the buzzer. Runtime verification is pending.

- The DRV header starts at E0. With INT at that end, E1 is SDA, E2 SCL, E3 GND and E4 VIN. Leave INT open. Verify hole positions against the labels on the breakout.
- Both XIAO I²C strips were full. The user confirmed adding the DRV to the red VL53L1X SDA and SCL strips, keeping the sensors connected. B42 to C1 and B41 to C2 were the suggested jumpers, subject to physical alignment.
- DRV VIN shares XIAO 3V3 and DRV GND shares XIAO GND. The motor leads go to the dedicated OUT+ and OUT− pads.
- Buzzer orange S goes to XIAO D0 and green − goes to GND. Leave the yellow middle lead open until its function is identified. SOS connects D1 to GND on press, and CANCEL connects D2 to GND on press.

Next bench check: with USB unplugged, check for a 3V3-to-GND short and verify that both switches close only while pressed. Then boot the demo on the personal PC and capture DRV2605L detection and the SOS, fall and CANCEL tests. COM17 was the last recorded port.

## Completion evidence

Record real sensor/button/motor operation, alert response times, false activations and missed obstacles. Test ordinary wrist movement, different target surfaces/lighting, sensor disconnection and operation without internet. Use supervised obstacle trials and controlled fall simulations.

One forward-facing distance sensor cannot identify people, cover all directions or guarantee drop-off detection. Assistance supplements existing mobility aids.

## Next phase

[Phase 3: backend and caregiver dashboard](../Phase%203/context.md).
