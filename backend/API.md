# Bindu API contract (shared by backend/ and app/)
Base: /api/v1. JSON everywhere. Caregiver auth = server-side session cookie (httpOnly, sameSite=lax). Errors: {"error": "<message>"} with proper status (400 validation, 401 unauthenticated, 403/404 not your device, 404 missing).
Device ids are text slugs, e.g. "bindu-001". Device status computed from last_seen: "online" (<2 min), "stale" (<10 min), "offline" (older or null/never).

Device ingestion:
- POST /api/v1/events  header Authorization: Bearer <device token>
  body {event_id: uuid, type: "SOS"|"FALL"|"FALL_CANCELLED"|"HEARTBEAT", value?: number, ts: ISO-8601, mode?: "DAY"|"NIGHT"|"ALERT"|"SAFETY", source?: "simulated"}
  201 new {event: Event, alert: Alert|null}; 200 duplicate (same device+event_id) with the same shape of the existing record; 401 missing/bad/revoked token; 400 invalid body.
  FALL_CANCELLED: store event; if there is an open FALL alert for that device, set it to state "cancelled" (no new SMS).

Caregiver:
- POST /api/v1/auth/login {email,password} -> 200 {caregiver:{id,email,name}} | 401
- POST /api/v1/auth/logout -> 204
- GET  /api/v1/auth/me -> 200 {caregiver} | 401
- GET  /api/v1/devices -> {devices: Device[]}
- GET  /api/v1/devices/:deviceId -> {device: Device}
- GET  /api/v1/devices/:deviceId/events?limit=50 -> {events: Event[]} newest first
- GET  /api/v1/devices/:deviceId/alerts?limit=50 -> {alerts: Alert[]} newest first
- POST /api/v1/alerts/:alertId/acknowledge -> {alert: Alert} (idempotent: already-acked returns current alert, 200)
- GET  /api/v1/stream  (SSE, caregiver session) — only that caregiver's devices.
   SSE messages: `event: device`  data: Device   (on any device last_seen change, e.g. heartbeat)
                 `event: event`   data: Event    (new device event)
                 `event: alert`   data: Alert    (new alert OR any update: ack, cancel, sms status change — client upserts by id)
   Comment `: ping` every 15 s. Each message has an `id:` (monotonic); on reconnect the client may simply refetch everything.

Shapes (snake_case, timestamps ISO strings, null when unknown):
Device = {id, name, last_seen: string|null, status: "online"|"stale"|"offline", open_alerts: number}
Event  = {id: number, device_id, event_id, type, value: number|null, mode: string|null, source: "device"|"simulated", device_ts, received_at}
Alert  = {id: number, device_id, event_id, type: "SOS"|"FALL", value: number|null, source: "device"|"simulated", state: "open"|"acknowledged"|"cancelled", created_at, acknowledged_at: string|null, acknowledged_by: string|null (caregiver email/name), sms: {status: "pending"|"sent"|"failed"|"none", attempts: number, last_error: string|null, sent_at: string|null}}
