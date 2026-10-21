# Bindu — Hardware order status

Updated 21 October 2026. This list supersedes the earlier shopping alternatives and spare quantities. All ten items below are confirmed ordered by the user; receipt and successful testing remain unconfirmed. Quantities are those requested, pending invoice checks.

| Part | Requested quantity | Ordered | Received | Tested |
| --- | --- | --- | --- | --- |
| [XIAO ESP32-S3 Plus — HUBTRONICS](https://hubtronics.in/seeed-studio/seeed-studio-xiao-esp32-s3-plus) | 1 | [x] | [ ] | [ ] |
| [MPU6050 — Quartz](https://quartzcomponents.com/products/mpu6050-gyroscope-accelerometer-sensor) | 1 | [x] | [ ] | [ ] |
| [VL53L1X — Robokits](https://robokits.co.in/sensors/lidar-laser-rangefinders/vl53l1x-laser-time-of-flight-tof-4-meters-ranging-sensor-module-i2c-interface) | 1 | [x] | [ ] | [ ] |
| [Adafruit DRV2605L — DigiKey India](https://www.digikey.in/en/products/detail/adafruit-industries-llc/2305/5356831) | 1 | [x] | [ ] | [ ] |
| [Coin vibration motor — Robocraze](https://robocraze.com/products/coin-type-micro-vibration-motor) | 1 | [x] | [ ] | [ ] |
| [Active buzzer module — Robocraze](https://robocraze.com/products/pcb-mounted-active-buzzer-module) | 1 | [x] | [ ] | [ ] |
| [830-point breadboard — Robocraze](https://robocraze.com/products/mb102-830-points-solderless-breadboard) | 1 | [x] | [ ] | [ ] |
| [120-piece jumper set — Robocraze](https://robocraze.com/products/jumper-wire-set-m2m-m2f-f2f-40-pcs-each) | 1 set | [x] | [ ] | [ ] |
| [40-pin male header strip — Robocraze](https://robocraze.com/products/40x1-pin-2-54mm-pitch-male-berg-strip) | 2 strips | [x] | [ ] | [ ] |
| [6 × 6 mm pushbuttons — ThinkRobotics](https://thinkrobotics.com/products/6mm-tactile-push-button-pack-of-20) | 1 pack of 2, 5 mm height | [x] | [ ] | [ ] |

Prices paid, invoice quantities and delivery dates still need recording. These links identify the ordered products; current stock and pricing are not guaranteed.

## Pending hardware and equipment

- LTE modem development board, matching antenna/RF cable, activated data SIM and suitable regulated power supply.
- Dedicated Android phone running httpSMS, internet and SMS-capable SIM for caregiver delivery.
- Multimeter, soldering access, temporary wrist strap/mount and insulated contact materials, if not already available.
- Additional wrist-bend sensor if IMU experiments cannot reliably detect the intended gesture.
- Removal/contact or strap sensor after testing candidate methods.
- Protected LiPo, charging/power-path circuit, regulators and connectors after measuring consumption and reviewing fit.
- Final compact PCB, assembly, enclosure and strap after electrical and mechanical review.

Existing equipment: mobile charger labelled 5 V / 3 A and a cable. Confirm USB data capability. Verify each board's voltage requirements before connecting it; do not power a modem or vibration motor directly from a GPIO.

## Scope and build sequence

See [build-tracker.md](build-tracker.md) for component tests, local alerts, backend/dashboard, httpSMS, LTE, removal detection and final watch milestones.

The breakout boards are for bench testing and a temporary wrist mockup. Final size and runtime require measurements and a compact PCB/power/antenna layout. GNSS, vitals, voice and a display are later extensions, not confirmed orders. No final battery or generic charger/boost combination has been approved as the watch power design.
