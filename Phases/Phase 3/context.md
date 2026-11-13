# Phase 3 — Backend and caregiver dashboard

Planned: 13 November 2026 (Asia/Calcutta). Status: pending; no completion evidence recorded.

Connect local watch events to the selected caregiver application. Work is planned; backend and dashboard are not demonstrated.

## Requirements and work

- [ ] Define device identities, unique event IDs, timestamps, validity fields and event payloads.
- [ ] Build the Node.js/TypeScript/Express API with PostgreSQL.
- [ ] Implement revocable device authentication, caregiver sign-in and device ownership enforcement.
- [ ] Persist accepted events and a durable SMS outbox before attempting notifications.
- [ ] Build the React/TypeScript/Vite dashboard with SSE updates.
- [ ] Show SOS, experimental falls, connection state, last-update time and alert history.
- [ ] Implement caregiver acknowledgement separately from message acceptance and delivery.
- [ ] Represent missing/stale data explicitly; label all mock telemetry.
- [ ] Implement reconnect, event deduplication and bounded outbox retry behaviour.

## Completion evidence

Verify authenticated event ingestion, durable storage, live updates, acknowledgement and reconnect behaviour using test data and a fake SMS adapter. Check that caregivers cannot access unassigned devices and devices cannot impersonate one another. Keep credentials out of firmware/browser code and Git.

Background caregiver push notifications are outside the selected initial dashboard scope. The final-phase CSI alert is local watch vibration.

## Next phase

[Phase 4: deployment and caregiver SMS](../Phase%204/context.md).
