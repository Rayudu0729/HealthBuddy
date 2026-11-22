# Bindu phases

Reviewed 22 November 2026. [Current status](../docs/current-status.md) summarises the latest evidence.

| Phase | Status | Context |
| --- | --- | --- |
| Phase 0: tools and board boot | Initial setup/boot passed on 27 October; historical pending-arrival notes are superseded | [context.md](Phase%200/context.md) |
| Phase 1: sensor bring-up | VL53L1X ranging demonstrated; ~32 % scale error, calibration on hold (22 Nov) | [context.md](Phase%201/context.md) |
| Phase 2: local safety and obstacle assistance | In progress: Fall + SOS firmware builds; hardware run pending; obstacle scanning waits on Phase 1 | [context.md](Phase%202/context.md) |
| Phase 3: backend and caregiver dashboard | Implemented locally with passing tests; not deployed, no real watch upload | [context.md](Phase%203/context.md) |
| Phase 4: deployment and caregiver SMS | Planned: Kubernetes deployment and httpSMS gateway delivery | [context.md](Phase%204/context.md) |
| Phase 5: standalone LTE operation | Planned: caregiver delivery with watch Wi-Fi disabled | [context.md](Phase%205/context.md) |
| Phase 6: watch-removal detection | Planned: removal/restoration states and notifications | [context.md](Phase%206/context.md) |
| Phase 7: compact wearable integration | Planned: battery/power, PCB, enclosure and measured runtime | [context.md](Phase%207/context.md) |
| Phase 8: final Wi-Fi CSI experiment | Planned: stationary room sensing, automatic watch vibration and button check for recent movement | [context.md](Phase%208/context.md) |

No later phase is marked complete. Received components do not imply working integrations. On 13 November the board was directly accessible on COM3 in this checkout. See Phase 1 for current installation paths, captured errors and the pending jumper-wire change; recheck access on another computer.

## Sequence and scope

Finish the current Phase 1 investigation before integrating local safety behaviours. Phase 1 also covers the remaining individual component tests in build-tracker Step 2. Phase 0 records setup/boot evidence; outstanding inspection and pin-allocation work from Step 1 remains required. Phases 2–7 correspond to build-tracker Steps 3–8. Phase 8 is Step 9, the final experimental extension after the core wearable milestones.

Each planned phase lists its requirements and completion evidence. GNSS/location, optical vitals, Telugu/Hindi voice, ML trends and an optional display remain separately deferred extensions; their inclusion in earlier proposals is not proof of implementation. Review their hardware and design before scheduling them. The selected CSI experiment adds local watch feedback, not caregiver push notifications or guaranteed person-entry detection.
