# Phase 4 — Deployment and caregiver SMS

Planned: 13 November 2026 (Asia/Calcutta). Status: pending; no completion evidence recorded.

Deploy the application on the existing Kubernetes VM and connect the selected httpSMS gateway. Work is planned; deployment and SMS delivery are not demonstrated.

## Requirements and work

- [ ] Confirm cluster access, resources, HTTPS ingress, persistent storage and database backups.
- [ ] Containerise and deploy the frontend, API and SMS worker.
- [ ] Arrange a dedicated Android gateway phone with internet, the httpSMS app and an active SMS-capable SIM.
- [ ] Configure gateway credentials server-side using the selected secret-management mechanism.
- [ ] Verify current httpSMS allowance and carrier SMS terms during setup.
- [ ] Connect durable outbox jobs to the gateway adapter and supported delivery-status mechanisms.
- [ ] Distinguish API acceptance, handset send/delivery and caregiver acknowledgement.
- [ ] Handle uncertain responses according to verified provider idempotency/status support.
- [ ] Test backend, gateway and network outages without notification storms.

## Completion evidence

Record a physical SOS event reaching persistent storage, the dashboard and a caregiver SMS, including timestamps and delivery state. Demonstrate recovery from outages and database persistence across service restarts.

Route: watch internet -> HTTPS backend -> httpSMS -> Android gateway -> caregiver. The gateway can stay with the team. This route depends on internet and the gateway; local obstacle alerts must continue independently.

## Next phase

[Phase 5: standalone LTE operation](../Phase%205/context.md).
