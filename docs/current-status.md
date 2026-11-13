# Bindu — Current project status

## Latest test update — 14 November 2026

Final handoff: active diagnostic now uses SHORT mode (1), 50 ms timing budget,
100 ms period, 50 kHz I2C on D4/D5, with TOF_QUALITY logging. Latest 20 cm
short-mode test: 451 valid samples, mean 17.994 cm, range 17.6–18.4 cm, no
I2C/invalid errors. No manual correction applied. Accuracy remains unresolved;
user lacks the calibrated 17% gray target specified by ST for offset calibration.
Keep the sensor for now; consider borrowing a target and testing other modules
separately. Read the 14 November Phase 1 handoff for full evidence and setup
limitations. The five-minute result below was in long mode. All monitors closed.

On the D:\Bindu computer, COM17, the standalone ranging diagnostic now uses
SDA D4/GPIO5, SCL D5/GPIO6 and 50 kHz I2C. Fresh builds/flashes verified their
written hashes. Earlier tests today still encountered intermittent communication
failures; cause of subsequent recovery is not isolated. MPU6050 substitution
acknowledged at 0x68. VL53L1X subsequently acknowledged 0x29 and ID 0xEACC.

Latest stationary test at the user-confirmed 20 cm target captured 3,196 valid
readings across 314.14 seconds: mean 19.664 cm, range 17.4–20.8 cm, zero invalid
measurements and zero I2C error messages. Official monitor startup reset the MCU;
monitor is closed. This passes this five-minute communication check, not proof
that all intermittent faults are eliminated. No calibration correction applied.

Other same-day captures: 10 cm mean 8.852 cm (328 samples); repeat assumed still
10 cm mean 8.988 cm (366); 20 cm mean 19.729 cm (300), later repeat 19.116 cm
(350); improved-setup 50 cm mean 45.670 cm (400), range 44.1–46.7 cm. These runs
had no I2C/invalid messages but demonstrate remaining accuracy/repeatability
questions. Raw logs and summaries are in data/samples/vl53l1x-2026-11-14-*.
Next investigate measurement setup and ST-documented calibration; do not apply
a guessed constant correction or mark Phase 1 complete. The older handoff and
environment details below are historical where superseded by this update.

Reviewed: 13 November 2026 (Asia/Calcutta). Latest physical test evidence: 13 November 2026. Ranging is demonstrated; accuracy and intermittent I2C faults remain unresolved. No deployment or later hardware purchase is confirmed.

## Source of truth

This status summary and the phase contexts describe actual progress. The product roadmap describes intended behaviour; it is not proof of implementation. Hardware evidence includes earlier user-supplied logs and direct COM3 build/flash/monitor checks on 13 November. The board was directly connected to this computer for the latest session; recheck access when moving computers.

| Area | Evidence / current status |
| --- | --- |
| Controller | XIAO ESP32-S3 Plus; 16 MB flash, 8 MB octal PSRAM; memory test and Bindu boot passed |
| Local tools | ESP-IDF v5.5.6 at C:\esp\v5.5.6\esp-idf; tools under C:\Espressif\tools; direct board access on COM3 this session |
| Initial parts | All ten items reported received and soldered on 1 November; invoice quantities and joint quality not independently checked |
| Distance sensor | VL53L1X: I2C acknowledgement at 0x29, ID 0xEACC and ranging demonstrated |
| Distance accuracy | Not passed: latest confirmed 10 cm target averaged 7.966 cm (157 samples); 20 cm averaged 17.067 cm (21 samples); prior 50 cm averaged 40.12 cm |
| Calibration settings | Reported offset=0 mm, xtalk=0 after ULD initialisation; no correction applied by our diagnostic |
| I2C reliability | ESP_ERR_INVALID_STATE captured on ID/data-ready reads; latest 20 cm run stopped after 21 valid readings; root cause unresolved |
| Sensor code location | Tracked standalone firmware/diagnostics/tof-check with ST ULD v3.5.5/licence and USB console; not integrated into Bindu main firmware |
| Other peripherals | MPU6050, DRV2605L/motor, buzzer and buttons received; operation not demonstrated |
| SOS / falls / wrist gesture / removal | Planned; no completed end-to-end demonstrations |
| Backend / dashboard | Selected architecture only; no implementation or deployment demonstrated |
| LTE / battery / custom PCB / case | Selection, sourcing, integration and measurements pending; no later-stage purchases confirmed |
| Camera | Explicitly excluded; retain the ordered Plus controller |
| Wi-Fi CSI room-motion experiment | Selected final phase only; additional stationary node, automatic vibration and button check planned; no hardware purchase or sensing result confirmed |

## Current scope

Core targets: physical SOS, experimental fall detection with cancellation, gesture-activated local obstacle feedback, removal/restoration notifications, standalone LTE event upload, caregiver dashboard and SMS. A wrist-mounted IMU may not detect hand bending if the watch stays stationary; validate that interaction before promising it.

Extensions: GNSS/location, optical vitals, Telugu/Hindi voice and ML trends. They do not take priority over reliable core functions. No clinical accuracy, fall prevention, guaranteed clear-path detection, final watch size or battery runtime is established.

The [complete phase roadmap](../Phases/README.md) ends with [Phase 8: Wi-Fi CSI](../Phases/Phase%208/context.md), after core wearable integration. It adds automatic watch vibration for monitored room movement and a non-SOS button check for recent movement. No-recent-event and unavailable responses must be distinct; motion is not proof that another person entered or that a path is safe. Current execution remains Phase 1.

## Selected architecture and cost constraint

Firmware: C / ESP-IDF / FreeRTOS, I2C, UART and planned cellular AT integration. Frontend: React / TypeScript / Vite. Backend: Node.js / TypeScript / Express, PostgreSQL and SSE. Deploy on the existing Kubernetes VM; cluster access, TLS, persistence and backups remain to be configured.

Remote alert route: watch Wi-Fi/LTE data -> authenticated backend -> httpSMS -> dedicated Android gateway with internet and active SMS-capable SIM -> caregiver. This gateway can remain with the team. Direct modem SMS is a later optional fallback, not the initial implementation. Local obstacle feedback is intended to work without network access, but is not integrated yet.

Use open-source software and available infrastructure. Recheck httpSMS free allowance and carrier terms at setup; no current pricing/stock/quotas were revalidated in this documentation review. Hardware, fabrication, infrastructure and SIM/data/SMS can still cost money. Keep credentials server-side and out of Git.

## Immediate next action

User is leaving and continuing in another session. Read [Phase 1 resume handoff](../Phases/Phase%201/context.md) first. SDA/SCL use jumper wires. The latest pending instruction is to disconnect USB, replace/reseat SDA/SCL with a short pair, preserve the pin mapping, and reconnect with a fixed white card at 20 cm. The user has not confirmed doing this.

Capture startup and readings with the official ESP-IDF monitor; check the current computer/COM port first. Resolve intermittent communication before calibration. Do not add guessed offsets or scaling. Once stable, validate multiple known distances and only then integrate obstacle feedback.

## Navigation

- [Phase index](../Phases/README.md)
- [Phase 0 setup evidence](../Phases/Phase%200/context.md)
- [Phase 1 diagnostic details](../Phases/Phase%201/context.md)
- [Build tracker](build-tracker.md)
- [Parts and receipt status](parts-list.md)
- [Submission narrative](project-submission.html)
