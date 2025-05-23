#include "string.h"
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <Fonts/FreeSansBold24pt7b.h>
#include <ds18b20.h>
#include "wifi.h"
#include "mqtt.h"
#include "nvs_flash.h"                 
#include <Wire.h>
#include "Adafruit_GFX.h"
#include "GxEPD.h"
#include "GxIO_SPI.h"      
#include "GxGDEH0154D67.h"
#include "BitmapGraphics.h"
#include "ccs811.h"                     
#include "Adafruit_Si7021.h"            
#include "Adafruit_BMP280.h"                   
#include <Wire.h>


extern "C"
{
    void app_main();
}


#define DS_GPIO 25
#define SDA_1 33
#define SCL_1 32

#define GxEPD_BLACK     0x0000
#define GxEPD_WHITE     0xFFFF

#define TOPIC "esp-home/send"
#define PAYLOAD "{\"ds18b20_tmpr_1_str\":%f,\
                 \"bmp280_tmpr_str\":%f,\
                 \"bmpr280_prs_str\":%f,\
                 \"si702x_tmpr_str\":%f,\
                 \"si702x_hum_str\":%f,\
                 \"ccs811_co2_str\":%d,\
                 \"ccs811_tvoc_str\":%d}"

char  buffer[500];
static esp_mqtt_client_handle_t mqtt_client = NULL;

TwoWire I2CBME = TwoWire(1);
CCS811 ccs811 = CCS811(-1, CCS811_SLAVEADDR_0, &I2CBME);
Adafruit_BMP280 bmp280 = Adafruit_BMP280(&I2CBME);                                                        
Adafruit_Si7021 SI702x = Adafruit_Si7021(&I2CBME);

GxIO_Class io(SPI, SS, 22, 21);
GxEPD_Class display(io, 21, 4);

void show_partial_update(float temperature) {
  printf("Updating display ...\n");
  String temperature_str = String(temperature,1);
  const GFXfont* f = &FreeSansBold24pt7b;
  
  uint16_t box_x = 60;
  uint16_t box_y = 60;
  uint16_t box_w = 90;
  uint16_t box_h = 100;
  uint16_t cursor_y = box_y + 16;

  display.setRotation(45);
  display.setFont(f);
  display.setTextColor(GxEPD_BLACK);

  display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
  display.setCursor(box_x, cursor_y+38);
  display.print(temperature_str); 
  display.updateWindow(box_x, box_y, box_w, box_h, true);
}

/**
 * */
void begin_cjmcu_8128_sensors() {
  printf("Wire begin\n");
  I2CBME.begin(SDA_1, SCL_1, 100000);

  printf("CCS811 test\n");
  ccs811.set_i2cdelay(50);
  if (!ccs811.begin()) {
    for(int i = 0; i < 3; i++) {
      printf("Failed to start sensor! Please check your wiring.\n");
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
  }

  bool ok = ccs811.start(CCS811_MODE_1SEC);
  if ( !ok ) printf("setup: CCS811 start FAILED\n");

  printf("BMP280 test\n");     
  if (!bmp280.begin(0x76)) {
    for(int i = 0; i < 3; i++) {
      printf("Could not find a valid BMP280 sensor, check wiring!\n");
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
  }

  printf("Si7021 test!\n");     
  if (!SI702x.begin()) {
    for(int i = 0; i < 3; i++) {
      printf("Did not find Si702x sensor!\n");
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
  }
  printf("Found model \n");
  switch (SI702x.getModel()) {
    case SI_Engineering_Samples:
      printf("SI engineering samples\n"); break;
    case SI_7013:
      printf("Si7013\n"); break;
    case SI_7020:
      printf("Si7020\n"); break;
    case SI_7021:
      printf("Si7021\n"); break;
    case SI_UNKNOWN:
    default:
      printf("Unknown\n");
  }
  printf(" Revision(\n");
  printf("%d\n", SI702x.getRevision());
  printf(")\n");
  //printf(" Serial #"); printf(SI702x.sernum_a, HEX); printfln(SI702x.sernum_b, HEX);
}

void begin_display() {
  display.init();
  display.drawExampleBitmap(gImage_splash, 0, 0, 200, 200, GxEPD_BLACK);
  display.update();
  delay(3000);
  display.drawExampleBitmap(gImage_gui, 0, 0, 200, 200, GxEPD_BLACK);
  display.update();
  display.drawExampleBitmap(gImage_gui, sizeof(gImage_gui), 1 |  (1 << 6));
}


void display_task(void *param) {
  printf("DS18B20 init\n");
  ds18b20_init(DS_GPIO);

  begin_cjmcu_8128_sensors();

  begin_display();

  while (true) {
    float ds18b20_tmpr_1 = ds18b20_get_temp();
    printf("DS18B20 => Temperature = ");
    printf("%f", ds18b20_tmpr_1);
    printf(" °C, ");
    show_partial_update(ds18b20_tmpr_1);

    float bmp280_tmpr = bmp280.readTemperature();
    printf("BMP280 => Temperature = ");
    printf("%f", bmp280_tmpr);
    printf(" °C, ");

    float bmpr280_prs = bmp280.readPressure() / 100;
    printf("Pressure = ");
    printf("%f", bmpr280_prs);
    printf(" Pa, \n");

    float si702x_tmpr = SI702x.readTemperature();
    float si702x_hum = SI702x.readHumidity();
    printf("SI702x => Temperature = ");
    printf("%f", si702x_tmpr);
    printf(" °C, ");
    printf("Humidity = ");
    printf("%f", si702x_hum);
    printf("\n");

    uint16_t eco2, etvoc, errstat, raw;     // Read CCS811

    ccs811.set_envdata(bmp280_tmpr, si702x_hum);
    ccs811.read(&eco2, &etvoc, &errstat, &raw);
    if ( errstat == CCS811_ERRSTAT_OK ) {
      printf("CCS811 => CO2 = ");
      printf("%d", eco2);
      printf("ppm, TVOC = ");
      printf("%d\n", etvoc);
    }

      sprintf(buffer, PAYLOAD, ds18b20_tmpr_1, bmp280_tmpr, bmpr280_prs, si702x_tmpr, si702x_hum, eco2, etvoc);

      printf("before sending\n");

      esp_mqtt_client_publish(mqtt_client, TOPIC, buffer, 0,0,0);

      vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
 
  }

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_start();
    mqtt_client = mqtt_app_start();

   /* we create a new task here */
  xTaskCreate(
      &display_task,   /* Task function. */
      "display_task", /* name of task. */
      10000,                /* Stack size of task */
      NULL,                 /* parameter of the task */
      5,                    /* priority of the task */
      NULL);                /* Task handle to keep track of created task */
}
