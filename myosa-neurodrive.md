---
publishDate: 2026-08-25T00:00:00Z
title: NeuroDrive: A Multi-Sensor IoT Platform for Real-Time, Camera-Free Driver Vigilance Monitoring
excerpt: A privacy-preserving driver monitoring system that estimates vigilance from steering behaviour and environmental sensing, built entirely on the MYOSA (ESP32) platform with no cameras or wearables required.
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

We are grateful to the organizers of IEEE International MYOSA Event 6.0 and the IEEE Sensors Council for creating a platform that connects academic learning with practical engineering problems in road safety.

Finally, we thank our teammates for the many hours spent calibrating sensors, debugging firmware, and refining the vigilance scoring logic that turned this from an idea into a working prototype.

## Overview

Driver fatigue and reduced vigilance are consistently identified as major contributing factors in road accidents, particularly during long-distance and night-time travel. Existing driver monitoring solutions typically rely on in-cabin cameras or wearable devices, which raise privacy concerns, add cost, and are difficult to retrofit into ordinary vehicles.

NeuroDrive takes a different approach. It estimates driver vigilance not by observing the driver directly, but by analysing how the driver interacts with the vehicle, primarily through steering behaviour, combined with environmental context such as cabin temperature and journey duration. The entire system runs on a single MYOSA Motherboard (ESP32), interfaced with three sensors over I2C, with no image capture and no wearable hardware involved.

The system continuously computes a Driver Vigilance Score (DVS) on a scale of 0 to 100, displays it locally on an OLED screen, raises escalating buzzer alerts as risk increases, and streams live telemetry to a Blynk cloud dashboard for remote monitoring.

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

The system computes a composite score every second, combining steering behaviour (S), environmental context (C), and journey duration (J):

```
DVS = 0.65 x S + 0.20 x C + 0.15 x J
```

The journey duration factor increases gradually over a two-hour reference window, reflecting the established link between drive duration and fatigue risk. Ambient light data from the APDS9960 is currently logged and displayed on the dashboard but not yet weighted into the score. Full integration of this engagement index is planned as the next development step.

### 4. OLED Real-Time Display

The OLED renders a header, live temperature and altitude readings, the numeric vigilance score, and a progress bar that fills as risk increases. When the score crosses a warning threshold, the lower half of the screen inverts to display WARNING, CRITICAL ALERT, or DANGER LEVEL, depending on severity.

### 5. Escalating Buzzer Alert System

The prototype uses an active buzzer rather than the haptic motor described in the original proposal, chosen for its reliability within the build timeline. Alert duration scales with severity: a 100 ms chirp at the Warning stage (DVS above 60), a 250 ms beep at Critical (DVS above 70), and a 500 ms alarm at Danger level (DVS above 85).

### 6. Blynk Cloud Dashboard

The device connects over Wi-Fi to a Blynk dashboard, transmitting vigilance score, status label, temperature, altitude, pressure, acceleration magnitude, and ambient light to virtual pins in real time. This allows a family member or fleet supervisor to monitor vigilance status remotely, and forms the basis for the GPS tracking and notification features planned in later development.

### 7. Auto-Calibration Sequence

Each power cycle begins with a guided calibration sequence, displayed on the OLED with a live countdown, ensuring the vigilance baseline adapts to each driver rather than relying on a fixed threshold.

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

- MYOSA Motherboard (ESP32)
- MPU6050 (accelerometer and gyroscope)
- APDS9960 (gesture, proximity, and ambient light)
- BMP180 (barometric pressure and temperature)
- OLED display
- Active buzzer
- Arduino framework (C++)
- Blynk IoT platform

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

This project was developed as a functional prototype for IEEE International MYOSA Event 6.0. Planned future work includes integrating the ambient-light engagement index into the DVS formula, adding GPS-based ETA and family notification features to the Blynk dashboard, and evaluating machine learning models for improved vigilance classification across different drivers.

Team NeuroDrive
Sree Chitra Thirunal College of Engineering
Thiruvananthapuram, Kerala, India
