# Bindu — Standalone Safety and Accessibility Wearable

Bindu is a standalone 4G wrist-wearable project for older adults, blind and low-vision users, and their caregivers. The planned design combines physical SOS, motion-based fall detection, gesture-activated obstacle warnings, and watch-removal alerts. Location sharing, vitals and Telugu/Hindi voice interaction are planned extensions. Built for HackWithAditya 2026 (HealthTech track).

## Current status — 21 October 2026

The repository contains an ESP-IDF/FreeRTOS firmware scaffold: event dispatch, task priorities, power-mode state, and component task loops. Hardware readings, SMS, voice and cloud sync are currently stubs. No end-to-end hardware demonstration or measured accuracy/battery results are available yet.

The XIAO ESP32-S3 Plus, MPU6050 and the eight remaining items in the latest shopping list have been ordered; receipt is not yet confirmed. Follow the [build tracker](docs/build-tracker.md) for delivery checks, local component tests and integration milestones. The standalone milestone is SOS-button activation followed by caregiver SMS delivery over LTE with watch Wi-Fi disabled. The final wrist device requires an integrated PCB, battery/power design, antennas and a measured enclosure layout; its size and runtime are not yet validated.

Formatted submission PDF: [Download Bindu project submission](output/pdf/Bindu-Project.pdf).

Submission fields ready to copy: [Bindu project submission](docs/project-submission.html). Open in a browser; use its print button to save a PDF if required.

Full plan (roles, week-by-week tasks, parts, pitch): [docs/HackWithAditya-Plan.html](docs/HackWithAditya-Plan.html)

Parts to order, with order/received tracking: [docs/parts-list.md](docs/parts-list.md)

## Planned features

### Accessibility: obstacle alerts

Gesture-activated obstacle alerts for blind and low-vision users: raise the forearm so the watch's distance sensor faces forward, then make a deliberate wrist-bending gesture to activate scanning. Vibration pulses speed up as an obstacle gets closer, with an optional audible warning. Distinct vibration patterns confirm activation and deactivation.

This is a proposed feature, not implemented firmware. The gesture needs prototyping: a watch-mounted motion sensor detects watch movement, but hand bending alone may require an additional sensor across the wrist. See the [accessibility design notes](docs/HackWithAditya-Plan.html#obstacle-assistance) for constraints and validation.

### Watch-removal alerts

Notify the caregiver when the watch is removed, after a short confirmation delay to filter brief contact loss. Evaluate skin-contact sensing, optionally combined with a strap-open sensor; missing heart-rate readings alone must not be treated as proof of removal. Include a caregiver-approved charging/removal mode and a separate worn-again update. This feature is proposed, not implemented. See the [watch-removal design notes](docs/HackWithAditya-Plan.html#watch-removal-alerts).

## Repo layout

| Folder | What lives here | Owner |
| --- | --- | --- |
| `firmware/` | ESP-IDF project for the ESP32-S3 wearable, including BinduOS | Member 2 (hardware) + Lead (BinduOS, voice) |
| `backend/` | Planned Node.js/TypeScript/Express API, PostgreSQL, httpSMS integration; deployed on existing Kubernetes VM | Member 1 |
| `app/` | Planned React/TypeScript caregiver dashboard, built with Vite | Member 1 |
| `ml/` | Walking-stability score, risk causes, anomaly detection | Lead |
| `data/` | Recorded test sessions (falls, stand-ups, wake word) | Member 3 |
| `docs/` | Plan, architecture, interview notes, pitch material | Member 3 |

## Firmware quick start

Selected application architecture: wearable → authenticated Express HTTPS API → PostgreSQL → React caregiver dashboard (live SSE updates). Run containerised frontend, API and SMS worker on the existing Kubernetes VM, with persistent database storage. SMS route: backend → httpSMS API → dedicated Android gateway phone → caregiver SMS. The wearer does not need a nearby phone, but delivery requires internet, a reachable backend and an operational gateway phone. Use open-source application software and httpSMS's free tier (currently up to 200 sent/received SMS per month); existing infrastructure and SIM/data/SMS charges remain separate. Gateway credentials stay server-side. Frontend/backend implementation is pending.

Requirements: ESP-IDF v5.x installed. The selected controller is the XIAO ESP32-S3 Plus. The existing firmware configuration originated with an ESP32-S3 N16R8 board; verify the exact XIAO's flash/PSRAM settings and allocate its GPIOs before connecting hardware or flashing.

```bash
cd firmware
idf.py set-target esp32s3
idf.py build
idf.py -p COM5 flash monitor
```

Replace `COM5` with your board's port. On boot the serial log should end with `Bindu ready`.

## Team workflow

- Everyone commits their own work under their own Git name. The ownership viva checks commit history.
- Work on a branch per feature (`feature/fall-detect`, `feature/voice-pipeline`), open a pull request, the lead reviews.
- Never commit API keys. Store future backend secrets in a local `backend/.env`; `backend/.env.example` records the planned configuration fields.
- Credit any open-source library you use in this README under **Credits**.

## Key dates (IST)

- 25 Nov, 11:59 PM — Level 1 prototype submission
- 17 Dec — baseline freeze
- 18–20 Dec — 48-hour on-site build

## Credits

- [ESP-IDF](https://github.com/espressif/esp-idf) and FreeRTOS (Espressif)
