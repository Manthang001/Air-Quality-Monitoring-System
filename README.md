# ESP32 Comprehensive Air Quality Monitoring Station

## Overview
This repository contains the firmware for a real-time, multi-sensor environmental monitoring node built on the ESP32. It is designed to track a wide spectrum of atmospheric pollutants—ranging from particulate matter (PM2.5/PM10) to reactive and toxic gases (O3, NO2, NH3, and CO). 

The system integrates both digital (UART) and analog (ADC) sensors, handling continuous background data streams and raw voltage conversions to provide a complete picture of local air quality.

## Key Features
* **Multi-Protocol Interfacing:** Simultaneously manages dual hardware serial streams (UART) for precision sensors alongside multi-channel analog reads.
* **Particulate & Gas Tracking:** Monitors physical dust and smog (PM2.5, PM10) alongside volatile gases (Carbon Monoxide, Ozone, Nitrogen Dioxide, Ammonia).
* **Raw Data Diagnostics:** Currently configured as a hardware diagnostic tool to read, parse, and validate raw sensor byte streams and ADC values before applying calibration and PPM conversion algorithms.

## Hardware Stack
* **Microcontroller:** ESP32 (configured for 11dB ADC attenuation for 0-4095 resolution)
* **Particulate Matter:** PMS-series sensor (e.g., PMS5003/7003) via UART
* **Digital Gas Sensor:** Winsen ZE15-CO (Carbon Monoxide) via UART
* **Analog Gas Sensors (MQ Series):**
  * MQ-131 (Ozone / O3)
  * MQ-136 (Nitrogen Dioxide / NO2)
  * MQ-137 (Ammonia / NH3)
  * MQ-7 (Carbon Monoxide / CO)

## Pin Mapping

| Component | ESP32 Pin | Interface |
|---|---|---|
| PMS Sensor RX/TX | 16 / 17 (Serial1) | UART |
| ZE15 CO RX/TX | 25 / 26 (Serial2) | UART |
| MQ-136 (NO2) AOUT | 32 | ADC |
| MQ-7 (CO) AOUT | 33 | ADC |
| MQ-131 (O3) AOUT | 34 | ADC |
| MQ-137 (NH3) AOUT | 35 | ADC |

> **⚠️ Hardware Warning for MQ Sensors:** MQ series breakout boards typically output a 5V analog signal. Because the ESP32 operates on 3.3V logic, the AOUT pins must be routed through a voltage divider before connecting to the ESP32 to prevent damage to the ADC pins.

## Next Steps / Roadmap
* Implement voltage dividers on all MQ sensor outputs.
* Execute a 48-hour continuous burn-in for MQ semiconductor stabilization.
* Establish clean-air baseline values (R0) for the MQ sensors.
* Map raw ADC values to physical gas concentrations (PPM) using sensor datasheet logarithmic curves.
