#include <Arduino.h>

// ====================================================================
// HARDWARE PIN CONFIGURATION
// ====================================================================
#define RX1_SAFE_PIN 16
#define TX1_SAFE_PIN 17
#define RX2_CO_PIN   25
#define TX2_CO_PIN   26

#define MQ131_PIN       34
#define NH3_ANALOG_PIN  35
#define NO2_ANALOG_PIN  32

// ====================================================================
// CALIBRATION CONSTANTS
// ====================================================================
const float PM25_SLOPE = 1.0; 
const float PM10_SLOPE = 1.0;
const float RL_VALUE   = 10.0;
const float R0_CLEAN_AIR = 18.5; 
const float NH3_SENSITIVITY = 850.0;
const float NO2_SENSITIVITY = 420.0;
const float O3_CONV  = 1.65;
const float NO2_CONV = 1.50; 
const float NH3_CONV = 0.550;
const float CO_CONV  = 1.0; 

// ====================================================================
// GLOBALS
// ====================================================================
uint16_t global_pm25 = 0;
uint16_t global_pm10 = 0;
unsigned long lastPrintTime = 0;
const unsigned long printInterval = 2000;

// ====================================================================
// SENSOR READING FUNCTIONS
// ====================================================================
bool parsePM_Stream() {
  static uint8_t pm_state = 0;
  static uint8_t packet_buffer[32];
  static uint8_t byte_counter = 0;
  static uint16_t checksum_accumulator = 0;

  while (Serial1.available() > 0) {
    uint8_t incoming_byte = Serial1.read();
    switch (pm_state) {
      case 0: if (incoming_byte == 0x42) { packet_buffer[0] = incoming_byte; checksum_accumulator = incoming_byte; byte_counter = 1; pm_state = 1; } break;
      case 1: if (incoming_byte == 0x4D) { packet_buffer[1] = incoming_byte; checksum_accumulator += incoming_byte; byte_counter = 2; pm_state = 2; } else pm_state = 0; break;
      case 2: packet_buffer[byte_counter] = incoming_byte; if (byte_counter < 30) checksum_accumulator += incoming_byte; byte_counter++;
              if (byte_counter >= 32) {
                uint16_t parsed_checksum = ((uint16_t)packet_buffer[30] << 8) | packet_buffer[31];
                if (checksum_accumulator == parsed_checksum) {
                  global_pm25 = ((uint16_t)packet_buffer[6] << 8) | packet_buffer[7];
                  global_pm10 = ((uint16_t)packet_buffer[10] << 8) | packet_buffer[11];
                  pm_state = 0; return true;
                }
                pm_state = 0;
              } break;
    }
  }
  return false;
}

float readZE15_CO() {
  unsigned long t = millis();
  while (Serial2.available() > 9) Serial2.read();
  while (Serial2.available() && Serial2.peek() != 0xFF) { Serial2.read(); if (millis() - t > 150) return -1.0f; }
  if (Serial2.available() < 9) return -1.0f;
  uint8_t buf[9]; Serial2.readBytes(buf, 9);
  if (buf[0] != 0xFF || buf[1] != 0x04) return -1.0f;
  uint8_t cks = 0; for (int i = 1; i <= 7; i++) cks += buf[i];
  cks = (~cks) + 1;
  if (cks != buf[8] || (buf[4] & 0x80) == 0x80) return -1.0f;
  uint16_t rawValue = ((buf[4] & 0x1F) << 8) | buf[5];
  return (rawValue * 0.1f) * CO_CONV;
}

float readDFRobot_Gas(int pin, float sensitivity, float conv_factor, bool is_no2) {
  int adc = analogRead(pin); float volt = ((float)adc / 4095.0f) * 3.3f;
  //if (volt >= 3.28f || volt <= 0.05f) return 0.0f;
  float ppb = 0.0f;
  if (is_no2) { if (volt < 2.263f) ppb = (2.263f - volt) * (sensitivity * 0.12f); }
  else { if (volt > 0.215f) ppb = (volt - 0.215f) * (sensitivity * 0.5f); }
  return ppb * conv_factor;
}

float readMQ131_O3() {
  int adc = analogRead(MQ131_PIN); float volt = ((float)adc / 4095.0f) * 3.3f;
  if (volt <= 0.1f || volt >= 3.28f) return 0.0f;
  float rs = ((3.3f - volt) / volt) * RL_VALUE;
  float ratio = rs / R0_CLEAN_AIR;
  return (0.25f * pow(ratio, -1.25f) * 1000.0f * O3_CONV);
}

// ====================================================================
// MAIN SETUP & LOOP
// ====================================================================
void setup() {
  Serial.begin(115200);
  
  // Set ADC resolution for ESP32
  analogSetAttenuation(ADC_11db);
  
  // Initialize UART for sensors
  Serial1.begin(9600, SERIAL_8N1, RX1_SAFE_PIN, TX1_SAFE_PIN); // PM Sensor
  Serial2.begin(9600, SERIAL_8N1, RX2_CO_PIN, TX2_CO_PIN);     // CO Sensor
  
  // Define Analog pins as input
  pinMode(MQ131_PIN, INPUT);
  pinMode(NH3_ANALOG_PIN, INPUT);
  pinMode(NO2_ANALOG_PIN, INPUT);

  Serial.println("=========================================");
  Serial.println("  SENSOR DIAGNOSTIC TEST STARTED ");
  Serial.println("=========================================");
}

void loop() {
  // Constantly parse incoming PM data in the background
  while (Serial1.available() > 0) {
    parsePM_Stream();
  }

  // Print all values every 2 seconds
  if (millis() - lastPrintTime >= printInterval) {
    lastPrintTime = millis();

    // Read all sensors
    float pm25 = (float)global_pm25 * PM25_SLOPE;
    float pm10 = (float)global_pm10 * PM10_SLOPE;
    float o3   = readMQ131_O3();
    float nh3  = readDFRobot_Gas(NH3_ANALOG_PIN, NH3_SENSITIVITY, NH3_CONV, false);
    float no2  = readDFRobot_Gas(NO2_ANALOG_PIN, NO2_SENSITIVITY, NO2_CONV, true);
    float co   = readZE15_CO();

    // Print to Serial Monitor
    Serial.println("--- Current Readings ---");
    Serial.printf("PM2.5 : %7.1f ug/m3\n", pm25);
    Serial.printf("PM10  : %7.1f ug/m3\n", pm10);
    Serial.printf("NO2   : %7.1f ug/m3\n", no2);
    Serial.printf("NH3   : %7.1f ug/m3\n", nh3);
    Serial.printf("O3    : %7.1f ug/m3\n", o3);
    
    if (co >= 0) {
      Serial.printf("CO    : %7.1f mg/m3\n", co);
    } else {
      Serial.println("CO    : SENSOR TIMEOUT/ERROR");
    }
    Serial.println("------------------------\n");
  }
}
