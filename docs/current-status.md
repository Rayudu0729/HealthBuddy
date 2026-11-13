# Bindu — Current project status

Reviewed: 13 November 2026 (Asia/Calcutta). Latest physical test evidence: 1 November 2026. No subsequent repair, accuracy validation, deployment or hardware purchase has been reported.

## Source of truth

This status summary and the phase contexts describe actual progress. The product roadmap describes intended behaviour; it is not proof of implementation. User-supplied local build/serial logs are the hardware evidence. The agent runs on a separate remote computer.

| Area | Evidence / current status |
| --- | --- |
| Controller | XIAO ESP32-S3 Plus; 16 MB flash, 8 MB octal PSRAM; memory test and Bindu boot passed |
| Local tools | ESP-IDF v5.5.6; board last detected at COM17 on the user's local computer |
| Initial parts | All ten items reported received and soldered on 1 November; invoice quantities and joint quality not independently checked |
| Distance sensor | VL53L1X: I2C acknowledgement at 0x29, ID 0xEACC and ranging demonstrated |
| Distance accuracy | Not passed: user-reported 20 cm targets produced approximately 13.3–17.4 cm across different runs; 50 cm sample averaged 40.12 cm |
| Calibration settings | Reported offset=0 mm, xtalk=0 after ULD initialisation; no correction applied by our diagnostic |
| I2C reliability | Intermittent ID/read-ready failures returned -1; adapter now prints underlying read errors, but no failing I2C_DIAG line has been shared |
| Sensor code location | Separate local D:\Bindu\tof-check diagnostic; not yet imported into this repository or Bindu's main firmware |
| Other peripherals | MPU6050, DRV2605L/motor, buzzer and buttons received; operation not demonstrated |
| SOS / falls / wrist gesture / removal | Planned; no completed end-to-end demonstrations |
| Backend / dashboard | Selected architecture only; no implementation or deployment demonstrated |
| LTE / battery / custom PCB / case | Selection, sourcing, integration and measurements pending; no later-stage purchases confirmed |
| Camera | Explicitly excluded; retain the ordered Plus controller |

## Current scope

Core targets: physical SOS, experimental fall detection with cancellation, gesture-activated local obstacle feedback, removal/restoration notifications, standalone LTE event upload, caregiver dashboard and SMS. A wrist-mounted IMU may not detect hand bending if the watch stays stationary; validate that interaction before promising it.

Extensions: GNSS/location, optical vitals, Telugu/Hindi voice and ML trends. They do not take priority over reliable core functions. No clinical accuracy, fall prevention, guaranteed clear-path detection, final watch size or battery runtime is established.

## Selected architecture and cost constraint

Firmware: C / ESP-IDF / FreeRTOS, I2C, UART and planned cellular AT integration. Frontend: React / TypeScript / Vite. Backend: Node.js / TypeScript / Express, PostgreSQL and SSE. Deploy on the existing Kubernetes VM; cluster access, TLS, persistence and backups remain to be configured.

Remote alert route: watch Wi-Fi/LTE data -> authenticated backend -> httpSMS -> dedicated Android gateway with internet and active SMS-capable SIM -> caregiver. This gateway can remain with the team. Direct modem SMS is a later optional fallback, not the initial implementation. Local obstacle feedback is intended to work without network access, but is not integrated yet.

Use open-source software and available infrastructure. Recheck httpSMS free allowance and carrier terms at setup; no current pricing/stock/quotas were revalidated in this documentation review. Hardware, fabrication, infrastructure and SIM/data/SMS can still cost money. Keep credentials server-side and out of Git.

## Immediate next action

Keep the current diagnostic code and calibration unchanged. Capture five valid raw readings at each known target distance: 10 cm, 20 cm and 50 cm. Use one fixed sensor, a large flat white target square to its windows, documented lighting and a clear field of view. Record invalid statuses and any I2C_DIAG error. The actual target distance for the later 6.7–7.5 cm excerpt was never confirmed.

After reviewing this controlled set, investigate setup/optical interference, communication or calibration as the evidence warrants. Do not add arbitrary offsets or scaling. Only then validate at multiple distances and integrate the driver into Bindu. Preserve ST license and exact source version when importing the user's diagnostic.

## Navigation

- [Phase index](../Phases/README.md)
- [Phase 0 setup evidence](../Phases/Phase%200/context.md)
- [Phase 1 diagnostic details](../Phases/Phase%201/context.md)
- [Build tracker](build-tracker.md)
- [Parts and receipt status](parts-list.md)
- [Submission narrative](project-submission.html)
