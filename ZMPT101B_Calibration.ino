/**
 * ZMPT101B Voltage Sensor Calibration Utility
 * 
 * This program calibrates and tests the ZMPT101B voltage sensor,
 * providing real-time RMS voltage readings and statistical analysis.
 * 
 * Features:
 * - Continuous voltage sampling
 * - Real-time RMS voltage calculation
 * - Statistical analysis (min, max, average)
 * - Adjustable sampling rate
 * - Serial output for data logging and analysis
 * 
 * Hardware:
 * - Arduino/ESP32 board
 * - ZMPT101B voltage sensor connected to analog pin A0
 * 
 */

#include <ZMPT101B.h>

// Configuration
#define SENSOR_PIN A0
#define MAINS_FREQUENCY 50.0
#define INITIAL_SENSITIVITY 940.25f
#define SAMPLE_INTERVAL 100  // milliseconds
#define SAMPLES_PER_READING 10
#define SERIAL_BAUD_RATE 115200

ZMPT101B voltageSensor(SENSOR_PIN, MAINS_FREQUENCY);

// Statistics variables
float minVoltage = 1000.0;
float maxVoltage = 0.0;
float totalVoltage = 0.0;
unsigned long readingCount = 0;

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  
  voltageSensor.setSensitivity(INITIAL_SENSITIVITY);
  
  Serial.println("ZMPT101B Voltage Sensor Calibration Utility");
  Serial.println("-------------------------------------------");
  Serial.println("Sampling started. Press any key to recalibrate sensitivity.");
}

void loop() {
  if (Serial.available()) {
    recalibrateSensitivity();
  }
  
  float voltage = sampleVoltage();
  updateStatistics(voltage);
  printResults(voltage);
  
  delay(SAMPLE_INTERVAL);
}

float sampleVoltage() {
  float sum = 0.0;
  for (int i = 0; i < SAMPLES_PER_READING; i++) {
    sum += voltageSensor.getRmsVoltage();
  }
  return sum / SAMPLES_PER_READING;
}

void updateStatistics(float voltage) {
  minVoltage = min(minVoltage, voltage);
  maxVoltage = max(maxVoltage, voltage);
  totalVoltage += voltage;
  readingCount++;
}

void printResults(float voltage) {
  Serial.print("Current RMS Voltage: ");
  Serial.print(voltage, 2);
  Serial.print("V | Min: ");
  Serial.print(minVoltage, 2);
  Serial.print("V | Max: ");
  Serial.print(maxVoltage, 2);
  Serial.print("V | Avg: ");
  Serial.print(totalVoltage / readingCount, 2);
  Serial.println("V");
}

void recalibrateSensitivity() {
  float newSensitivity;
  Serial.println("\nEnter new sensitivity value:");
  while (!Serial.available()) {
    // Wait for input
  }
  newSensitivity = Serial.parseFloat();
  voltageSensor.setSensitivity(newSensitivity);
  Serial.print("Sensitivity updated to: ");
  Serial.println(newSensitivity, 2);
  
  // Reset statistics
  minVoltage = 1000.0;
  maxVoltage = 0.0;
  totalVoltage = 0.0;
  readingCount = 0;
}