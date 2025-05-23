#ifndef MAIN_INCLUDE_WIFI_H_
#define MAIN_INCLUDE_WIFI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "freertos/event_groups.h"

#define CONFIG_ESP_WIFI_SSID "Keenetic-2919"
#define CONFIG_ESP_WIFI_PASSWORD "32w6BRSe"
static const char *TAG = "WIFI_ESP";

static EventGroupHandle_t wifi_event_group = NULL;

void wifi_start();

#ifdef __cplusplus
}
#endif

#endif 