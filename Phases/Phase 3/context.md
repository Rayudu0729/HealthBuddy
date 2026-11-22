# Phase 3 — Backend and caregiver dashboard

Planned: 13 November 2026 (Asia/Calcutta). Status: implemented locally with tests (22 November); not deployed and not yet fed by the real watch.

Connect local watch events to the selected caregiver application.

## Requirements and work

- [x] Define device identities, unique event IDs, timestamps, validity fields and event payloads.
- [x] Build the Node.js/TypeScript/Express API with PostgreSQL.
- [x] Implement revocable device authentication, caregiver sign-in and device ownership enforcement.
- [x] Persist accepted events and a durable SMS outbox before attempting notifications.
- [x] Build the React/TypeScript/Vite dashboard with SSE updates.
- [x] Show SOS, experimental falls, connection state, last-update time and alert history.
- [x] Implement caregiver acknowledgement separately from message acceptance and delivery.
- [x] Represent missing/stale data explicitly; label all mock telemetry.
- [x] Implement reconnect, event deduplication and bounded outbox retry behaviour.

## 22 November 2026 implementation

`backend/` (Express 5, PostgreSQL, zod, sessions, SSE, outbox with fake and httpSMS adapters) and `app/` (React/Vite dashboard). `docker-compose.yml` runs PostgreSQL 16. API reference: [backend/API.md](../../backend/API.md). Devices post `POST /api/v1/events` with a bearer token; duplicate `event_id` returns 200, new returns 201. `npm run simulate` sends labelled simulated events.

Evidence: backend build passes and 11 API tests pass (pg-mem: auth, ownership, dedupe, acknowledgement, outbox); 3 dashboard tests pass. Production startup refuses a missing `SESSION_SECRET`.

Open: session cookie needs `secure` behind HTTPS; SSE reconnect after an API restart is untested; real httpSMS delivery and deployment are Phase 4; firmware does not upload events yet.

## Completion evidence

Verify authenticated event ingestion, durable storage, live updates, acknowledgement and reconnect behaviour using test data and a fake SMS adapter. Check that caregivers cannot access unassigned devices and devices cannot impersonate one another. Keep credentials out of firmware/browser code and Git.

Background caregiver push notifications are outside the selected initial dashboard scope. The final-phase CSI alert is local watch vibration.

## Next phase

[Phase 4: deployment and caregiver SMS](../Phase%204/context.md).
