# Bindu firmware — implementation and local setup

Reviewed 13 November 2026. [Current status](../docs/current-status.md).

This directory is the ESP-IDF/FreeRTOS scaffold that the user built and booted as revision 9c8b2c4 on 27 October. The successful boot used ESP-IDF v5.5.6, XIAO ESP32-S3 Plus, 16 MB flash and 8 MB octal PSRAM. Later repository edits are documentation/comments only; this review did not compile or flash new hardware firmware.

## What exists

Event dispatch, task/core priorities, power-mode state and component task loops exist. Sensor input functions and network/SMS/audio actions are stubs. Fall thresholds are experimental placeholders. Logged SMS messages do not send real SMS. OTA slot configuration and an app-valid call exist, but OTA update/rollback was not end-to-end tested.

The separate local D:\Bindu\tof-check program uses STSW-IMG009 ULD v3.5.5 to read VL53L1X on SDA GPIO5 and SCL GPIO6. It is not integrated here. Its identity/ranging checks passed, accuracy did not, and intermittent I2C reads failed. See [Phase 1](../Phases/Phase%201/context.md) before importing it. Preserve the original ST license and exact source version. Do not copy the STM32 platform implementation into ESP-IDF.

## Build on the user's local computer

Activate the installed IDF PowerShell environment, then:

```powershell
Set-Location D:\Bindu\HealthBuddy\firmware
idf.py build
# Only after a successful build, with the intended board/port confirmed:
idf.py -p COM17 flash monitor
```

COM17 was the last verified local port; check it after reconnecting. The agent's E:\ClaudeDesktop\HealthBuddy checkout is on a separate remote computer and cannot directly see that USB device. The tof-check diagnostic uses a factory partition layout; flashing this main firmware replaces it with Bindu's OTA layout. A normal sensor test does not require erase-flash or eFuse changes.

## Integration boundaries

- Acquire/validate real sensor data before publishing events. Preserve raw distance, range validity, timestamps and bus faults; invalid/missing data must never imply a clear path.
- Allocate pins for buttons, I2C, UART, feedback and any contact sensing together. GPIO43/44 appear as console UART pins in prior boot logs; resolve console/UART assignment before attaching a modem.
- Initial remote route: LTE/Wi-Fi HTTPS event -> backend -> httpSMS Android gateway. Direct modem SMS and calls are deferred. No credentials belong in firmware source.
- Local obstacle feedback should remain independent of internet once implemented. No camera integration is planned.
