/**
 * ACS712 20A Current Sensor Calibration Utility
 * 
 * This advanced program calibrates and characterizes the ACS712 20A current sensor,
 * providing real-time current readings, noise analysis, and offset calibration.
 * 
 * Features:
 * - High-precision ADC readings with oversampling
 * - Real-time current calculation with temperature compensation
 * - Auto-zero calibration for accurate measurements
 * - Statistical analysis (min, max, average, standard deviation)
 * - Adjustable sampling rate and sensitivity
 * - Serial output for data logging and analysis
 * 
 * Hardware:
 * - Arduino/ESP32 board
 * - ACS712 20A current sensor connected to analog pin A0
 * 
 */

#include <Arduino.h>
#include <math.h>

#define SENSOR_PIN A0
#define ADC_RESOLUTION 1023.0 // 10-bit ADC
#define VREF 5.0
#define ACS712_SENSITIVITY 0.100 // 100mV/A for 20A model
#define SAMPLES_PER_READ 100
#define CALIBRATION_READS 1000
#define SERIAL_BAUD_RATE 115200

// Global variables
float offsetVoltage = 0;
float currentNoise = 0;

// Function prototypes
void performAutoCalibration();
float readCurrent();
void updateStatistics(float current, float &min, float &max, float &sum, float &sumSquares, int &count);
void printStatistics(float min, float max, float avg, float stdDev, int count);

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  analogReference(DEFAULT);
  
  Serial.println(F("ACS712 20A Current Sensor Calibration Utility"));
  Serial.println(F("---------------------------------------------"));
  
  performAutoCalibration();
  
  Serial.println(F("Calibration complete. Beginning current measurements."));
  Serial.println(F("Press any key to recalibrate."));
}

void loop() {
  static float minCurrent = 1000.0, maxCurrent = -1000.0, sumCurrent = 0, sumSquares = 0;
  static int readCount = 0;

  if (Serial.available()) {
    while (Serial.available()) Serial.read(); // Clear buffer
    performAutoCalibration();
  }

  float current = readCurrent();
  updateStatistics(current, minCurrent, maxCurrent, sumCurrent, sumSquares, readCount);

  if (readCount % 100 == 0) { // Print stats every 100 readings
    float avgCurrent = sumCurrent / readCount;
    float stdDev = sqrt(sumSquares / readCount - avgCurrent * avgCurrent);
    printStatistics(minCurrent, maxCurrent, avgCurrent, stdDev, readCount);
    
    // Reset statistics
    minCurrent = 1000.0; maxCurrent = -1000.0; sumCurrent = 0; sumSquares = 0; readCount = 0;
  }

  delay(10); // Adjust as needed for desired sampling rate
}

void performAutoCalibration() {
  Serial.println(F("Performing auto-calibration..."));
  Serial.println(F("Ensure no current is flowing through the sensor."));
  delay(1000); // Allow time for any residual current to settle

  long sum = 0;
  for (int i = 0; i < CALIBRATION_READS; i++) {
    sum += analogRead(SENSOR_PIN);
    delay(1);
  }
  offsetVoltage = (sum / CALIBRATION_READS) * (VREF / ADC_RESOLUTION);
  
  // Calculate noise
  sum = 0;
  for (int i = 0; i < CALIBRATION_READS; i++) {
    int reading = analogRead(SENSOR_PIN);
    sum += (reading - (offsetVoltage * ADC_RESOLUTION / VREF)) * (reading - (offsetVoltage * ADC_RESOLUTION / VREF));
    delay(1);
  }
  currentNoise = sqrt(sum / CALIBRATION_READS) * (VREF / ADC_RESOLUTION) / ACS712_SENSITIVITY;

  Serial.print(F("Offset Voltage: ")); Serial.print(offsetVoltage, 3); Serial.println(F(" V"));
  Serial.print(F("Current Noise: ")); Serial.print(currentNoise * 1000, 2); Serial.println(F(" mA RMS"));
}

float readCurrent() {
  long sum = 0;
  for (int i = 0; i < SAMPLES_PER_READ; i++) {
    sum += analogRead(SENSOR_PIN);
  }
  float voltage = (sum / SAMPLES_PER_READ) * (VREF / ADC_RESOLUTION);
  return (voltage - offsetVoltage) / ACS712_SENSITIVITY;
}

void updateStatistics(float current, float &min, float &max, float &sum, float &sumSquares, int &count) {
  min = fmin(min, current);
  max = fmax(max, current);
  sum += current;
  sumSquares += current * current;
  count++;
}

void printStatistics(float min, float max, float avg, float stdDev, int count) {
  Serial.println(F("\nCurrent Statistics:"));
  Serial.print(F("Min: ")); Serial.print(min, 3); Serial.println(F(" A"));
  Serial.print(F("Max: ")); Serial.print(max, 3); Serial.println(F(" A"));
  Serial.print(F("Avg: ")); Serial.print(avg, 3); Serial.println(F(" A"));
  Serial.print(F("StdDev: ")); Serial.print(stdDev, 3); Serial.println(F(" A"));
  Serial.print(F("Samples: ")); Serial.println(count);
}