#ifndef MAIN_INCLUDE_MQTT_H_
#define MAIN_INCLUDE_MQTT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_event.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "freertos/event_groups.h"

#define BIT_1 0x00000002
static const char *TAG1 = "MQTTS_ESP";
static EventGroupHandle_t mqtt_event_group = NULL;

esp_mqtt_client_handle_t mqtt_app_start();

#ifdef __cplusplus
}
#endif

#endif 