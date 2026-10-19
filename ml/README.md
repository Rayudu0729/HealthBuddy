# ML (Lead)

## Level 1
- Walking-stability score from MPU6050 motion data (stride regularity, sway).
- Risk trend with causes: link drops in stability to medicine changes, poor sleep, high resting heart rate.
- Stand-up guard rules: stand-up detected + unstable heart rate / SpO2.

## Level 2
- Routine anomaly detection (waking time, movement, bathroom visits).
- Fall direction and impact classification.

Start with simple rules on recorded data in `data/`, then add ML once there is enough data.
