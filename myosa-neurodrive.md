---
publishDate: 2026-08-25T00:00:00Z
title: "NeuroDrive: Vehicle-Interaction-Based Driver Vigilance Estimation Using Multi-Sensor Fusion on the MYOSA Platform"
excerpt: "A privacy-preserving driver monitoring system that estimates vigilance from steering behaviour and environmental sensing, built entirely on the MYOSA (ESP32) platform with no cameras or wearables required."
image: neurodrive-cover.jpg
tags:
  - IoT
  - Embedded Systems
  - Driver Safety
  - Sensor Fusion
  - MYOSA
---

A privacy-preserving driver monitoring system built entirely on steering behaviour and environmental sensing, requiring no cameras, wearables, or internet-dependent infrastructure.

---

## Acknowledgements

We express our sincere gratitude to our Faculty Mentor, Dr. Nelwin Raj N R, Assistant Professor, Department of Electronics and Communication Engineering, Sree Chitra Thirunal College of Engineering, Thiruvananthapuram, for his continuous guidance and technical support throughout this project.

We thank the Department of Electronics and Communication Engineering and the management of Sree Chitra Thirunal College of Engineering for providing the resources and environment needed to build and test NeuroDrive.

We are grateful to the organizers of IEEE International MYOSA Event 6.0 and the IEEE Sensors Council for creating a platform that connects academic learning with practical engineering problems.

Finally, we thank our teammates for the many hours spent calibrating sensors, debugging firmware, and refining the vigilance scoring logic that turned this from an idea into a working prototype.

## Overview

Road accidents caused by reduced driver vigilance are a persistent transportation safety problem, particularly during long-distance and night-time travel. Existing driver monitoring solutions typically rely on in-cabin cameras or wearable devices, which raise privacy concerns, increase system cost, and are difficult to retrofit into ordinary vehicles.

NeuroDrive investigates an alternative approach: estimating driver vigilance through behavioural signals derived from the driver's interaction with the vehicle itself, rather than direct observation of the driver. The system continuously analyses steering behaviour and environmental context to generate a real-time Driver Vigilance Score, using only the sensing, processing, and communication capabilities of the MYOSA platform.

Because the approach requires no image capture and no wearable hardware, it is inherently privacy-preserving, low-cost, and suitable for retrofitting into any vehicle. The complete system runs on a single MYOSA Motherboard (ESP32), interfaced with three sensors over I2C, with local alerting through an OLED display and buzzer, and remote monitoring through a Blynk cloud dashboard.

**Key Features**
- Real-time Driver Vigilance Score computed from steering behaviour, cabin temperature, and journey duration
- Self-calibrating baseline: a 20-second on-device calibration routine learns each driver's normal steering variance
- OLED display showing temperature, altitude, and live vigilance score with a status bar
- Escalating active-buzzer alerts, from a short chirp to a sustained alarm, tied to risk severity
- Blynk cloud dashboard for remote and family monitoring of vigilance status and sensor readings
- Zero cameras, zero wearables: all sensing is derived from vehicle interaction and ambient conditions

## Demo / Examples

### Images

<p align="center">
  <img src="/assets/images/myosa-neurodrive/hardware-setup.jpg" width="800"><br/>
  <i>NeuroDrive hardware assembled on the MYOSA ESP32 motherboard with MPU6050, APDS9960, BMP180, OLED display, and buzzer</i>
</p>

<p align="center">
  <img src="/assets/images/myosa-neurodrive/oled-live-status.jpg" width="800"><br/>
  <i>OLED display showing live temperature, altitude, and Driver Vigilance Score</i>
</p>

<p align="center">
  <img src="/assets/images/myosa-neurodrive/blynk-dashboard.jpg" width="800"><br/>
  <i>Blynk cloud dashboard showing real-time vigilance status and sensor telemetry</i>
</p>

### Videos

<video controls width="100%">
  <source src="/neurodrive-demo.mp4" type="video/mp4">
</video>

## Features (Detailed)

### 1. Steering Interaction Sensing

The MPU6050, accessed through the MYOSA `Ag` sensor object, provides gyroscope readings along the X and Y axes. Steering variance is computed as the sum of the absolute values of these two readings.

On startup, the device runs a 20-second calibration routine during which the driver is asked to hold the wheel steady. The average steering variance recorded during this period becomes the driver's personal baseline, with a minimum floor applied to avoid oversensitivity. During monitoring, current steering variance is compared against this baseline: values well below or well above the normal range are both scored as reduced vigilance, since they indicate either disengagement or erratic overcorrection.

### 2. Environmental Context Sensing

The BMP180, accessed through the `Pr` object, provides temperature, pressure, and altitude, calculated against a standard sea-level pressure of 1013.25 hPa. Cabin temperature is mapped into a contextual risk index, since elevated temperature is a known contributor to drowsiness.

The APDS9960, accessed through the `Lpg` object, measures ambient light and is streamed to the cloud dashboard as an additional context signal.

### 3. Driver Vigilance Score Engine

The original project proposal defines the Driver Vigilance Score as a four-factor weighted combination of steering interaction (S), driver engagement (E), environmental context (C), and journey duration (J):

```
Target design (proposal):
DVS = 0.45 x S + 0.25 x E + 0.15 x C + 0.15 x J
```

The current firmware implements a three-factor version of this scoring engine, recalculated every second:

```
Current implementation:
DVS = 0.65 x S + 0.20 x C + 0.15 x J
```

The engagement index (E), intended to be derived from APDS9960 ambient light and proximity data, is currently logged and displayed on the Blynk dashboard but not yet weighted into the score. Integrating it into the DVS formula is the immediate next step in development, which will align the live scoring engine with the original four-factor model. The journey duration factor increases gradually over a two-hour reference window, reflecting the established link between drive duration and fatigue risk.

### 4. OLED Real-Time Display

The OLED renders a header, live temperature and altitude readings, the numeric vigilance score, and a progress bar that fills as risk increases. When the score crosses a warning threshold, the lower half of the screen inverts to display WARNING, CRITICAL ALERT, or DANGER LEVEL, depending on severity.

### 5. Escalating Buzzer Alert System

The proposal specified a haptic actuator for physical feedback. The built prototype instead uses an active buzzer, chosen for its reliability and simplicity within the development timeline while still delivering a clear, unmissable alert. Alert duration scales with severity: a 100 ms chirp at the Warning stage (DVS above 60), a 250 ms beep at Critical (DVS above 70), and a 500 ms alarm at Danger level (DVS above 85).

### 6. Blynk Cloud Dashboard

The device connects over Wi-Fi to a Blynk dashboard, transmitting vigilance score, status label, temperature, altitude, pressure, acceleration magnitude, and ambient light to virtual pins in real time. This allows a family member or fleet supervisor to monitor vigilance status remotely, and forms the basis for the GPS tracking and notification features planned in later development.

### 7. Auto-Calibration Sequence

Each power cycle begins with a guided calibration sequence, displayed on the OLED with a live countdown, ensuring the vigilance baseline adapts to each driver rather than relying on a fixed threshold.

## System Architecture

```
Sensor Layer (MPU6050 + APDS9960 + BMP180)
        |
ESP32 Processing Layer
        |
Sensor Fusion and Feature Extraction
        |
Driver Vigilance Score Engine
        |
OLED Display + Buzzer Alerts + Blynk Dashboard
        |
(Planned) GPS Tracking + ETA Estimation + Family Notifications
```

The first five stages are implemented in the current prototype. GPS tracking, ETA estimation, and family notification, described in the original proposal, are planned extensions to the Blynk dashboard layer.

## Innovation and Novelty

Unlike conventional camera-based fatigue monitoring systems, NeuroDrive estimates driver vigilance through indirect behavioural sensing using only the MYOSA sensor suite, without collecting images or requiring wearable devices. Combining motion sensing, environmental awareness, and on-device analytics into a single low-cost platform demonstrates how existing MYOSA sensor modules can be repurposed for behavioural safety monitoring, an application area distinct from the platform's typical use cases.

## Usage Instructions

**Step 1: Connect the I2C sensor chain**
```
Motherboard -> OLED -> MPU6050 -> APDS9960 -> BMP180
```

**Step 2: Connect the buzzer**
```
GND -> GND
VCC -> VIN
SIG -> GPIO16
```

**Step 3: Configure firmware credentials**

Open the sketch and set your own WiFi SSID, password, and Blynk auth token before flashing.

**Step 4: Power on the device**

Use any 5V USB power source. The OLED will display a connecting screen, followed by the calibration countdown.

**Step 5: Calibrate**

Hold the steering wheel steady for the full 20-second calibration window.

**Step 6: Monitor**

Live monitoring begins automatically once calibration completes. Open the Blynk app to view the remote dashboard.

## Tech Stack

| Component | Function in NeuroDrive |
|---|---|
| MYOSA Motherboard (ESP32) | Performs sensor fusion, analytics, communication, and system control |
| MPU6050 | Captures steering interaction patterns and motion dynamics |
| APDS9960 | Measures ambient light and proximity-based engagement context |
| BMP180 | Provides temperature, pressure, and altitude for contextual sensing |
| OLED Display | Displays Driver Vigilance Score, risk level, and system status |
| Buzzer | Generates escalating audio alerts during elevated-risk conditions |

Software: Arduino framework (C++), Blynk IoT platform.

## Requirements / Installation

```bash
# Arduino IDE libraries required
Blynk (BlynkSimpleEsp32)
WiFi (built-in ESP32 core)
myosa (MYOSA sensor library)
```

Install the ESP32 board package in Arduino IDE, add the libraries above through the Library Manager, then set the WiFi and Blynk credentials in the sketch before uploading.

## File Structure

```
/myosa-neurodrive
  |- myosa-neurodrive.md
  |- neurodrive-cover.jpg
  |- hardware-setup.jpg
  |- oled-live-status.jpg
  |- blynk-dashboard.jpg
  |- neurodrive-demo.mp4
  |- LICENSE
  `- neurodrive-main.ino
```

## License

MIT License. Free to use, modify, and distribute with attribution.

Developed by Team NeuroDrive for IEEE International MYOSA Event 6.0.

## Contribution Notes

This project was developed as a functional prototype for IEEE International MYOSA Event 6.0. Planned future work includes:

- Integrating the ambient-light engagement index into the DVS formula, bringing the live scoring engine to the full four-factor model
- Adding GPS-based ETA estimation and family notification features to the Blynk dashboard
- Incorporating vehicle telemetry such as steering angle, braking behaviour, and acceleration patterns for improved accuracy
- Evaluating machine learning models trained on larger behavioural datasets to refine vigilance estimation
- Adapting the sensing framework for operator monitoring in industrial vehicles and logistics fleets

Team NeuroDrive,
Sree Chitra Thirunal College of Engineering,
Thiruvananthapuram, Kerala, India
