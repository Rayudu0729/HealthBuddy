# Bindu backend (Member 1)

Planned self-hosted service for wearable telemetry, caregiver access and HTTP SMS delivery. Implementation is pending.

## Selected stack — implementation pending

- Node.js, TypeScript and Express: HTTPS API for device ingestion, caregiver sign-in and alert acknowledgement.
- PostgreSQL: readings, device/caregiver assignments, alert history and a durable SMS outbox.
- Server-Sent Events (SSE): live dashboard updates; caregiver actions use authenticated API requests.
- httpSMS gateway adapter: authenticated `POST https://api.httpsms.com/v1/messages/send`, using server-side `x-api-key`, E.164 `from`/`to` numbers, message `content` and an event-correlated `request_id`. Keep gateway credentials server-side; API acceptance, handset send/delivery and caregiver acknowledgement are separate states. Do not assume `request_id` guarantees idempotency without verifying provider behaviour.
- Container images deployed to the user's existing Kubernetes VM: frontend/API Deployments, Services and HTTPS ingress; PostgreSQL with persistent storage and backups. Cluster access, storage class and ingress configuration remain to be checked before deployment.

The wearable sends telemetry/events over HTTPS through Wi-Fi or LTE data. The API authenticates each device, validates payloads and deduplicates event IDs. It persists accepted alerts and outbox jobs before an SMS worker attempts delivery through the HTTP gateway. Retries must account for provider idempotency/status support so an uncertain response does not blindly create duplicate messages. No MQTT broker or Firebase service is required for the first version.

The selected SMS route is watch → internet → backend → httpSMS hosted API → dedicated Android gateway phone → caregiver. The gateway phone runs the httpSMS Android app, has internet and an active SIM/SMS allowance, and remains powered and connected; it can stay with the team rather than near the wearer. It can work with watch Wi-Fi unavailable if LTE data and all delivery components are reachable; it does not work without internet. Direct modem SMS is an optional future fallback, not the selected initial delivery route. Obstacle feedback and gesture detection remain local. Missed-call reply support needs separate telephony/gateway integration and is deferred.

Use the hosted httpSMS free plan for the demo: currently up to 200 sent/received SMS per month without a credit card. Carrier SMS allowances/charges still apply. Track quota use and gateway heartbeat, and consume authenticated/verified webhooks for status where supported. Do not infer that running Bindu on Kubernetes also self-hosts httpSMS; self-hosting that gateway is a separate integration.

Use open-source software and the existing infrastructure to avoid new managed-platform subscriptions. VM resources, storage, SIM/data and SMS gateway credits may still have costs; free SMS delivery is not assumed. Keep credentials in appropriately configured Kubernetes Secrets or an existing secret manager, not Git, firmware or browser code. Local development uses a local Node.js/PostgreSQL environment and a fake SMS adapter; no real SMS is sent by tests.

Selected services have not been provisioned or implemented yet.

## Responsibilities
- Store readings: heart rate, SpO2, temperature, steps, stability score, location.
- Alert service: live in-dashboard updates and HTTP SMS to configured caregivers.
- Device alert events originate in `firmware/components/alerts`; gateway delivery runs in the backend.
- Medicine log and caregiver contacts.

## Example reading format (final schema pending)

```json
{
  "device_id": "bindu-001",
  "ts": "2026-10-27T09:15:00+05:30",
  "hr": 72,
  "spo2": 97,
  "temp_c": 36.8,
  "stability": 0.82,
  "mode": "DAY",
  "lat": 17.0,
  "lon": 82.0
}
```

Local secrets belong in a git-ignored `.env`; `.env.example` records the planned configuration fields and must be wired up during implementation. Caregiver password hashing, server-side session handling, ownership checks and revocable per-device credentials must be implemented before exposing the API.

## References

- [Express](https://expressjs.com/)
- [PostgreSQL](https://www.postgresql.org/about/)
- [Kubernetes workloads](https://kubernetes.io/docs/concepts/workloads/)
- [httpSMS API](https://docs.httpsms.com/)
- [httpSMS plans](https://auth.httpsms.com/)
