# AquaSense-Smart-Irrigation
Smart irrigation system using ESP32 with soil moisture, water flow and pressure monitoring.
AquaSense – Smart Irrigation System

An ESP32-based smart irrigation system for intelligent soil moisture monitoring and automated water management.

Overview

AquaSense is designed to monitor soil moisture at two different depths, identify the drier irrigation zone and manage irrigation based on soil conditions. It also incorporates water flow and pressure monitoring to identify possible water delivery issues.

Key Features

- Dual Soil Moisture Monitoring: Monitors moisture levels in two zones.
- Smart Irrigation Logic: Prioritizes the drier zone based on moisture readings.
- Pump Control: Controls irrigation through the programmed system logic.
- Flow Monitoring: Tracks water flow conditions.
- Pressure Monitoring: Identifies low-pressure conditions.
- OLED Display: Displays moisture readings and system status.
- Fault Detection: Identifies possible flow-related issues.

Components Used

- ESP32 DevKit V1
- Soil Moisture Sensor Inputs
- YF-S201 Water Flow Sensor
- Pressure Sensor
- SSD1306 OLED Display
- Relay Module
- 12V DC Water Pump
- Buck Converter

Technologies Used

Programming: Embedded C++ (Arduino)

Platform: Wokwi Simulator

Communication: I2C

Libraries: Wire, Adafruit_GFX, Adafruit_SSD1306

Working Principle

1. Reads moisture levels from two soil zones.
2. Compares the readings with a predefined moisture threshold.
3. Selects the drier zone for irrigation.
4. Monitors water flow and pressure.
5. Displays readings, system status and alerts on the OLED.

Simulation

Wokwi Project:
https://wokwi.com/projects/478502519315182593

Future Enhancements

- IoT-based remote monitoring.
- Mobile application integration.
- Weather-based irrigation scheduling.
- Solar-powered operation.

---

Project Domain: Internet of Things (IoT) | Embedded Systems | Smart Agriculture
