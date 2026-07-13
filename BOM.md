# Bill of Materials (BOM)

This document outlines all the hardware components required to build the ESP32 Air Quality Monitoring Station. 

## Primary Components

| Item | Qty | Description & Specifications | Purpose in Project |
| :--- | :---: | :--- | :--- |
| **ESP32 Development Board** | 1 | Standard 30-pin or 38-pin ESP-WROOM-32 module. | The core microcontroller handling UART parsing, ADC conversion, and serial output. |
| **PMS5003 / PMS7003** | 1 | Plantower Particulate Matter Sensor. | Reads PM2.5 and PM10 levels. Communicates via UART at 3.3V logic. |
| **Winsen ZE15-CO** | 1 | Electrochemical Carbon Monoxide Sensor. | Provides highly accurate CO baseline readings via UART at 3.3V logic. |
| **MQ-131 Module** | 1 | Semiconductor Gas Sensor (Breakout Board). | Detects Ozone (O3). Outputs a 5V analog signal. |
| **MQ-136 Module** | 1 | Semiconductor Gas Sensor (Breakout Board). | Detects Nitrogen Dioxide (NO2). Outputs a 5V analog signal. |
| **MQ-137 Module** | 1 | Semiconductor Gas Sensor (Breakout Board). | Detects Ammonia (NH3). Outputs a 5V analog signal. |
| **MQ-7 Module** | 1 | Semiconductor Gas Sensor (Breakout Board). | Detects Carbon Monoxide (CO). Outputs a 5V analog signal. |

## Passive Components & Wiring

| Item | Qty | Description & Specifications | Purpose in Project |
| :--- | :---: | :--- | :--- |
| **1.8kΩ Resistor** | 4 | 1/4 Watt, Through-hole. | Forms the top half (R1) of the voltage dividers for the 4 MQ sensors. |
| **3.3kΩ Resistor** | 4 | 1/4 Watt, Through-hole. | Forms the bottom half (R2) of the voltage dividers for the 4 MQ sensors. |
| **5V Power Supply** | 1 | 5V DC Wall Adapter (Minimum 2 Amps). | Essential for powering the internal heating elements of the four MQ sensors simultaneously without burning out the ESP32's onboard regulator. |
| **Breadboard** | 1-2 | Standard 830-tie point breadboards. | For prototyping and component mounting. |
| **Jumper Wires** | 1 Set | Male-to-Male and Female-to-Male (Dupont cables). | Connecting sensors, establishing the shared ground, and building the voltage dividers. |

---

### ⚠️ A Note on the Resistors (The Voltage Dividers)
The 4 MQ sensor modules output a 5V analog signal, but the ESP32 ADC pins can only safely handle 3.3V. The **1.8kΩ** and **3.3kΩ** resistors are critical for building four separate voltage dividers. 

Using this specific combination steps the 5V signal down to approximately **3.23V**, which safely maximizes the reading resolution of the ESP32's 0-4095 ADC range without risking hardware damage.
