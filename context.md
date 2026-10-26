# Bindu — Project context

Last updated: 27 October 2026 (Asia/Calcutta).

## Project and agreed scope

Bindu is a planned compact standalone 4G safety and accessibility wristwatch. Core features are physical SOS, experimental fall detection, gesture-activated obstacle distance warnings through vibration/optional sound, watch-removal notifications, a caregiver dashboard and caregiver SMS. Camera was considered and explicitly excluded. Keep the XIAO ESP32-S3 Plus as the selected controller.

Firmware uses C, ESP-IDF and FreeRTOS. Selected application stack: React/TypeScript/Vite frontend, Node.js/TypeScript/Express backend, PostgreSQL and SSE, deployed on the user's existing Kubernetes VM. SMS route: watch internet connection -> backend -> httpSMS -> dedicated Android gateway phone -> caregiver. The wearer does not need a nearby phone; remote delivery still depends on those services. Backend, dashboard, cellular and sensor integrations remain pending. GNSS, vitals and Telugu/Hindi voice are later extensions.

## Phase 0 — Development setup and first board boot

Status: software setup and initial board bring-up passed. Physical assembly and peripheral tests remain pending.

### Completed, with evidence

- [x] XIAO ESP32-S3 Plus received.
- [x] USB data connection verified on the user's local Windows computer.
- [x] Windows reports `USB Serial Device (COM17)` and `USB JTAG/serial debug unit`, both with status `OK` (Espressif VID 303A, PID 1001).
- [x] ESP-IDF v5.5.6 installed and environment activation completed successfully.
- [x] Official `hello_world` example built, flashed and printed `Hello world!` on the actual board.
- [x] Bindu repository cloned onto the local computer; firmware built successfully.
- [x] Bindu firmware revision `9c8b2c4` flashed and reached `Bindu ready`.
- [x] Boot log confirms 16 MB flash, 8 MB octal PSRAM and `SPI SRAM memory test OK`.

Evidence is the build and serial output supplied by the user in this chat on 27 October 2026. The remote agent did not independently access the physical board. Initial boot does not establish long-term stability, working sensor readings, fall accuracy, SMS delivery or battery runtime.

The `hello_world` example's countdown/restart is expected. Its initial 2 MB flash configuration caused a size warning; the Bindu boot log correctly reports 16 MB. `Returned from app_main()` after `Bindu ready` is expected: the firmware creates background FreeRTOS tasks.

### Critical environment distinction

The board is plugged into the user's **local computer**, not the remote desktop where this agent runs. Earlier remote USB/COM queries found nothing because they queried the other computer. Do not treat that as evidence of a faulty board or cable. Run physical-board commands locally unless USB/serial forwarding is explicitly configured and verified.

| Location | Verified path or setting |
| --- | --- |
| Agent/remote repository | `E:\ClaudeDesktop\HealthBuddy` |
| User/local repository | `D:\Bindu\HealthBuddy` |
| User/local firmware directory | `D:\Bindu\HealthBuddy\firmware` |
| User/local ESP-IDF | `D:\Bindu\.espressif\v5.5.6\esp-idf` |
| User/local IDF tools | `C:\Users\Rayudu\.espressif` |
| User/local IDF Python | `C:\Users\Rayudu\.espressif\python_env\idf5.5_py3.12_env\Scripts\python.exe` |
| Last verified board port | `COM17`; recheck after reconnecting if necessary |

The official Windows EIM GUI installer was used after WinGet could not find `Espressif.EIM`. Installed selections: target `esp32s3`, stable IDF `v5.5.6`, core features and required tools.

### Resume commands on the local computer

Use the installed IDF PowerShell shortcut. If its environment is not active, use the verified paths:

```powershell
$env:IDF_TOOLS_PATH = "C:\Users\Rayudu\.espressif"
$env:IDF_PATH = "D:\Bindu\.espressif\v5.5.6\esp-idf"
& "$env:IDF_PATH\export.ps1"
idf.py --version
Set-Location D:\Bindu\HealthBuddy\firmware
```

For a subsequent firmware build/upload, after checking the intended revision and port:

```powershell
idf.py build
# Run only after a successful build:
idf.py -p COM17 flash monitor
```

Exit the serial monitor with **Ctrl + ]**. The target has already been set to `esp32s3`; setting it again is unnecessary for ordinary rebuilds. Hardware remains disconnected from peripherals during this initial stage.

### Pending before wired component tests

- [ ] Solder the XIAO's currently loose side headers. User has no soldering iron/solder available yet; arrange suitable equipment or experienced help.
- [ ] Inspect solder joints and check for shorts before powering the board.
- [ ] Confirm GPIO allocation and module supply/logic levels before wiring.
- [ ] Test the physical SOS button, then VL53L1X separately.
- [ ] Receive and test MPU6050; motion/gesture/fall experiments must wait.
- [ ] Receive DRV2605L before vibration-motor integration. Do not drive the motor directly from a GPIO.

Disconnect USB before soldering or changing wiring. Loose unsoldered pins are not a reliable connection. Pin headers on the other modules have not been confirmed soldered.

## Hardware status correction

All ten items in the agreed initial shopping list were reported ordered. Eight are reported received: XIAO ESP32-S3 Plus, VL53L1X, coin motor, active buzzer, breadboard, jumper set, header strips and pushbuttons. **MPU6050 and Adafruit DRV2605L are not received.** The earlier statement that only the driver was missing was corrected by the user.

Only the controller has passed initial boot/memory checks. All peripheral tests remain pending. Actual invoice quantities, prices and exact delivery dates still need confirmation. LTE modem/accessories/SIM, final protected battery and power circuit, final removal sensing, PCB and enclosure are not confirmed ordered.

## Next session

Resume with arranging header soldering and checking the planned button connection. USB-only firmware work can continue while assembly is pending. Keep hardware readings and alert delivery labelled as unimplemented until demonstrated. Do not add camera hardware or replace the selected controller.

Detailed milestone checklist: [build tracker](docs/build-tracker.md). Hardware receipt table: [parts list](docs/parts-list.md). The submission PDF is an older presentation snapshot, not the current progress record.
