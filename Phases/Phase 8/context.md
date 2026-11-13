# Phase 8 — Final phase — Wi-Fi CSI room-motion experiment

Planned: 13 November 2026 (Asia/Calcutta). Status: pending; no completion evidence recorded.

User-selected extension: detect movement in a controlled indoor area and notify the wearer through automatic vibration, with a physical-button check for recent movement. This is planned experimental work, not implemented or validated.

## Intended use and limits

A person moving near a monitored doorway may change the radio signal sufficiently to trigger an alert. The initial claim is "movement detected in the monitored area." CSI alone does not confirm that another person entered, identify them, determine direction or distance, or establish collision risk. The wearer, other occupants, doors and other environmental changes may also trigger it. This feature does not provide a 3D map or replace local obstacle assistance.

## Selected setup

Existing compatible Wi-Fi router -> additional stationary CSI-capable ESP32 -> local wireless event -> Bindu -> DRV2605L/motor vibration.

Keep the sensing ESP32 fixed; do not use the moving watch as the initial CSI detector. Confirm router compatibility, exact ESP32/ESP-IDF support, USB power and placement before purchasing. The extra board is not confirmed ordered or received. Use a laptop/USB connection for calibration and logs.

Proposed transport for implementation review: authenticated local Wi-Fi messaging between the room node and watch on the same LAN, with unique event IDs, a node/session identity, event age and a heartbeat. Internet, the cloud backend and caregiver SMS are not required for this local experiment. Watch Wi-Fi must be enabled for this mode; the separate LTE-only milestone remains tested with Wi-Fi disabled.

## Required interactions

| Interaction/state | Required behaviour |
| --- | --- |
| Automatic | When monitoring is enabled and a new confirmed motion event arrives, play a distinct movement vibration; filter brief disturbances and rate-limit repeats. |
| Button check: recent event | Report that movement was detected within the configured recent window, using the movement pattern. |
| Button check: no recent event | Give a separate positive acknowledgement meaning "no recent movement detected"; never imply nobody is present or the path is clear. |
| Offline, stale, calibrating or fault | Give a distinct unavailable acknowledgement; never reuse the no-recent-event response. |
| Reconnection/reboot | Refresh node/session state; reject stale/replayed events and avoid replaying old automatic vibrations. |

Use the existing non-SOS cancel/test control through an explicitly defined mode/action. Review conflicts with fall cancellation and scanning before assigning a short/long press. SOS remains reserved and higher priority. Do not add a caregiver push-notification requirement.

## Proposed prototype settings

These are engineering starting points, not validated limits: three short pulses for movement; a configurable 30-second recent-event window and 10-second automatic-alert cooldown; 5-second heartbeats and unavailable status after 15 seconds without a heartbeat. Final timings and all acknowledgement patterns must be tuned and documented after user distinguishability tests. Differentiate movement, no-recent-event and unavailable responses from obstacle, fall and SOS patterns.

Keep motion-event recency separate from node health. A recent event in the cache does not prove the node is currently online. Use event age/session checks so delayed packets cannot become fresh motion. On button check, prefer a fresh node response; if it cannot be obtained, report unavailable.

## Implementation work

- [ ] Confirm prerequisites: reliable motor/buttons, SOS priority, local obstacle alerts and the completed core wearable milestones.
- [ ] Confirm or source a compatible additional ESP32, router and power/data cables.
- [ ] Run Espressif's CSI acquisition/motion example with serial logs before adding watch messages.
- [ ] Record placement and calibrate in the target room; choose motion-confirmation criteria from data.
- [ ] Add paired/authenticated local messaging, heartbeats, event-age checks and duplicate rejection.
- [ ] Integrate automatic vibration and the button-query states above.
- [ ] Add monitoring enable/disable feedback and document the physical control mapping.
- [ ] Validate all haptic patterns, priorities, recency/cooldown values and unavailable behaviour.
- [ ] Measure additional watch/node power consumption and repeat radio/obstacle concurrency checks.
- [ ] Save evidence, limitations and a short controlled demonstration.

## Completion evidence

Test an empty room; a person entering/exiting; the wearer moving alone; door movement; multiple people; and motion away from the doorway. Record trial counts, missed detections, false alerts and detection-to-vibration latency rather than claiming an unmeasured accuracy.

Verify button checks within and beyond the recent window; no-motion acknowledgements; node/router loss; sensor faults/calibration; watch/node restarts; delayed/duplicate packets; reconnection; and cooldown behaviour. Confirm SOS and obstacle warnings remain responsive and distinct during CSI traffic. Demonstrate local operation without internet while the LAN remains available.

Entry direction, person identification, full-room coverage, outdoor navigation and guaranteed obstacle avoidance are outside this experiment.

## Reference

[Espressif ESP-CSI](https://github.com/espressif/esp-csi) documents router-based CSI acquisition and motion/presence demonstrations. Recheck compatibility and APIs when implementation starts.
