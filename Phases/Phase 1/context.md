# Phase 1 — Sensor bring-up and distance accuracy investigation

Last reviewed: 14 November 2026 (Asia/Calcutta). Latest evidence is direct COM17 testing in D:\Bindu\HealthBuddy. Read the new handoff below before the historical notes.

## Resume here — 14 November 2026

- Repository main was pulled from 9c8b2c4 to 0fa8f47. Current computer uses COM17,
  ESP-IDF D:\Bindu\.espressif\v5.5.6\esp-idf and Python environment
  C:\Users\Rayudu\.espressif\python_env\idf5.5_py3.12_env. Activate the IDF
  export.ps1 from that installation. The diagnostic builds in
  firmware/diagnostics/tof-check/build. Recheck USB/port on resume.
- Active flashed application is the repository ranging main.c plus ST ULD 3.5.5,
  now SHORT distance mode (1), 50 ms budget, 100 ms period and 50 kHz I2C.
  USB console enabled; 16 MB flash. Builds and all written flash-region hashes
  verified. Main Bindu firmware remains a scaffold. No monitor is left open.
- Current wiring: orange VCC -> 3V3, yellow GND -> GND, red SDA -> D4/GPIO5,
  brown SCL -> D5/GPIO6. INT/SHUT unconnected. User discussed 20 cm jumpers;
  actual final wire length is not independently verified. Do not use D2/D3:
  that alternate-pin test was unsuccessful and original mapping was restored.
- User measured sensor-side VCC 3.27 V, SDA/SCL 3.26 V, XSHUT 3.24 V, reported
  continuity beeps, and measured both SDA-to-VCC and SCL-to-VCC at 4.65 kohm
  on the 20 kohm range with power disconnected. This is consistent with pull-ups
  being present; do not add more without checking the breakout circuit.
- Repeated first-ID reads initially failed ESP_ERR_INVALID_STATE even after a
  fresh verified flash. Communication-only scanning found zero devices despite
  high digital idle inputs. MPU6050 substitution repeatedly acknowledged 0x68
  on D4/D5: this is an address test only, not an IMU functional test.
- Alternate D2/GPIO3 SDA and D3/GPIO4 SCL also found no devices and failed ID
  reads at 50 kHz. ESP-IDF 5.5.6 i2c_master_probe internally uses 100 kHz;
  changing the device clock does not slow the probe. The diagnostic therefore
  also attempted the actual ID transaction at 50 kHz regardless of probe result.
- After original D4/D5 wiring was restored, repeated 0x29 ACKs and ID 0xEACC
  returned. Cause of recovery is not isolated; rewiring, reset and read speed
  changed across attempts. Do not claim flash corruption or a dead sensor.
- Ranging at 50 kHz initially still failed: one run stopped after 26 samples at
  interrupt-clear register 0x0086, and another after 550 samples at data-ready
  reads 0x0030/0x0031, both with ESP_ERR_INVALID_STATE. Stop writes could fail too.
- Later long-mode stationary 20 cm test lasted 314.14 seconds with 3,196 valid
  readings, zero invalid readings and zero I2C errors: mean 19.664 cm, range
  17.4–20.8 cm. This passes that five-minute communication check, not a guarantee
  that intermittent faults are eliminated.
- Same-day long-mode results: 10 cm mean 8.852 cm (328 samples, 8.4–9.3 cm);
  repeat assumed still 10 cm mean 8.988 cm (366 samples). 20 cm mean 19.729 cm
  (300), later repeat 19.116 cm (350). Improved-setup 50 cm mean 45.670 cm
  (400, range 44.1–46.7 cm). All these excerpts had zero invalid/I2C messages.
  Other captures varied widely and must not be combined into a calibration fit.
- A requested 20-minute test was corrected by the user to TWO minutes. It ran
  133.32 seconds, 1,357 samples, mean 5.600 cm, range 5.0–6.3 cm, no I2C/invalid
  messages despite the stated 20 cm target. Subsequent photos raised alignment
  questions; the exact explanation for the 5.6 cm return is not established.
- Photos show a small card supported upright and the sensor propped near a tape
  roll, later raised on a multimeter. Overhead views were initially misread by
  the assistant as a flat card; a side view confirmed the card was upright.
  Do not repeat that claim. Exact optical alignment and window-to-face distance
  cannot be independently established from these photos. User confirms 20 cm.
- After raising the sensor, 150 readings averaged 23.03 cm (22.4–23.7 cm); after
  further alignment, 400 averaged 18.296 cm (16.9–19.0 cm). No I2C/invalid errors.
  These setup-dependent shifts require care before attributing error to hardware.
- Added TOF_QUALITY logs: status, raw_mm, ambient, signal_per_spad and spads from
  the same existing VL53L1X_GetResult block, without extra reads or calibration
  writes. These are raw ULD fields, not a calibrated target reflectance estimate.
  Long-mode quality capture: 270 samples mean 17.786 cm, range 16.6–18.9 cm,
  no I2C errors. Do not infer a particular optical fault solely from these fields.
- Latest SHORT-mode comparison at user-confirmed 20 cm: 451 valid samples,
  mean 17.994 cm, range 17.6–18.4 cm, zero invalid/I2C errors. Short mode did not
  remove the roughly 2 cm short reading. It is the current flashed mode.
- Offset and xtalk read zero after initialization. No guessed correction,
  calibration or persistence was added. ST UM2510 offset procedure specifies a
  17% gray target at 140 mm; returned correction must be stored by the host and
  reapplied after initialization. User does NOT have a calibrated gray target.
  Ordinary printed gray paper is not a verified reflectance reference. Do not
  run a purported ST calibration on the current 20 cm white-card setup.
- All new capture logs/summaries are in data/samples/vl53l1x-2026-11-14-*.
  Official monitor startup resets the MCU; captures are monitor transcripts
  with terminal formatting removed. Calibration/accuracy and Phase 1 remain open.

### Agreed next direction

Keep the VL53L1X for now; damage is not established. Borrow an appropriate gray
calibration target from a college electronics/robotics lab if possible, follow
ST's documented offset procedure, and verify at multiple distances afterward.
Do not apply a blanket +2 cm offset: errors varied across distances and setups.
Other component work (MPU6050 function, motor/driver, buzzer and SOS button)
can proceed separately, but no such functional tests were performed today.
Keep obstacle assistance experimental until distance performance and reliability
are validated. User requested this context update and push; no firmware operation
or serial monitor should remain running at handoff.

Reference: https://www.st.com/resource/en/user_manual/um2510-a-guide-to-using-the-vl53l1x-ultra-lite-driver-stmicroelectronics.pdf

## Historical status — superseded where noted above

## Current status

VL53L1X communication, ID 0xEACC and centimetre ranging are demonstrated in the repository diagnostic firmware/diagnostics/tof-check. Confirmed 10 cm and 20 cm targets read short. The latest 20 cm run stopped with ESP_ERR_INVALID_STATE. Current readings are not calibrated or reliable enough for obstacle alerts.

On 1 November the user reported that all components in the initial shopping list had arrived and were soldered. This supersedes the 27 October report that MPU6050 and DRV2605L were missing and headers were loose. This does not confirm receipt of later-stage LTE, battery, power or enclosure parts. Other peripherals have not yet been tested.

## Resume here — leaving-session handoff, 13 November 2026

User is going home and will continue in another session. No work is running and serial monitors are closed. Do not mark Phase 1 complete or assume the latest hardware fault has been fixed.

- Current computer: repo `E:\ClaudeDesktop\HealthBuddy`; board directly attached on COM3 during this session. Check the port and installed paths again on any other computer.
- ESP-IDF v5.5.6: `C:\esp\v5.5.6\esp-idf`; activate `. 'C:\Espressif\tools\Microsoft.v5.5.6.PowerShell_profile.ps1'`. CMake, Ninja, compiler and Python dependency checks passed.
- Current flashed program: repository `firmware/diagnostics/tof-check`, built/flashed from ignored `tmp/tof-check`. Main Bindu firmware remains a scaffold; do not replace the diagnostic with it for sensor testing.
- ST ULD v3.5.5 source/header files and licence are included in this diagnostic. The original ZIP is at `C:\Users\Rayudu.Rambabu\Downloads\STSW-IMG009.zip`; no download is required after cloning the committed diagnostic.
- Wiring: VCC→3V3, GND→GND, SDA→D4/GPIO5, SCL→D5/GPIO6; INT/SHUT unconnected. SDA/SCL use jumper wires. User reports VCC/SDA/SCL/SHUT each 3.27 V, end-to-end continuity on SDA/SCL/GND, and no sustained shorts between SDA/SCL or either signal and GND. Jumper length, external pull-ups, dynamic signals and contact quality are not verified.
- Confirmed 10 cm target: 157 valid samples, mean 7.966 cm, range 7.4–8.7 cm; no invalid/I2C messages in this excerpt.
- Confirmed 20 cm target: 21 valid samples, mean 17.067 cm, range 16.7–17.5 cm; then ESP_ERR_INVALID_STATE at reads 0x0030/0x0031 and the stop write 0x0087. Offset and xtalk read zero after init. No added correction.
- Latest instruction: unplug USB, replace SDA/SCL with another short pair (ideally 10 cm or less if available), firmly seat headers, preserve pin mapping and other wiring, then reconnect with the white card at 20 cm. **The user has NOT confirmed doing this yet.** Start next session by establishing whether that physical change happened.
- Next software action: capture the diagnostic at a confirmed target with the official ESP-IDF monitor. Plain pyserial opens repeatedly returned no data, whereas the official monitor captured startup; monitor normally resets the MCU. Record this reset in the evidence. On this computer, activate IDF, change to `E:\ClaudeDesktop\HealthBuddy\tmp\tof-check` (existing build), and run `idf.py -p COM3 monitor`; exit Ctrl+]. For a new checkout use the tracked diagnostic directory and build it first. Do not assume the old COM17 or D:\Bindu paths apply.
- Resolve bus reliability before calibration, threshold integration, buzzer/motor or other sensor work. Do not add an arbitrary +2–3 cm correction. The 10 cm/20 cm biases differ. Continue measured multi-distance validation after communication is stable.
- Evidence and summaries: `data/samples/vl53l1x-2026-11-13-target-10cm*` and `...target-20cm*`. The earlier unlabelled ranging excerpt has unknown target distance and is not an accuracy dataset.

## Historical environments and source locations — earlier computer


The following describes the earlier separate-computer setup. Direct USB access and the current paths are recorded in the resume section above.

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

## Direct-host test — 13 November 2026

The user confirmed that the XIAO is now directly attached to the agent's current computer. This supersedes the separate-host limitation above for this session. Current port is COM3. ESP-IDF v5.5.6 is installed at C:\esp\v5.5.6\esp-idf; tools and the verified Python environment are under C:\Espressif\tools. Activate with `. 'C:\Espressif\tools\Microsoft.v5.5.6.PowerShell_profile.ps1'`. Compiler, CMake, Ninja and Python dependency checks passed.

A new standalone communication-only diagnostic is saved at firmware/diagnostics/tof-check. It built and flashed successfully through COM3 (flash hashes verified). USB Serial/JTAG console is explicitly enabled. A ten-second capture showed five consecutive `No response at 0x29: ESP_ERR_NOT_FOUND` messages. This establishes that the board runs and logs, but the sensor did not acknowledge on SDA GPIO5 / SCL GPIO6 in this setup. It does not establish a defective sensor. Physical wiring/power, correct pins, continuity and sensor shutdown state need investigation before ranging. The diagnostic is currently flashed instead of Bindu or the earlier ranging app. No distance calibration or correction was applied.

Next: disconnect USB before changing wires; verify VCC→3V3, GND→GND, SDA→D4/GPIO5, SCL→D5/GPIO6 against printed labels. Keep other peripherals disconnected for the isolated test; INT and SHUT should be unconnected for the original single-sensor setup. Reconnect and repeat the ID check. Old ST ULD/ranging source has not been located on this computer.

### Recheck after user inspected wiring

On 13 November 2026 the user reported checking the pins and reconnecting USB. COM3 and both ESP32 USB interfaces remained present with Windows status OK. A fresh 12-second serial capture contained six consecutive `No response at 0x29: ESP_ERR_NOT_FOUND` messages. No valid sensor ID was received; communication remains unresolved. Capture: ignored local file tmp/tof-check/sensor-recheck.txt. The next physical checks should establish actual voltage at the sensor VCC/GND and end-to-end wiring continuity; do not infer sensor failure from missing acknowledgement alone.

### Communication restored after voltage checks and diagnostic reflash

The user reported VCC=3.27 V; SDA, SCL and SHUT each=3.27 V; continuity on SDA→D4, SCL→D5 and GND→GND; and no continuous beep between SDA/SCL or either signal and GND (unpowered short checks). These are user measurements, not independently measured by the agent.

The communication diagnostic was extended to scan 7-bit addresses 0x08–0x77 after a failed default-address probe. The incremental build and COM3 flash passed, with flash hashes verified. The following ten-second serial capture contained six successful sensor-ID messages, all 0xEACC. Because the 0x29 probe now succeeded, the fallback full-bus scan was not executed. Communication is restored in this excerpt; no cause is established, no distance samples were collected, and sustained reliability/accuracy remain unvalidated. Evidence is in ignored tmp/tof-check/full-scan-capture.txt. The next step is obtaining the original STSW-IMG009 v3.5.5 ULD sources on this computer and restoring the ranging test with validity/error reporting.

### ST ULD import and centimetre ranging — 13 November 2026

The user downloaded C:\Users\Rayudu.Rambabu\Downloads\STSW-IMG009.zip. Extracted package and API version macros confirm v3.5.5. ZIP SHA256: 5E8114C0F036CF8FAF30E19B3D864455B2459DB50DBF6CAFC7480E13A58A10A1. The four API/core/platform source/header files and API/LICENSE.txt are preserved under firmware/diagnostics/tof-check/main/vl53l1x. Platform functions are implemented for ESP-IDF in main.c, with 7-bit address 0x29 and ST argument 0x52. The original identity/scan program is preserved in communication_check.c (not compiled by default).

Ranging build and COM3 flash succeeded with verified flash hashes. Current flashed diagnostic uses long mode 2, 50 ms timing budget, 100 ms period; status 0 is valid, centimetres are raw mm / 10.0. Invalid readings are labelled separately; no arbitrary correction was added. Actual ESP-IDF bus errors are logged and stop the test; data-ready and boot waits are bounded. Offset/xtalk are read after initialisation, not manually changed. The captured excerpt did not include their startup values, so do not claim a newly observed value.

A roughly 15-second capture contains 147 valid readings, zero invalid readings and zero I2C_DIAG messages. Raw min/mean/max: 107 / 111.69 / 117 mm. Actual target distance has not been confirmed by the user; the earlier request to position a card at 20 cm is not confirmation that this happened. Accuracy and sustained reliability remain unvalidated. Raw serial evidence is saved at data/samples/vl53l1x-2026-11-13-ranging.txt; build and flash logs remain in ignored tmp/tof-check.

Next: obtain user confirmation of a fixed white-card target measured from the optical windows (start at 20 cm), capture a new labelled dataset, then repeat at 10 and 50 cm without changing code or applying guessed corrections.

### Corrected 10 cm target attempt — no distance samples

The user initially said the card was at 20 cm, then corrected it to 10 cm (`it 10`). Two raw serial captures were empty; the initial scratch summary still carried target_mm=200 but contained zero samples and must not be treated as a 20 cm dataset. A software reset via esptool run did not produce a useful raw capture. The official ESP-IDF monitor then reset the board and captured startup with `E (191) I2C_DIAG: Read register 0x010F: ESP_ERR_INVALID_STATE`, followed by `VL53L1X_GetSensorId(ST_DEVICE, &id) failed: ULD=255, I2C=ESP_ERR_INVALID_STATE; stopping` and return from app_main. Monitor was closed normally with Ctrl+]. No firmware was changed during this attempt. There are no fresh distance samples for a confirmed 10 cm target, and no new accuracy result. Communication is intermittent; a full sensor/board power cycle and startup recheck are needed before further target comparisons.

### Confirmed 10 cm target after USB power cycle — ranging restored

The user confirmed disconnecting USB for ten seconds, reconnecting, and keeping the white card at 10 cm. A raw serial capture remained silent. Starting the official ESP-IDF monitor reset the MCU and exposed a successful startup: sensor ID 0xEACC, settings after init offset=0 mm and xtalk=0, then ranging. Do not attribute the recovery solely to the power cycle, because the monitor also reset the MCU.

The captured monitor transcript contains 157 complete valid status-0 samples, zero invalid messages and zero I2C_DIAG messages. Raw minimum/mean/maximum: 74 / 79.6624203821656 / 87 mm for the user-confirmed 100 mm target. Mean error is -20.3375796178344 mm, about 2.03 cm short. These are short-run results only; accuracy has not passed. No calibration or arbitrary correction was applied. Monitor was closed normally with Ctrl+]. Evidence and summary are saved under data/samples/vl53l1x-2026-11-13-target-10cm*. Next obtain a controlled 20 cm target measurement without code changes to investigate whether the short reading is consistent across distances. The earlier unknown-target ~11.2 cm capture is not a valid comparison point.

### Confirmed 20 cm target — short readings and communication failure

The user confirmed the white card was 20 cm from the sensor. The raw serial capture again contained no data. Starting the official ESP-IDF monitor reset the MCU and provided startup plus 21 complete valid status-0 readings, with zero invalid-range messages. Settings after init were offset=0 mm and xtalk=0. Raw minimum/mean/maximum were 167 / 170.6666666666667 / 175 mm for the 200 mm target: mean error -29.3333333333333 mm. At uptime 2301 ms, reads of registers 0x0030 and 0x0031 failed with ESP_ERR_INVALID_STATE. The diagnostic stopped at valid=21, invalid=0; its stop-ranging write to register 0x0087 also failed with ESP_ERR_INVALID_STATE. Three I2C_DIAG messages total; this is not three failed measurement samples. Monitor was closed with Ctrl+]. Evidence and summary: data/samples/vl53l1x-2026-11-13-target-20cm*.

Comparison: confirmed 10 cm mean 79.6624 mm (error -20.3376 mm), confirmed 20 cm mean 170.6667 mm (error -29.3333 mm). A single constant correction is not validated. Communication is demonstrably intermittent and must be investigated before calibration or obstacle alert integration. Do not characterise short excerpts without errors as proving the bus fault resolved. Prefer the official monitor for future startup diagnosis because plain serial opens have repeatedly returned empty captures; document that its normal startup resets the MCU.

## 21 November 2026 breadboard placement

The user reports mounting the XIAO ESP32-S3 Plus across the center groove, with the silver shield up and USB-C facing left. Each seven-pin header starts at the leftmost hole and extends rightward, in D on the upper half and H on the lower half. The photo shows USB unplugged and no jumper wires attached. Electrical contact verification is pending. Resume with MPU-6050 wiring.

## 22 November 2026 bench verification

The XIAO ESP32-S3 Plus enumerated as COM17 on the Windows test PC. The MPU-6050 breakout returned WHO_AM_I 0x74 and streamed acceleration and angular velocity. A manual tilt changed both outputs. After adding the VL53L1X to the shared 3.3 V, ground, SDA (GPIO5), and SCL (GPIO6) connections, the MPU still streamed before the standalone ranging diagnostic was flashed.

The VL53L1X returned sensor ID 0xEACC. In short mode, the confirmed 10 cm target gave a 9.4 cm median. The confirmed 30 cm target gave a 24.5 cm median in two runs. In long mode, the median was 24.0 cm across 61 valid readings, assuming the target stayed at 30 cm. The checked-in diagnostic now uses long mode with a 50 ms timing budget and a 100 ms measurement period. The driver source shows that GetResult and GetDistance read the same distance register bytes, and the diagnostic converts millimetres to centimetres by dividing by ten.

After the user cleaned the optical windows, startup failed while reading the sensor ID register (0x010F, ESP_ERR_INVALID_STATE). On the next check the sensor ID read succeeded and all 61 samples were valid, with a 22.5 cm median and a 21.2–23.0 cm range. The target distance for this last capture was not reconfirmed. Intermittent I2C communication and ranging accuracy remain open. Compare a known-good VL53L1X breakout with the same setup and a fixed, measured target before applying calibration or relying on obstacle alerts.

## 22 November 2026 calibration attempt — on hold

Capture ran on the personal PC (`D:\Bindu\tof-flash`, COM17) because RDP COM redirection breaks when the board resets. The diagnostic adds an optional 8×8 ROI setting, an optional offset-calibration step and a linear correction; the ROI change did not remove the error.

Long-mode medians from 305 samples each: confirmed 10 cm target 67 mm raw, confirmed 30 cm target 204 mm raw. The error grows with distance (about 32 % short), so it is a scale error, not a constant offset. A two-point fit (gain 1.460, +2 mm) was flashed; a 20 cm check read 146 mm raw, 21.5 cm corrected (+1.5 cm). Readings drifted about 2 cm between sessions.

The checked-in `main.c` now holds a three-point least-squares fit (gain 1.449, −1 mm) from these three medians. It builds but has not been flashed or tested; the board still runs the two-point build. The correction is fitted only between 10 and 30 cm and is not valid for the 1.5 m obstacle range. Work is on hold to meet the 25 November deadline. Before relying on obstacle alerts, compare a second VL53L1X module and check 50, 100 and 150 cm targets.
