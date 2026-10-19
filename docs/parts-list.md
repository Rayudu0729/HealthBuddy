# Parts list (bill of materials)

Prices are approximate (Indian online stores such as Robu.in and Amazon.in, Oct 2026). Check current prices before ordering. Delivery can take 1–2 weeks, so order the core set first.

Track progress by ticking **Ordered** and **Received** as you go.

## 1. Core prototype — order now

| # | Part | Qty | Approx. ₹ (total) | Purpose | Ordered | Received |
| --- | --- | --- | --- | --- | --- | --- |
| 1 | ESP32-S3 N16R8 dev board (16 MB flash, 8 MB PSRAM) | 2 | 1,000–1,600 | Main controller | [ ] | [ ] |
| 2 | MPU6050 accelerometer + gyro module | 2 | 250–400 | Falls, stand-up, walking stability | [ ] | [ ] |
| 3 | MAX30102 heart rate + SpO2 module | 2 | 300–500 | Vitals | [ ] | [ ] |
| 4 | INMP441 I2S microphone | 2 | 300–500 | Wake word, voice | [ ] | [ ] |
| 5 | MAX98357A I2S amplifier | 2 | 300–500 | Speaker driver | [ ] | [ ] |
| 6 | Speaker 8 Ω, 1–3 W (40 mm or smaller) | 2 | 100–200 | Voice replies | [ ] | [ ] |
| 7 | A7670C 4G LTE GSM module (with antenna) | 1 | 900–1,400 | SMS, calls, missed-call mode | [ ] | [ ] |
| 8 | NEO-6M GPS module (with antenna) | 1 | 300–450 | Location | [ ] | [ ] |
| 9 | LiPo battery 3.7 V, 1,000–1,500 mAh | 2 | 400–700 | Power | [ ] | [ ] |
| 10 | TP4056 charger module (with protection, USB-C) | 3 | 60–120 | Charging | [ ] | [ ] |
| 11 | 3.3 V LDO regulator (HT7333 or MCP1700) or MT3608 boost | 3 | 60–150 | Clean power from LiPo | [ ] | [ ] |
| 12 | Push buttons | 5 | 20–50 | SOS / push-to-talk | [ ] | [ ] |
| 13 | Active buzzer 3.3–5 V | 2 | 30–60 | Emergency beacon | [ ] | [ ] |
| 14 | Coin vibration motor | 2 | 60–120 | Quiet alerts | [ ] | [ ] |
| 15 | Nano SIM card (Airtel or Jio, 4G with SMS) | 1 | ~200 | For the GSM module | [ ] | [ ] |

## 2. Assembly and tools — skip what you already have

| Part | Qty | Approx. ₹ | Ordered | Received |
| --- | --- | --- | --- | --- |
| Breadboard (full size) | 2 | 150–250 | [ ] | [ ] |
| Jumper wires (M-M, M-F, F-F set) | 1 set | 150–250 | [ ] | [ ] |
| Male/female header pins | 1 pack | 50–100 | [ ] | [ ] |
| Perfboard / zero PCB | 3 | 60–120 | [ ] | [ ] |
| USB-C data cable | 2 | 200–300 | [ ] | [ ] |
| Soldering iron kit + solder wire | 1 | 400–800 | [ ] | [ ] |
| Multimeter | 1 | 300–600 | [ ] | [ ] |
| Resistors and capacitors kit | 1 | 150–300 | [ ] | [ ] |
| Pendant or belt-clip enclosure (or 3D print later) | 1 | 100–300 | [ ] | [ ] |

## 3. Optional / Level 2

| Part | Qty | Approx. ₹ | Purpose |
| --- | --- | --- | --- |
| MLX90614 temperature sensor | 1 | 600–900 | Body temperature, early illness warning |
| WS2812B LED strip, 30 cm | 1 | 150–250 | Night path light |
| Extra ESP32 board | 1 | 350–500 | Night light controller |

## Budget

- Core + tools: roughly ₹5,000–8,500 (includes spares and tools).
- Split across 4 members: about ₹1,250–2,100 each.
- One finished wearable (no spares, no tools): under ₹5,000.

## Notes

- **A7670C, not SIM800L.** SIM800L is 2G only; Jio has no 2G and other networks are phasing it out, so it may not connect.
- **Power:** a LiPo gives 3.7–4.2 V. Many dev boards cannot run reliably from it directly — Member 2 checks the board's power pins and uses item 11.
- **GSM peak current:** the A7670C draws short current spikes when sending; give it its own supply path and a large capacitor near the module.
- **Buy 2 of each sensor** so one burnt part does not stop the team.
