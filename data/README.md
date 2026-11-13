# Bindu — Test evidence

Reviewed 13 November 2026. User-supplied serial excerpts are the current evidence; the agent cannot directly access the local board. Keep large original logs in ignored data/raw/ and small, de-identified samples in data/samples/. Record the actual date, physical setup, software version and target distance. Unknown values must remain unknown.

## Recorded milestones

| Evidence date | Test | Result | Limits |
| --- | --- | --- | --- |
| 27 October 2026 | XIAO build/flash/boot | Bindu ready; 16 MB flash; 8 MB PSRAM test passed | Boot only, not integrated features |
| 1 November 2026 | VL53L1X I2C/ID/ranging | 0x29 acknowledges; ID 0xEACC; distance/status output | Separate local tof-check diagnostic |
| 1 November 2026 | Known-distance excerpts | 20 cm averaged 16.79 cm; 50 cm averaged 40.12 cm | Accuracy failed; geometry/lighting not fully recorded |
| 1 November 2026 | Calibration-settings read | offset=0 mm, xtalk=0 | No correction applied; no calibration performed |

[Sample summary CSV](samples/vl53l1x-2026-11-01-summary.csv) contains aggregates of two explicitly labelled user excerpts. It is not a new test, a complete raw dataset or a calibration fit. Later 20 cm results differed between runs. No measurement is proof that a walking path is clear.

## Next collection

With unchanged code and one reproducible setup, collect five readings each at measured 10, 20 and 50 cm from the optical windows. Include invalid statuses, I2C_DIAG error details, target size/material, orientation, ambient light, distance reference, cover/film state and any wiring movement. Keep a separate failure count. Do not replace missing/invalid readings with the last valid distance.

Later test plans cover button debounce/cancellation, IMU false alarms, vibration recognition, removal/restoration, network/SMS failures and battery consumption. These results have not yet been collected. GNSS, vitals and wake-word results must not be invented or presented as live data.
