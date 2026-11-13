# VL53L1X distance diagnostic

Latest mode comparison: active main.c now selects short mode (1), keeping the
50 ms budget, 100 ms period, D4/D5 and 50 kHz I2C. Build and flash verified.
No manual offset/crosstalk correction added. The white target was reported at
20 cm; compare the saved short-mode log with the preceding quality-20cm log.
Short mode did not remove the approximately 2 cm short reading. Physical setup
has varied across the broader session, so no single correction is established.

The active ranging app now logs TOF_QUALITY fields (status, raw distance,
ambient, signal per SPAD, SPAD count) from the same ULD result block for every
measurement. No extra sensor reads or calibration writes are added. Field values
are labelled as ULD fields; do not interpret them as calibrated reflectance.
Build and flash hash verification passed. Initial 20 cm capture remains short
and includes valid status-0 readings. Raw evidence: the 14 November quality log.
ST UM2510 specifies a 17% gray target at 140 mm for offset calibration, followed
by host storage/reapplication of the returned value. Current white-card tests
are not that calibration setup; no new offset or crosstalk values were applied.

Latest active application (14 November): ranging `main.c` plus ST ULD, D4/GPIO5
SDA, D5/GPIO6 SCL, 50 kHz. The later ranging tests supersede the earlier
communication-only selection described below. Latest five-minute 20 cm capture:
3,196 valid samples over 314.14 seconds, mean 196.642 mm, range 174–208 mm,
zero invalid readings and zero I2C errors. Monitor closed. See current-status.md
and the saved five-minute raw log/summary for limitations and next steps.

## Active diagnostic — 14 November 2026

The current `main/CMakeLists.txt` selects `communication_check.c`, not the ranging
application. It reports digital idle levels on GPIO5/GPIO6, probes address 0x29,
scans addresses 0x08–0x77 if that probe fails, and reads the ID only after an ACK.
Digital high is not a voltage or signal-quality measurement. No calibration or
ranging is performed. To restore ranging, compile `main.c` and
`vl53l1x/VL53L1X_api.c` with the `vl53l1x` include directory.

On this computer (D:\Bindu, COM17), a fresh unmodified ranging build of revision
0fa8f47 flashed with all region hashes verified, but still failed its first ID
read with ESP_ERR_INVALID_STATE. The communication-only build also flashed with
verified hashes. Five consecutive cycles observed SDA=1/SCL=1, no ACK at 0x29
(ESP_ERR_NOT_FOUND), and zero devices/zero bus errors across each scan. The
official monitor reset the MCU at startup and was closed after the capture.
This does not establish sensor failure or adequate pull-ups; check sensor-side
power, ground, signal continuity, shutdown state and electrical signalling next.
No distance samples were obtained.

Standalone ESP-IDF v5.5.6 project for XIAO ESP32-S3 Plus, using STSW-IMG009 ULD v3.5.5. USB Serial/JTAG console, 16 MB flash, SDA GPIO5 (D4), SCL GPIO6 (D5), 100 kHz I2C. Sensor VCC goes to 3V3, GND to GND; leave INT/SHUT unconnected for the original single-sensor setup.

## Behaviour

The application verifies ID 0xEACC, waits for boot, initialises the ST driver, selects long mode with a 50 ms timing budget and 100 ms period, and reports the offset/crosstalk settings read after initialisation. Valid status-0 samples print centimetres (raw millimetres / 10.0) and raw millimetres. Other range statuses are labelled invalid; no arbitrary calibration correction is added. Bus errors include the ESP-IDF error and register address. Boot and data-ready waits are bounded; communication errors stop the diagnostic rather than reusing stale measurements.

The original address/ID-only diagnostic is preserved in main/communication_check.c, which is not compiled by default. To restore it, change main/CMakeLists.txt to compile communication_check.c instead of main.c and the ST API source.

## Build and monitor

Activate the installed ESP-IDF environment, change to this directory, and run `idf.py build`. With the intended board confirmed, run `idf.py -p COM3 flash monitor` (recheck the port first). Flashing replaces the currently running application and partition table with this diagnostic's factory layout. Exit the monitor with Ctrl+]. This is separate from Bindu's main firmware and OTA layout.

## Evidence — 13 November 2026

The initial communication-only diagnostic built/flashed, but probes reported ESP_ERR_NOT_FOUND. After user wiring/voltage checks and a reflash, six IDs returned 0xEACC. The cause of restored communication is not established.

The ranging diagnostic then built and flashed successfully, with flash hashes verified. A serial capture contained 147 valid samples, zero invalid samples and zero I2C error messages. The raw values ranged from 107 to 117 mm (mean 111.69 mm). Actual target distance was not confirmed, so this is not an accuracy validation. Previous 1 November accuracy discrepancies and intermittent faults remain relevant. Next collect controlled readings at known 10, 20 and 50 cm measured from the optical windows.

Build/flash logs are in ignored tmp/tof-check. The captured samples are under data/samples/vl53l1x-2026-11-13-ranging.txt.

## Vendor source

main/vl53l1x contains unmodified API/core VL53L1X_api.c and VL53L1X_api.h, API/platform vl53l1_platform.h and vl53l1_types.h, and API/LICENSE.txt from the user-downloaded STSW-IMG009.zip. ESP-IDF platform functions are implemented in main/main.c; the STM32 platform implementation is not copied. Preserve vendor notices and applicable licence terms.

Official source: https://www.st.com/en/embedded-software/stsw-img009.html
ZIP SHA256: 5E8114C0F036CF8FAF30E19B3D864455B2459DB50DBF6CAFC7480E13A58A10A1

## Later controlled targets and unresolved bus failure

On 13 November, a user-confirmed 10 cm target produced 157 valid readings with raw mean 79.6624 mm (range 74–87 mm). A confirmed 20 cm target produced 21 valid readings with raw mean 170.6667 mm (range 167–175 mm), then ESP_ERR_INVALID_STATE on reads at registers 0x0030/0x0031 and on the stop-ranging write at 0x0087. Both runs used the official ESP-IDF monitor, whose normal startup reset the MCU. Settings after init read offset=0 mm and xtalk=0. These results do not validate a constant correction; bus reliability and distance accuracy remain unresolved. Detailed transcripts/summaries are in data/samples and Phase 1 context.

### Alternate pins test — 14 November 2026

Active communication diagnostic now uses SDA D2/GPIO3 and SCL D3/GPIO4. ID reads use 50 kHz and are attempted even when the address probe fails. ESP-IDF 5.5.6 i2c_master_probe uses a fixed 100 kHz internally, so the scan is still at 100 kHz. Build and flash passed with all written hashes verified. Five consecutive initial cycles showed both idle inputs high, no ACK at 0x29, zero scanned devices/zero scan bus errors, and failed 50 kHz ID reads (ESP_ERR_INVALID_STATE). Monitor startup reset the MCU; monitor was closed. Earlier substitution with the MPU6050 on D4/D5 repeatedly acknowledged at 0x68 using the same scanner. User reported sensor VCC 3.27 V, SDA/SCL 3.26 V, XSHUT 3.24 V and SDA-to-VCC/SCL-to-VCC resistance 4.65 kohm with power disconnected. These findings increase suspicion of the VL53L1X breakout or its specific contacts; sensor damage is not confirmed. Ranging main.c still uses D4/D5 and is not the active application.


### Communication restored on D4/D5 — 14 November 2026

User supplied the original mapping again: orange VCC to 3V3, yellow GND to GND, red SDA to D4/GPIO5, brown SCL to D5/GPIO6. Diagnostic was rebuilt for these pins, retaining 50 kHz ID reads; flash hashes verified. Five initial consecutive cycles acknowledged 0x29 (100 kHz probe) and returned sensor ID 0xEACC (50 kHz read). Communication is restored in this capture. Cause is not isolated: wiring handling, reset and read speed changed across tests. No distance readings or sustained reliability/accuracy claim. Monitor closed; active firmware now uses D4/D5, superseding the active-pin statement above.


### Confirmed 20 cm ranging attempt — 14 November 2026

Active build restored to main.c plus ST ULD, using D4/GPIO5, D5/GPIO6 and 50 kHz I2C. Build and flash hashes passed. User confirmed white card at 20 cm. Monitor reset MCU and captured ID 0xEACC, offset=0 mm, xtalk=0, and 26 valid-status samples ranging from 31 to 355 mm (mean 196.69 mm). Large variation means the mean must not be treated as an accuracy pass; scene stability is unconfirmed. At uptime 3021 ms, interrupt-clear write 0x0086 failed with ESP_ERR_INVALID_STATE and ranging stopped; stop write 0x0087 also failed. Communication remains intermittent even at 50 kHz. Monitor closed. Evidence: data/samples/vl53l1x-2026-11-14-target-20cm*. No correction applied.
