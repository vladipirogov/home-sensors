#include "mqtt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "mqtt_client.h"

static EventGroupHandle_t mqtt_event_group;

/**
 * MQTT event handler
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG1, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_subscribe(client, "esp-home/cmnd/#", 0);
            ESP_LOGI(TAG1, "Sent subscribe successful, msg_id=%d", msg_id);
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG1, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG1, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            msg_id = esp_mqtt_client_publish(client, "esp-home/status/activ", "1", 0, 0, 1);
            ESP_LOGI(TAG1, "Sent publish successful, msg_id=%d", msg_id);
            break;

        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG1, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG1, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG1, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG1, "MQTT_EVENT_ERROR");
            break;

        default:
            ESP_LOGI(TAG1, "Other event id: %d", event->event_id);
            break;
    }
}

/**
 * Starts the MQTT client
 */
esp_mqtt_client_handle_t mqtt_app_start()
{
    mqtt_event_group = xEventGroupCreate();

    // Configure MQTT client
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://192.168.1.107:1883", // Update with your broker URI
        .session.keepalive = 10,
        .session.last_will.topic = "esp-home/status/activ",
        .session.last_will.msg = "0",
        .session.last_will.retain = true,
    };

    //ESP_LOGI(TAG1, "[APP] Free memory: %d bytes", esp_get_free_heap_size());

    // Initialize MQTT client
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);

    // Register the event handler
    ESP_ERROR_CHECK(esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL));

    // Start the MQTT client
    ESP_ERROR_CHECK(esp_mqtt_client_start(client));

    return client;
}