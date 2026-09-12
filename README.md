# Smart Parking Availability Monitoring System Using IoT

An IoT-based smart parking system that monitors parking slot availability in real time using ESP32, IR sensors, LCD, LEDs, servo motors, and Blynk IoT.

## Project Overview

The system detects the availability of four parking slots using IR sensors. The ESP32 processes the sensor data and displays the parking status on an LCD. The parking information is also sent to the Blynk IoT platform for remote monitoring and booking.

Servo motors are used to control the entry and exit gates, while LEDs indicate the status of individual parking slots.

## Features

- Real-time parking slot monitoring
- Four-slot parking availability detection
- IR-based vehicle detection
- LCD display for parking status
- LED indication for slot status
- Parking slot booking through Blynk
- Remote monitoring using Blynk IoT
- Automatic entry and exit gate control

## System Architecture

![System Architecture](images/architecture.png)

## Data Flow

### Level 0

![Data Flow Level 0](images/data-flow-level-0.png)

### Level 1

![Data Flow Level 1](images/data-flow-level-1.png)

## Hardware Setup

![Hardware Setup](images/hardware-setup.jpg)

## Parking Model

![Parking Model](images/parking-model.jpg)

### Parking Model with Vehicles

![Parking Model with Vehicles](images/parking-model-with-vehicles.jpg)

## Blynk Dashboard

![Blynk Dashboard](images/blynk-dashboard.jpg)

## Technologies Used

- ESP32
- IR Sensors
- Arduino IDE
- Embedded C/C++
- Blynk IoT
- LCD Display
- LEDs
- Servo Motors
- Wi-Fi

## Project Structure

```text
smart-parking-iot/
├── images/
├── src/
│ └── smart_parking.ino
├── .gitignore
└── README.md
