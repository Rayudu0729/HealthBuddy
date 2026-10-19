# HealthBuddy — Saathi wearable

An always-on AI companion wearable for elderly people: talks in Telugu or Hindi, predicts and prevents falls, detects emergencies, tracks location and alerts caregivers. Built for HackWithAditya 2026 (HealthTech track).

> "Other devices call for help after you fall. Ours stops the fall."

Full plan (roles, week-by-week tasks, parts, pitch): [docs/HackWithAditya-Plan.html](docs/HackWithAditya-Plan.html)

## Repo layout

| Folder | What lives here | Owner |
| --- | --- | --- |
| `firmware/` | ESP-IDF project for the ESP32-S3 wearable, including SaathiOS | Member 2 (hardware) + Lead (SaathiOS, voice) |
| `backend/` | MQTT/Firebase, SMS and missed-call handling, APIs | Member 1 |
| `app/` | Caregiver dashboard or app | Member 1 |
| `ml/` | Walking-stability score, risk causes, anomaly detection | Lead |
| `data/` | Recorded test sessions (falls, stand-ups, wake word) | Member 3 |
| `docs/` | Plan, architecture, interview notes, pitch material | Member 3 |

## Firmware quick start

Requirements: ESP-IDF v5.x installed (VS Code ESP-IDF extension is the easiest way), an ESP32-S3 N16R8 board.

```bash
cd firmware
idf.py set-target esp32s3
idf.py build
idf.py -p COM5 flash monitor
```

Replace `COM5` with your board's port. On boot the serial log should end with `Saathi ready`.

## Team workflow

- Everyone commits their own work under their own Git name. The ownership viva checks commit history.
- Work on a branch per feature (`feature/fall-detect`, `feature/voice-pipeline`), open a pull request, the lead reviews.
- Never commit API keys. Copy `backend/.env.example` to `backend/.env` locally.
- Credit any open-source library you use in this README under **Credits**.

## Key dates (IST)

- 25 Nov, 11:59 PM — Level 1 prototype submission
- 17 Dec — baseline freeze
- 18–20 Dec — 48-hour on-site build

## Credits

- [ESP-IDF](https://github.com/espressif/esp-idf) and FreeRTOS (Espressif)
