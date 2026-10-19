# Backend (Member 1)

Receives readings from the wearable, stores them, and sends alerts.

## To decide in Week 1
- Firebase (fastest to start, free tier) or MQTT broker (e.g. Mosquitto) + small API.

## Responsibilities
- Store readings: heart rate, SpO2, temperature, steps, stability score, location.
- Alert service: push notifications to the caregiver app.
- Missed-call mode and SMS fallback logic (device side lives in `firmware/components/alerts`).
- Medicine log and caregiver contacts.

## Reading format (agree with Lead + Member 2 in Week 1)

```json
{
  "device_id": "saathi-001",
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

Secrets go in `.env` (copy from `.env.example`), never in Git.
