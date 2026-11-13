# Phase 5 — Standalone LTE operation

Planned: 13 November 2026 (Asia/Calcutta). Status: pending; no completion evidence recorded.

Enable caregiver event delivery without watch Wi-Fi or a nearby wearer phone. Work is planned; LTE hardware selection and integration remain pending.

## Requirements and work

- [ ] Select a documented LTE development board after checking network bands, supply requirements and UART levels.
- [ ] Source the matching antenna/RF cable, activated data SIM and regulated supply.
- [ ] Verify peak-current capability; do not power the modem from the XIAO 3.3 V pin.
- [ ] Allocate GPIOs and implement cellular AT-command integration, HTTPS and reconnect handling.
- [ ] Queue and retry events across connection loss with event deduplication.
- [ ] Keep local sensing, SOS handling and obstacle vibration responsive during modem work.
- [ ] Distinguish queued, uploaded and remotely delivered alert states.

## Completion evidence

With watch Wi-Fi disabled and no nearby wearer phone, demonstrate physical SOS -> LTE data -> backend -> gateway -> caregiver SMS. Measure latency, reconnect behaviour and supply stability during transmission. Remote delivery still needs internet/backend/gateway availability.

Direct modem SMS remains an optional future fallback, not the selected first delivery route.

## Next phase

[Phase 6: watch-removal detection](../Phase%206/context.md).
