/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
     along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_rom_sys.h" // For esp_rom_delay_us()
#include "ds18b20.h"

static const char *TAG = "DS18B20";

int DS_GPIO;
int init = 0;

// Define a global portMUX_TYPE variable
static portMUX_TYPE ds18b20_mux = portMUX_INITIALIZER_UNLOCKED;

// Update macros to use the global variable
#define noInterrupts() taskENTER_CRITICAL(&ds18b20_mux)
#define interrupts() taskEXIT_CRITICAL(&ds18b20_mux)

/// Sends one bit to bus
void ds18b20_send(char bit) {
    gpio_set_direction(DS_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(DS_GPIO, 0);
    esp_rom_delay_us(5); // Replaced ets_delay_us with esp_rom_delay_us
    if (bit == 1) gpio_set_level(DS_GPIO, 1);
    esp_rom_delay_us(80);
    gpio_set_level(DS_GPIO, 1);
}

// Reads one bit from bus
unsigned char ds18b20_read(void) {
    unsigned char PRESENCE = 0;
    gpio_set_direction(DS_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(DS_GPIO, 0);
    esp_rom_delay_us(2);
    gpio_set_level(DS_GPIO, 1);
    esp_rom_delay_us(15);
    gpio_set_direction(DS_GPIO, GPIO_MODE_INPUT);
    if (gpio_get_level(DS_GPIO) == 1) PRESENCE = 1;
    else PRESENCE = 0;
    return PRESENCE;
}

// Sends one byte to bus
void ds18b20_send_byte(char data) {
    unsigned char i;
    unsigned char x;
    for (i = 0; i < 8; i++) {
        x = data >> i;
        x &= 0x01;
        ds18b20_send(x);
    }
    esp_rom_delay_us(100);
}

// Reads one byte from bus
unsigned char ds18b20_read_byte(void) {
    unsigned char i;
    unsigned char data = 0;
    for (i = 0; i < 8; i++) {
        if (ds18b20_read()) data |= 0x01 << i;
        esp_rom_delay_us(15);
    }
    return data;
}

// Sends reset pulse
unsigned char ds18b20_RST_PULSE(void){
	unsigned char presence;
	gpio_set_direction(DS_GPIO, GPIO_MODE_OUTPUT);
	noInterrupts();
	gpio_set_level(DS_GPIO, 0);
	esp_rom_delay_us(480);
	gpio_set_level(DS_GPIO, 1);
	gpio_set_direction(DS_GPIO, GPIO_MODE_INPUT);
	esp_rom_delay_us(70);
	presence = (gpio_get_level(DS_GPIO) == 0);
	esp_rom_delay_us(410);
	interrupts();
	return presence;
}

// Returns temperature from sensor
float ds18b20_get_temp(void) {
    static float last_valid_temp = 0.0f; // Cache for last valid temperature

    if (init == 1) {
        for (int retry = 0; retry < 3; retry++) { // Retry up to 3 times
            unsigned char check;
            unsigned char scratchpad[9]; // Scratchpad data
            check = ds18b20_RST_PULSE();
            if (check == 1) {
                ds18b20_send_byte(0xCC);
                ds18b20_send_byte(0x44);
                vTaskDelay(pdMS_TO_TICKS(750)); // Wait for conversion
                check = ds18b20_RST_PULSE();
                ds18b20_send_byte(0xCC);
                ds18b20_send_byte(0xBE);
                for (int i = 0; i < 9; i++) {
                    scratchpad[i] = ds18b20_read_byte();
                }
                check = ds18b20_RST_PULSE();

                // Validate CRC
                if (ds18b20_crc8(scratchpad, 8) != scratchpad[8]) {
                    ESP_LOGE(TAG, "CRC check failed, returning last valid temperature: %.2f", last_valid_temp);
                    continue; // Retry on CRC failure
                }

                char temp1 = scratchpad[0];
                char temp2 = scratchpad[1];
                float temp = (float)(temp1 + (temp2 * 256)) / 16;

                // Validate the temperature range
                if (temp >= -55.0 && temp <= 125.0) {
                        last_valid_temp = temp; // Update cache
                    return temp; // Valid temperature
                } else {
                    ESP_LOGE(TAG, "Invalid temperature reading: %.2f, returning last valid: %.2f", temp, last_valid_temp);
                }
            } else {
                ESP_LOGE(TAG, "Sensor not responding, retrying...");
            }
        }
        ESP_LOGE(TAG, "Failed to get valid temperature after retries, returning last valid: %.2f", last_valid_temp);
        return last_valid_temp; // Return last valid temperature if all retries fail
    } else {
        ESP_LOGE(TAG, "Sensor not initialized, returning last valid: %.2f", last_valid_temp);
        return last_valid_temp;
    }
}

// Initializes the DS18B20 sensor
void ds18b20_init(int GPIO) {
    DS_GPIO = GPIO;
    gpio_reset_pin(DS_GPIO); // Replaces gpio_pad_select_gpio
    gpio_set_direction(DS_GPIO, GPIO_MODE_OUTPUT);
    init = 1;
    ESP_LOGI(TAG, "DS18B20 initialized on GPIO %d", DS_GPIO);
}

// Calculate CRC8 for data validation
unsigned char ds18b20_crc8(const unsigned char *data, int len) {
    unsigned char crc = 0;
    for (int i = 0; i < len; i++) {
        unsigned char inbyte = data[i];
        for (int j = 0; j < 8; j++) {
            unsigned char mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if (mix) crc ^= 0x8C;
            inbyte >>= 1;
        }
    }
    return crc;
}
