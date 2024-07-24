/**
 * Advanced Energy Monitoring System
 * 
 * This system measures voltage, current, and power consumption,
 * calculates energy usage over time, and reports data to a cloud service.
 * 
 * Components:
 * - ESP32 microcontroller
 * - ZMPT101B voltage sensor
 * - Current transformer sensor
 * - WiFi connectivity
 * - Cloud integration for remote monitoring
 * 
 */

#include <ZMPT101B.h>
#include "EmonLib.h"
#include <EEPROM.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Configuration constants
#define VOLTAGE_SENSOR_PIN A0
#define CURRENT_SENSOR_PIN 34
#define VOLTAGE_CALIBRATION 41.5
#define CURRENT_CALIBRATION 0.15
#define MAINS_FREQUENCY 50.0
#define SENSITIVITY 971.25f

// Cloud service credentials (replace with your own)
#define CLOUD_TEMPLATE_ID "TMPL_ID_PLACEHOLDER"
#define CLOUD_TEMPLATE_NAME "Energy_Meter"
#define CLOUD_AUTH_TOKEN "AUTH_TOKEN_PLACEHOLDER"

// WiFi credentials (to be filled securely)
const char ssid[] = "WIFI_SSID";
const char pass[] = "WIFI_PASSWORD";

// Sensor initialization
ZMPT101B voltageSensor(VOLTAGE_SENSOR_PIN, MAINS_FREQUENCY);
EnergyMonitor emon;

// Timer for regular updates
BlynkTimer timer;

// Energy calculation variables
float kWh = 0.0;
unsigned long lastMillis = 0;

// EEPROM address for persistent storage
const int EEPROM_KWH_ADDR = 0;

// Function prototypes
void sendEnergyDataToCloud();
void readEnergyDataFromEEPROM();
void saveEnergyDataToEEPROM();

void setup() {
    Serial.begin(115200);
    
    // Initialize cloud connection
    Blynk.begin(CLOUD_AUTH_TOKEN, ssid, pass);
    
    // Configure voltage sensor
    voltageSensor.setSensitivity(SENSITIVITY);
    
    // Initialize EEPROM
    EEPROM.begin(sizeof(float));
    readEnergyDataFromEEPROM();
    
    // Setup energy monitoring
    emon.voltage(35, VOLTAGE_CALIBRATION, 1.7);
    emon.current(CURRENT_SENSOR_PIN, CURRENT_CALIBRATION);
    
    // Set up periodic data transmission
    timer.setInterval(5000L, sendEnergyDataToCloud);
    
    Serial.println("Energy Monitoring System Initialized");
}

void loop() {
    Blynk.run();
    timer.run();
}

void sendEnergyDataToCloud() {
    emon.calcVI(20, 2000);
    
    // Calculate energy consumption
    unsigned long currentMillis = millis();
    float timeDelta = (currentMillis - lastMillis) / 3600000.0;  // Convert to hours
    kWh += emon.apparentPower * timeDelta / 1000.0;  // Convert W to kW
    lastMillis = currentMillis;
    
    // Log data for debugging
    Serial.printf("Vrms: %.2fV\tIrms: %.4fA\tPower: %.4fW\tkWh: %.5fkWh\n",
                  emon.Vrms, emon.Irms, emon.apparentPower, kWh);
    
    // Persist data
    saveEnergyDataToEEPROM();
    
    // Send data to cloud
    Blynk.virtualWrite(V0, emon.Vrms);
    Blynk.virtualWrite(V1, emon.Irms);
    Blynk.virtualWrite(V2, emon.apparentPower);
    Blynk.virtualWrite(V3, kWh);
}

void readEnergyDataFromEEPROM() {
    EEPROM.get(EEPROM_KWH_ADDR, kWh);
    if (isnan(kWh)) {
        kWh = 0.0;
        saveEnergyDataToEEPROM();
    }
}

void saveEnergyDataToEEPROM() {
    EEPROM.put(EEPROM_KWH_ADDR, kWh);
    EEPROM.commit();
}