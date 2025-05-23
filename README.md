# ESP-IDF E-Paper Sensor Project

This project demonstrates how to interface multiple environmental sensors (DS18B20, BMP280, SI7021, CCS811) with an ESP32 and display the data on an e-paper display using ESP-IDF and Arduino-ESP32 components.

## Features

- **E-paper Display**: Uses the GxEPD library to drive various e-paper panels.
- **Sensors Supported**:
  - **DS18B20**: 1-Wire temperature sensor.
  - **BMP280**: I2C temperature and pressure sensor.
  - **SI7021**: I2C temperature and humidity sensor.
  - **CCS811**: I2C air quality sensor (CO₂ and TVOC).
- **MQTT Support**: Publishes sensor data to an MQTT broker.
- **FreeRTOS Tasks**: Sensor reading and display updates run in separate tasks.
- **Error Handling**: Includes CRC and range checks for robust sensor readings.

## Directory Structure

```
epaper-idf2/
├── components/
│   ├── Adafruit_BusIO/           # Adafruit I2C/SPI bus abstraction library
│   ├── Adafruit-GFX-Library/     # Adafruit graphics primitives for displays
│   ├── Adafruit-sensors/         # Adafruit sensors
│   ├── ds18b20/                  # 1-Wire DS18B20 temperature sensor driver
│   ├── GxEPD/                    # E-paper display driver library
│   └── ccs811/                   # CCS811 air quality sensor driver
├── main/
│   ├── BitmapGraphics.h          # Header for bitmap/e-paper graphics utilities
│   ├── wifi.h                    # Wi-Fi connection management header
│   ├── mqtt.h                    # MQTT client interface header
│   ├── main.cc                   # Main application entry point
│   ├── wifi.c                    # Wi-Fi connection management implementation
│   └── mqtt.c                    # MQTT client implementation
├── CMakeLists.txt                # Project build configuration
└── README.md                     # Project overview and instructions
```

## Getting Started

### Prerequisites

- [ESP-IDF v5.4.1 or newer](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/)
- ESP32 development board
- Supported e-paper display
- Sensors: DS18B20, BMP280, SI7021, CCS811
- Pull-up resistor (4.7kΩ) for DS18B20 data line

### Building and Flashing

1. **Clone the repository**  
   ```sh
   git clone https://github.com/vladipirogov/home-sensors.git
   cd epaper-idf2
   ```

2. **Set up ESP-IDF**  
   Follow the [official ESP-IDF setup guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/).

3. **Configure the project**  
   ```sh
   idf.py menuconfig
   ```

4. **Build and flash**  
   ```sh
   idf.py build
   idf.py -p <PORT> flash monitor
   ```

### Configuration

- **Sensor GPIOs**: Set the correct GPIO numbers for your sensors in `main.cc` or configuration headers.
- **MQTT Broker**: Configure your MQTT broker address and credentials if using MQTT.

### Usage

- The application will initialize all sensors and the e-paper display.
- Sensor data is read periodically and displayed on the e-paper.
- Data is also published to the MQTT broker if configured.

## Notes

- Ensure all sensors are properly connected and powered.
- Use a pull-up resistor for the DS18B20 data line.
- If you encounter unreliable readings from DS18B20, check wiring and power supply, and ensure CRC/range checks are enabled.

## License

This project is licensed under the BSD License and/or the licenses of the respective libraries used.