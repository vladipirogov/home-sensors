cmake_minimum_required(VERSION 3.5)

idf_component_register(SRCS "Adafruit_Sensor.cpp" "Adafruit_BMP280.cpp" "Adafruit_Si7021.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES arduino-esp32)

project(Adafruit-sensors)
