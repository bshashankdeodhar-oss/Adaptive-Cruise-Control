# Adaptive Cruise Control — Model Car

This repository contains an Arduino sketch and supporting documentation for a small-scale Adaptive Cruise Control (ACC) demonstrator implemented on a 4WD model car. The README below summarizes hardware, wiring, software, usage, calibration and known limitations so you can run, modify and extend the project.

---

## Contents

* `Adaptive_Cruise_Control.ino` — Arduino sketch (included)
* `Analysis of a Rudimentary Adaptive Cruise Control System in a Model Car.pdf` — Project report and background analysis

---

## Project summary

A simplified ACC implementation using an Arduino Uno, two HC-SR04 ultrasonic sensors (front and rear), a Hall-effect wheel sensor, a motor driver (H-bridge), and a 4WD chassis. The sketch uses a PID controller to compute a motor drive value from measured distance to the object ahead and adjusts motor PWM to maintain a safe following distance.

---

## Hardware (wiring + pin mapping)

Use the pin constants defined in the sketch as the reference. The wiring below maps the code pins to the components:

* **Left motor driver**

  * ENA (PWM): **D5**
  * IN1: **D8**
  * IN2: **D9**

* **Right motor driver**

  * ENB (PWM): **D6**
  * IN3: **D10**
  * IN4: **D7**

* **Ultrasonic sensors**

  * Front HC-SR04: TRIG = **D12**, ECHO = **D11**
  * Rear HC-SR04: TRIG = **D4**, ECHO = **D3**

* **Hall-effect wheel sensor**

  * Output: **D2** (interrupt pin)

* **Power**

  * Motor power should be provided from a separate battery / motor supply capable of the motor current. Connect motor supply ground to Arduino GND.
  * Arduino can be powered via USB or a regulated 5V supply.

> Important safety note: verify motor driver wiring and motor supply voltage/current ratings before applying power. Wrong wiring or insufficient decoupling may damage components.

---

## Software / Dependencies

* Arduino IDE (or PlatformIO)
* Library: **PID_v1** (Arduino PID Library) — the sketch includes `#include <PID_v1.h>`

To install the PID library: use Library Manager in Arduino IDE (Sketch → Include Library → Manage Libraries → search "PID" → install), or add it manually.

---

## Key runtime parameters (defaults from the sketch)

* `setDistance` = **30.0 cm** — target following distance
* PID gains: `Kp = 20`, `Ki = 0.5`, `Kd = 2`
* Minimum effective PWM clamp in code: motor PWM is clamped so values below ~60 are boosted to 60 when positive (to overcome motor deadband).

You can tune `setDistance` and the PID gains in the sketch and re-upload to the board.

---

## How it works (high-level)

1. The sketch reads front and rear distances using HC-SR04 sensors.
2. The front distance is used as the process variable for a PID controller which outputs a motor PWM value.
3. If an object is closer than `setDistance - 5` cm, the code checks the rear distance; if safe, it commands reverse to avoid collision; otherwise it stops.
4. If the front distance is greater than `setDistance + 5` cm, the car moves forward using computed PWM.
5. Hall-effect sensor pulses are counted via an interrupt to measure wheel rotations (available for logging/expansion in the sketch).

---

## Uploading the sketch

1. Open `Adaptive_Cruise_Control.ino` in the Arduino IDE.
2. Ensure the correct board (Arduino Uno) and port are selected.
3. Install the `PID_v1` library if not already installed.
4. Verify and Upload.

---

## Calibration & tuning tips

* Start with small Kp and zero Ki/Kd. Increase Kp until oscillation; add Ki to reduce steady-state error and Kd to reduce overshoot.
* Use a fixed obstacle and adjust `setDistance` to the desired following distance.
* Check motor deadband: some motors do not move until PWM above a threshold; the sketch already forces a minimum PWM (≈60) for positive motion — adjust if your motors require a different threshold.
* Verify ultrasonic sensor angles and mounting: stray reflections and angled surfaces affect accuracy.

---

## Known limitations (project analysis)

This is a rudimentary demonstrator. Limitations documented in the project report include sensor range/accuracy (HC-SR04 up to ~4 m but affected by surface and environment), single-sensor perception, limited controller performance due to Arduino Uno processing and simple motor control, and lack of steering/lane keeping. See the full analysis for more details.

---

## Suggested improvements (next steps)

* Add sensor fusion (radar/LiDAR/camera) or multiple ultrasonic sensors for better coverage.
* Implement a proper speed-feedback loop using the Hall-effect pulses and a velocity PID on top of distance control.
* Replace the microcontroller with ESP32 or similar for more processing headroom and telemetry (Wi‑Fi) capabilities.
* Add braking hardware (motor controller with regenerative braking or separate braking actuator) for smoother deceleration.

---

## Troubleshooting

* If sensors read `300 cm` or other large values frequently, ensure echo pins are connected and sensors are powered.
* If motors don't move at low PWM, increase minimum PWM clamp.
* For noisy distance readings, add moving-average filtering or a median filter before PID input.

---

## License & attribution

Use and modify this project for educational or research purposes. Credit the original analysis and code when appropriate.

---

## References

* Project analysis: *Analysis of a Rudimentary Adaptive Cruise Control System in a Model Car* (project report).
