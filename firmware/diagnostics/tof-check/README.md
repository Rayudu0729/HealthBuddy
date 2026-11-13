# VL53L1X distance diagnostic

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
