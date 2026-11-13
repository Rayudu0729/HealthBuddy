# Bindu — Experimental analysis (deferred)

Reviewed 13 November 2026. [Current project status](../docs/current-status.md).

No trained model, validated score or motion dataset is present. MPU6050 is reported received, but real motion acquisition has not been demonstrated. Vitals and voice hardware are later extensions.

Priorities: first acquire timestamped real IMU readings, validate units/placement, and evaluate simple fall rules with false-alarm counts and a cancellation window. Use controlled motion recordings rather than asking people to fall. Do not present an experimental threshold as a medically validated detector.

Walking stability, posture-change detection, routine anomalies and fall-direction classification are future experiments. Wrist placement must be validated; results from chest/belt devices do not automatically transfer. Medicine/sleep/vitals correlations must not be labelled causal explanations or used as clinical advice. Stand-up guard logic depends on sensors and evidence not currently available.
