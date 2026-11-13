# Phase 2 — Local safety and obstacle assistance

Planned: 13 November 2026 (Asia/Calcutta). Status: pending; no completion evidence recorded.

Build the local watch behaviours for blind/low-vision users and older adults after Phase 1 sensor validation. Work is planned; no integrated demonstration is established.

## Requirements and work

- [ ] Import the validated local VL53L1X diagnostic into the main firmware, preserving the ST license and version.
- [ ] Integrate MPU6050 readings, DRV2605L/motor, buzzer and debounced physical buttons.
- [ ] Implement physical SOS activation and clear acknowledgement; reserve the SOS control for emergencies.
- [ ] Implement the cancel/test interaction without ambiguous button actions.
- [ ] Implement forearm-forward obstacle scanning and evaluate the proposed wrist-bending activation; add a sensor only if evidence shows the IMU cannot detect the interaction.
- [ ] Provide distinct activation/deactivation patterns, distance-dependent vibration and optional sound.
- [ ] Reject invalid/stale distance readings and signal sensing failure instead of implying a clear path.
- [ ] Evaluate experimental fall detection using controlled recordings and a cancellation window.
- [ ] Prioritise SOS and urgent obstacle feedback over informational notifications.

## Completion evidence

Record real sensor/button/motor operation, alert response times, false activations and missed obstacles. Test ordinary wrist movement, different target surfaces/lighting, sensor disconnection and operation without internet. Use supervised obstacle trials and controlled fall simulations.

One forward-facing distance sensor cannot identify people, cover all directions or guarantee drop-off detection. Assistance supplements existing mobility aids.

## Next phase

[Phase 3: backend and caregiver dashboard](../Phase%203/context.md).
