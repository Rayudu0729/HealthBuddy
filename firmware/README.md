# Bindu firmware — implementation and local setup

Reviewed 13 November 2026. [Current status](../docs/current-status.md).

This directory is the ESP-IDF/FreeRTOS scaffold that the user built and booted as revision 9c8b2c4 on 27 October. The successful boot used ESP-IDF v5.5.6, XIAO ESP32-S3 Plus, 16 MB flash and 8 MB octal PSRAM. The main scaffold has not been newly validated. On 13 November the standalone diagnostic below was built, flashed and tested directly on COM3.

## What exists

Event dispatch, task/core priorities, power-mode state and component task loops exist. Sensor input functions and network/SMS/audio actions are stubs. Fall thresholds are experimental placeholders. Logged SMS messages do not send real SMS. OTA slot configuration and an app-valid call exist, but OTA update/rollback was not end-to-end tested.

The separate local D:\Bindu\tof-check program uses STSW-IMG009 ULD v3.5.5 to read VL53L1X on SDA GPIO5 and SCL GPIO6. It is not integrated into the main scaffold; the restored standalone version is now tracked under [diagnostics/tof-check](diagnostics/tof-check/README.md). Its identity/ranging checks passed, accuracy did not, and intermittent I2C reads failed. See [Phase 1](../Phases/Phase%201/context.md) before importing it. Preserve the original ST license and exact source version. Do not copy the STM32 platform implementation into ESP-IDF.

## Build on the user's local computer

Activate the installed IDF PowerShell environment, then:

```powershell
Set-Location D:\Bindu\HealthBuddy\firmware
idf.py build
# Only after a successful build, with the intended board/port confirmed:
idf.py -p COM17 flash monitor
```

COM17 was the earlier computer's port; COM3 was verified on 13 November; check it after reconnecting. On 13 November the board was directly connected to E:\ClaudeDesktop\HealthBuddy on COM3; recheck the port and environment on the current computer. The tof-check diagnostic uses a factory partition layout; flashing this main firmware replaces it with Bindu's OTA layout. A normal sensor test does not require erase-flash or eFuse changes.

## Fall + SOS demo wiring

| Part | Pin / address | Notes |
|---|---|---|
| MPU-6050 | I2C 0x68; VCC 3V3, GND, SDA GPIO5 (D4), SCL GPIO6 (D5) | Accelerometer set to ±8 g, ~44 Hz filter, 100 Hz. Clone WHO_AM_I 0x74 is accepted |
| DRV2605L | I2C 0x5A, same SDA/SCL | ERM coin motor on OUT+/OUT-; EN pin high. Optional: if missing, buzzer only |
| Active buzzer | D0 = GPIO1 (+), GND (-) | Active high (`BUZZER_ACTIVE_LEVEL` in `feedback.h`) |
| SOS button | D1 = GPIO2 to GND | Internal pull-up; hold 1.5 s |
| CANCEL button | D2 = GPIO3 to GND | Internal pull-up; short press |

I2C runs at 100 kHz with internal pull-ups enabled in addition to the breakout pull-ups.

## How to demo

1. Build and flash: `idf.py -B build-demo build`, then `idf.py -p COMx flash monitor`.
2. Wait for `Bindu ready` and the boot beep.
3. **SOS:** hold the SOS button 1.5 s. Expect the SOS_CONFIRMED pattern, then `ALERT SENT (stub): SOS`.
4. **Fall:** tap or drop the MPU hard (> 2.5 g), then leave it still for 2 s. Log shows `impact x.xx g`, `|a| = ...` each second, then `Fall alert in 10 s - press CANCEL` with a tick each second.
5. **Cancel:** press CANCEL within 10 s: `Fall alert CANCELLED by user - nothing sent`. Otherwise `ALERT SENT (stub): FALL x.x g`.

Sending is a stub (log line only, no network). A second fall during the countdown is ignored; CANCEL outside a countdown is ignored. SOS during a countdown overrides it.

## Integration boundaries

- Acquire/validate real sensor data before publishing events. Preserve raw distance, range validity, timestamps and bus faults; invalid/missing data must never imply a clear path.
- Allocate pins for buttons, I2C, UART, feedback and any contact sensing together. GPIO43/44 appear as console UART pins in prior boot logs; resolve console/UART assignment before attaching a modem.
- Initial remote route: LTE/Wi-Fi HTTPS event -> backend -> httpSMS Android gateway. Direct modem SMS and calls are deferred. No credentials belong in firmware source.
- Local obstacle feedback should remain independent of internet once implemented. No camera integration is planned.
