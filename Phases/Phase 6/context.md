# Phase 6 — Watch-removal detection

Planned: 13 November 2026 (Asia/Calcutta). Status: pending; no completion evidence recorded.

Notify the caregiver about confirmed watch removal and restoration. Work is planned; removal-sensor selection remains pending.

## Requirements and work

- [ ] Evaluate insulated skin-contact sensing and/or a strap-open sensor.
- [ ] Select hardware after testing contact reliability, wear comfort and available pins.
- [ ] Implement worn, removed and unknown states.
- [ ] Use a confirmation delay to filter brief contact loss and loose straps.
- [ ] Provide a caregiver-approved charging/intentional-removal mode.
- [ ] Send a separate worn-again update.
- [ ] Persist and deliver removal/restoration events through the existing backend and SMS route.
- [ ] Handle network loss and reconnect without duplicate alerts or misleading current state.

## Completion evidence

Record removal, refitting, loose strap, brief contact loss, charging-mode and disconnected-sensor trials. Verify notification history and state after network reconnection. Missing heart-rate readings alone must never establish removal.

## Next phase

[Phase 7: compact wearable integration](../Phase%207/context.md).
