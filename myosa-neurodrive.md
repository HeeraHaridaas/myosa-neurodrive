---
publishDate: 2026-08-25T00:00:00Z
title: NeuroDrive – Camera-Free Driver Vigilance Monitoring Using Multi-Sensor Fusion on MYOSA
excerpt: NeuroDrive estimates driver vigilance in real time from steering behaviour and environmental context — no cameras, no wearables — using MPU6050, APDS9960, and BMP180 sensors fused on the MYOSA (ESP32) platform.
image: neurodrive-cover.jpg
tags:
IoT
Embedded Systems
Driver Safety
Sensor Fusion
MYOSA
---
> Your steering wheel already knows when you're losing focus — NeuroDrive just listens.
---
Acknowledgements
We're grateful to the IEEE Sensors Council and the MYOSA/MakeSense Edutech team for organizing IEEE International MYOSA Event 6.0 and for building a platform that made rapid, low-cost sensor fusion prototyping possible. Special thanks to our faculty mentor, Dr. Nelwin Raj N R, Assistant Professor at Sree Chitra Thirunal College of Engineering, Thiruvananthapuram, for his guidance throughout the design and testing of NeuroDrive.
---
Overview
Road accidents caused by reduced driver vigilance remain one of the most persistent transportation safety problems worldwide, especially on long-distance and night-time drives. Most existing driver-monitoring solutions rely on in-cabin cameras or wearable devices — approaches that raise privacy concerns, add cost, and are hard to retrofit into ordinary vehicles.
NeuroDrive takes a different approach. Instead of watching the driver directly, it infers vigilance from how the driver interacts with the vehicle itself — primarily steering behaviour — combined with ambient environmental context. Everything runs locally on a single MYOSA (ESP32) board wired to three sensors, with no image capture and no wearable hardware required.
It's built for anyone who drives long distances regularly — private vehicle owners, taxi and logistics fleets, or long-haul operators — where a cheap, privacy-preserving add-on could catch a vigilance dip before it becomes a hazard.
At a high level: the system calibrates itself to your normal steering pattern, continuously scores your current steering behaviour against that baseline along with temperature and trip duration, and raises escalating buzzer and dashboard alerts as the computed Driver Vigilance Score (DVS) rises.
Key features:
Real-time Driver Vigilance Score computed from live steering-motion, environmental, and journey-duration data
Self-calibrating baseline — a 20-second on-device calibration step learns your normal steering variance before monitoring starts
OLED dashboard showing temperature, altitude, and live vigilance percentage with a status bar
Escalating active-buzzer alerts (short chirp → medium beep → long alarm) tied to risk severity
Blynk cloud dashboard for remote and family monitoring of vigilance status, temperature, altitude, and acceleration
---
Demo / Examples
Images
<p align="center">
  <img src="/assets/images/myosa-neurodrive/hardware-setup.jpg" width="800"><br/>
  <i>NeuroDrive hardware assembled on the MYOSA ESP32 motherboard with MPU6050, APDS9960, BMP180, OLED display, and buzzer</i>
</p>
<p align="center">
  <img src="/assets/images/myosa-neurodrive/oled-live-status.jpg" width="800"><br/>
  <i>OLED display showing live temperature, altitude, and Driver Vigilance Score with status bar</i>
</p>
<p align="center">
  <img src="/assets/images/myosa-neurodrive/blynk-dashboard.jpg" width="800"><br/>
  <i>Blynk cloud dashboard showing real-time vigilance status and sensor telemetry</i>
</p>
Videos
<video controls width="100%">
  <source src="/neurodrive-demo.mp4" type="video/mp4">
</video>
---
Features (Detailed)
1. Steering Interaction Sensing (MPU6050)
The MPU6050, accessed through MYOSA's `Ag` sensor object, provides raw gyroscope (X/Y) and accelerometer readings at high frequency. NeuroDrive computes a steering variance metric as the sum of absolute gyro X and Y values. Rather than using a fixed threshold for every driver, the system runs a 20-second calibration phase on startup, during which the driver is asked to hold the wheel steady. The average steering variance recorded during this window becomes each driver's personal baseline, with a minimum floor to avoid over-sensitivity. During live monitoring, current steering variance is compared against multiples of this baseline: unusually low variance (signaling a lapse in active steering correction) and unusually high, erratic variance (signaling overcorrection or agitation) are both scored as reduced vigilance, while variance in the normal band scores as attentive.
2. Environmental Context Sensing (BMP180 & APDS9960)
The BMP180, accessed via the `Pr` object, supplies temperature, barometric pressure, and altitude (calculated against a standard sea-level baseline of 1013.25 hPa). Cabin temperature is mapped into a contextual risk index, since elevated in-cabin temperature is a known contributor to drowsiness. The APDS9960 (`Lpg` object) measures ambient light and is streamed live to the cloud dashboard as an engagement/context signal for the driver and any remote observer.
3. Driver Vigilance Score (DVS) Engine
The core of NeuroDrive is a lightweight on-device scoring engine that fuses steering behaviour (S), environmental context (C), and journey duration (J) into a single 0–100 score, recalculated every second:
```
DVS = 0.65 × S + 0.20 × C + 0.15 × J
```
Journey duration factor (J) rises gradually over a 2-hour reference window, reflecting the well-documented increase in fatigue risk on longer drives. The current firmware implementation weights steering and environmental context most heavily since they update fastest and are the most direct vigilance signals; the ambient-engagement index (APDS9960 data) is logged and displayed on the dashboard today, with full weighted integration into the DVS formula planned as the next iteration — bringing the live engine in line with our full four-factor model from the original proposal.
4. OLED Real-Time Display
A custom UI renders directly to the MYOSA's OLED: a header banner, live temperature and altitude, the numeric Vigilance Score, and a horizontal progress bar that fills as risk increases. When the score crosses warning thresholds, the bottom of the screen inverts to a bold black-on-white banner reading WARNING, CRITICAL ALERT, or !! DANGER LEVEL !!, giving the driver an unmistakable visual cue at a glance.
5. Escalating Buzzer Alert System
Rather than the haptic vibration motor originally proposed, our built prototype uses an active buzzer for physical feedback — a simpler, more robust component for a hackathon timeline while still delivering an effective, unmissable alert. The buzzer duration escalates with severity: a short 100ms chirp at the "Warning" stage (DVS ≥ 60), a 250ms beep at "Critical" (DVS ≥ 70), and a full 500ms alarm tone at "Danger" (DVS ≥ 85) — so the driver can distinguish severity by sound alone without looking at the display.
6. Blynk Cloud Dashboard
NeuroDrive connects over WiFi to a Blynk IoT dashboard, pushing the Driver Vigilance Score, current status label, temperature, altitude, pressure, acceleration magnitude, and ambient light to virtual pins in real time. This enables remote monitoring — for example, a family member or fleet dispatcher can watch a driver's vigilance trend live, laying the groundwork for the GPS tracking, ETA estimation, and family-notification features outlined in our full proposal.
7. Auto-Calibration Sequence
On every power-up, NeuroDrive walks the driver through a guided 20-second calibration with a live on-screen countdown and instructions ("Calibrating Sensor — Hold Wheel Steady"), ensuring the vigilance baseline adapts to each driver and vehicle rather than relying on a one-size-fits-all threshold.
---
Usage Instructions
Flash the firmware to your MYOSA (ESP32) board using the Arduino IDE.
Update the WiFi credentials and your own Blynk template ID / auth token at the top of the sketch.
Power on the board. The OLED will show a "Connecting to WiFi..." screen, then begin the 20-second calibration countdown.
Hold the steering wheel steady during calibration — this sets your personal baseline steering variance.
Once calibration completes, live monitoring begins automatically and the Blynk dashboard starts receiving telemetry.
```plaintext
Board: ESP32 Dev Module
Upload Speed: 115200
Partition Scheme: Default
```
```cpp
// Core scoring loop (simplified)
float DVS = (0.65 * S) + (0.20 * C) + (0.15 * J);
DVS = constrain(DVS, 0, 100);
```
---
Tech Stack
Hardware: MYOSA Motherboard (ESP32), MPU6050 (accelerometer/gyroscope), APDS9960 (gesture/proximity/ambient light), BMP180 (barometric pressure & temperature), OLED display, active buzzer
Firmware: C++ (Arduino framework)
Connectivity/Cloud: WiFi, Blynk IoT platform
Tools: Arduino IDE, Blynk mobile app
---
Requirements / Installation
```bash
# Arduino IDE library requirements
Blynk (BlynkSimpleEsp32)
WiFi (built-in ESP32 core)
myosa (MYOSA sensor library)
```
Install the ESP32 board package in Arduino IDE, add the libraries above via the Library Manager, then set your own `BLYNK_AUTH_TOKEN`, WiFi `ssid`, and `pass` before flashing.
---
File Structure (Optional)
```
/myosa-neurodrive
  ├─ LICENSE
  ├─ README.md
  └─ neurodrive-main.ino
```
---
License (Optional)
MIT License
---
Contribution Notes (Optional)
Future work includes integrating the ambient-engagement index (APDS9960) into the DVS formula per our original four-factor model, adding GPS-based ETA and family-notification alerts to the Blynk dashboard, and exploring on-device machine learning for improved vigilance classification across drivers.
