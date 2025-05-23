#include "Adafruit_Sensor.h"

/**************************************************************************/
/*!
    @brief  Prints sensor information to serial console
*/
/**************************************************************************/
void Adafruit_Sensor::printSensorDetails(void) {
  sensor_t sensor;
  getSensor(&sensor);
  printf("------------------------------------");
  printf("Sensor:       ");
  printf(sensor.name);
  printf("Type:         ");
  switch ((sensors_type_t)sensor.type) {
  case SENSOR_TYPE_ACCELEROMETER:
    printf("Acceleration (m/s2)");
    break;
  case SENSOR_TYPE_MAGNETIC_FIELD:
    printf("Magnetic (uT)");
    break;
  case SENSOR_TYPE_ORIENTATION:
    printf("Orientation (degrees)");
    break;
  case SENSOR_TYPE_GYROSCOPE:
    printf("Gyroscopic (rad/s)");
    break;
  case SENSOR_TYPE_LIGHT:
    printf("Light (lux)");
    break;
  case SENSOR_TYPE_PRESSURE:
    printf("Pressure (hPa)");
    break;
  case SENSOR_TYPE_PROXIMITY:
    printf("Distance (cm)");
    break;
  case SENSOR_TYPE_GRAVITY:
    printf("Gravity (m/s2)");
    break;
  case SENSOR_TYPE_LINEAR_ACCELERATION:
    printf("Linear Acceleration (m/s2)");
    break;
  case SENSOR_TYPE_ROTATION_VECTOR:
    printf("Rotation vector");
    break;
  case SENSOR_TYPE_RELATIVE_HUMIDITY:
    printf("Relative Humidity (percent)");
    break;
  case SENSOR_TYPE_AMBIENT_TEMPERATURE:
    printf("Ambient Temp (C)");
    break;
  case SENSOR_TYPE_OBJECT_TEMPERATURE:
    printf("Object Temp (C)");
    break;
  case SENSOR_TYPE_VOLTAGE:
    printf("Voltage (V)");
    break;
  case SENSOR_TYPE_CURRENT:
    printf("Current (mA)");
    break;
  case SENSOR_TYPE_COLOR:
    printf("Color (RGBA)");
    break;
  }

}
