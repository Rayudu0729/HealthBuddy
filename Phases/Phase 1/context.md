# Phase 1 — Sensor bring-up and distance accuracy investigation

Last reviewed: 13 November 2026 (Asia/Calcutta). Latest hardware evidence remains 1 November 2026; no later fix or validated dataset has been reported.

## Current status

VL53L1X communication, sensor identification and distance output are demonstrated in a separate local diagnostic project. Distance accuracy has not passed validation. Intermittent I2C failures have also occurred; their underlying ESP-IDF error has not yet been captured in the shared logs.

On 1 November the user reported that all components in the initial shopping list had arrived and were soldered. This supersedes the 27 October report that MPU6050 and DRV2605L were missing and headers were loose. This does not confirm receipt of later-stage LTE, battery, power or enclosure parts. Other peripherals have not yet been tested.

## Environments and source locations

The physical board is on the user's local computer. The agent works on a separate remote computer and cannot directly flash or inspect the local board.

| Item | Local path or setting |
| --- | --- |
| ESP-IDF | `D:\Bindu\.espressif\v5.5.6\esp-idf` |
| Diagnostic project | `D:\Bindu\tof-check` |
| Diagnostic application | `D:\Bindu\tof-check\main\hello_world_main.c` |
| Copied ST core/header files | `D:\Bindu\tof-check\main\vl53l1x` |
| ST package | `D:\Bindu\STSW-IMG009\STSW-IMG009_v3.5.5` |
| Physical USB port last verified | `COM17` |
| Main Bindu project | `D:\Bindu\HealthBuddy\firmware` |

The diagnostic started as a copy of Espressif's hello_world example. Logs still naming the project `hello_world` do not mean the wrong application ran: the `TOF_CHECK` and `TOF_RANGE` messages demonstrate the changed application. The diagnostic uses a factory partition, unlike Bindu's OTA partition layout. It is currently flashed in place of the Bindu scaffold.

Diagnostic source and ST files were created/copied through commands run by the user locally. They are not yet imported into this remote Git repository. Obtain the current local sources and preserve ST's license before integrating them into Bindu; do not assume an untracked local change is already available on GitHub.

## Wiring and configuration used

| VL53L1X breakout | XIAO ESP32-S3 Plus |
| --- | --- |
| VCC | 3V3 |
| GND | GND |
| SDA | D4 / GPIO5 |
| SCL | D5 / GPIO6 |

INT and SHUT are unconnected for this single-sensor test. Other modules remain disconnected. The breakout's rear label reads `VL53LXX`; its specific identity was established by the ID register, not that generic label. Follow pin labels: the apparent left/right pin order reverses when viewing the other side.

- ESP-IDF v5.5.6; new `driver/i2c_master.h` interface, 100 kHz device clock.
- I2C internal pull-ups enabled; external pull-up population and actual line voltages have not been independently measured.
- ESP-IDF address: 7-bit `0x29`; ST driver device argument: 8-bit `0x52`.
- Official STSW-IMG009 ULD v3.5.5, using files from `API/core` and `API/platform`, not the STM32 `Example`.
- Platform functions are implemented in the diagnostic application's C file: byte/word/dword and multi-byte I2C reads/writes, with big-endian register/data conversion and FreeRTOS millisecond waits.
- Long distance mode (2), 50 ms timing budget, 100 ms intermeasurement period.
- Print only range-status 0 as valid distance; report other statuses separately.
- Centimetres are raw millimetres divided by 10.0; no distance correction has been applied.

## Evidence and unresolved findings

1. Repeated `TOF_CHECK: Device responds at 0x29` confirmed acknowledgement.
2. `TOF_CHECK: Sensor ID: 0xEACC` confirmed the expected VL53L1X ID.
3. ST ULD ranging produced changing distances in millimetres, then centimetres plus raw millimetres.
4. One reported range status was 7 (wraparound/phase consistency failure); software rejected it. Invalid readings must never be interpreted as a clear path.
5. Initial fixed-target samples at a user-reported 20 cm averaged about 16.79 cm (16 samples), while a 50 cm set averaged about 40.12 cm (21 samples). This was not a constant additive error.
6. Further reported 20 cm sets read about 13.3 cm, later 16.2–17.0 cm and later around 17.4 cm. The setup or optical condition changes between runs were not fully recorded. Do not combine these into a calibration fit.
7. The user reconfirmed 20 cm for the latest approximately 17.4 cm readings, implying about 2.6 cm short. This is a failed accuracy check, despite small variation within a run.
8. User also reported about 5 cm without the white target. That is not proof of a clear optical scene: nearby surfaces or optical interference were not excluded by evidence.
9. `Read sensor ID failed: -1` occurred on a startup attempt. A power cycle was suggested; subsequent ranging logs returned, but the root cause was not established.
10. Another run stopped with `Check data ready failed: -1` and `No measurement available; stopping`. This is a communication-call failure, not the same event as a rejected range-status reading. The adapter mapped all read failures to -1.

Only user-provided local logs constitute hardware evidence. No sensor accuracy, absence of wiring faults, long-run reliability or suitability for walking assistance is established yet.

## Diagnostics already supplied to the user

- The I2C read adapter now logs the underlying `esp_err_to_name()` result under `I2C_DIAG`, including the register address, before returning -1. No actual `I2C_DIAG` error line has been shared yet. Successful later excerpts do not establish that intermittent errors are resolved.
- Latest code addition calls `VL53L1X_GetOffset()` and `VL53L1X_GetXtalk()` and logs settings after ranging is started. It only reads settings; it does not modify calibration.
- Subsequent user-provided flash/monitor output confirmed successful flashing, ID `0xEACC`, initialisation, `Calibration settings: offset=0 mm, xtalk=0` and `Ranging started`. The excerpt contains distances of 6.7–7.5 cm, but the physical target distance for this particular run was not supplied. Do not label these samples as a 20 cm accuracy test without confirmation. No I2C error is present in this short excerpt; sustained reliability is not established.
- Zero reported offset/crosstalk means no such correction was applied at the time of the read. It does not independently prove faulty hardware or prove that an optical crosstalk problem is absent; the ULD initialisation writes default configuration.
- Local backups were created alongside the application with `.bak` suffixes, including the pre-centimetre, pre-I2C-diagnostic and pre-calibration-check versions. Verify their actual contents before restoring anything.
- Protective film, tape, fingerprints and optical crosstalk were proposed as hypotheses. The user has not confirmed whether a film was present or removed. Do not state that film removal fixed anything.

## Immediate next step

On the user's local ESP-IDF PowerShell, exit any existing monitor with Ctrl + ], then run:

```powershell
Set-Location D:\Bindu\tof-check
idf.py -p COM17 monitor
```

Flash and settings capture are now complete: `offset=0 mm, xtalk=0`. The target distance during the 6.7–7.5 cm run remains unconfirmed. The latest instruction is to keep code unchanged and collect five raw readings each at known 10, 20 and 50 cm distances, plus invalid statuses and I2C errors. That dataset has not been received. Capture any `I2C_DIAG` failure line if a read fails again. Recheck the COM port if COM17 is unavailable.

Do not repeat the code-insertion scripts blindly; they deliberately reject a source file already containing the insertion marker.

## Following steps and acceptance

1. Inspect the reported calibration settings without guessing their values or zeroing them automatically.
2. If communication fails again, use the actual ESP-IDF error, wiring continuity, supply and pull-up evidence to investigate it before calibration. Do not hide failures with repeated retries or stale readings.
3. Establish a reproducible optical setup: stationary sensor, large flat target perpendicular to the optical axis, distance measured from the windows, clear field of view, documented lighting and any cover/film.
4. Record distance and invalid-read counts at known positions such as 10, 20, 50 and 100 cm. Mark conditions and any changes between runs.
5. If necessary, apply ST's documented offset/crosstalk calibration with the required controlled setup. Do not add an arbitrary 2.6 cm correction or scale based on the inconsistent earlier sets.
6. Validate after any change at multiple distances, including startup and sustained operation. Preserve raw readings and validity status.
7. Only after those checks integrate distance data into Bindu and add buzzer/vibration obstacle feedback. Alert thresholds and acceptable error limits remain to be agreed and tested.

## References and handoff

- [Phase 0: setup and initial Bindu boot](../Phase%200/context.md)
- [Current status](../../docs/current-status.md)
- [Build tracker](../../docs/build-tracker.md)
- [Hardware orders](../../docs/parts-list.md)
- [ST ULD package](https://www.st.com/en/embedded-software/stsw-img009.html)
- [ST ULD manual](https://www.st.com/resource/en/user_manual/um2510-a-guide-to-using-the-vl53l1x-ultra-lite-driver-stmicroelectronics.pdf)
- [ST sensor datasheet](https://www.st.com/resource/en/datasheet/vl53l1x.pdf)
