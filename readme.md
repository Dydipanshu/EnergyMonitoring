# Smart Energy Monitoring System

## Overview
An advanced IoT-based energy monitoring solution using ESP32, featuring high-precision voltage and current measurements, real-time power analysis, and cloud connectivity. Ideal for both residential and industrial applications.

## Key Features
- Precision voltage (ZMPT101B) and current (ACS712) sensing
- Real-time power and energy consumption calculations
- Cloud-based monitoring via Blynk
- EEPROM-based data persistence
- Advanced sensor calibration utilities
- Oversampling and noise reduction techniques
- Statistical analysis of power metrics

## Repository Contents
- `EnergyMonitoringSystem.ino`: Core monitoring system
- `ZMPT101B_Calibration.ino`: Voltage sensor calibration
- `ACS712_Current_Calibration.ino`: Current sensor calibration

## Tech Stack
- Hardware: ESP32, ZMPT101B, ACS712 20A
- Libraries: EmonLib, ZMPT101B, Blynk
- Cloud Platform: Blynk

## Quick Start
1. Clone the repo
2. Install required libraries
3. Configure WiFi and Blynk credentials in `EnergyMonitoringSystem.ino`
4. Calibrate sensors using respective utilities
5. Upload `EnergyMonitoringSystem.ino` to ESP32

## Calibration
1. Use `ZMPT101B_Calibration.ino` for voltage sensor
2. Use `ACS712_Current_Calibration.ino` for current sensor
3. Update calibration values in main system

## Future Roadmap
- ML-based predictive analysis
- Three-phase system support
- Custom PCB design
- Smart home integration

## Contributing
Contributions welcome! Fork, branch, and submit a pull request with detailed descriptions.

## License
MIT License. See [LICENSE.md](LICENSE.md) for details.

## Contact
[Your Name] - [your.email@example.com]
Project Link: [https://github.com/yourusername/smart-energy-monitoring](https://github.com/Dydipanshu/EnergyMonitoring)