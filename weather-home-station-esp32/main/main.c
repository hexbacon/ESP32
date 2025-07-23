/**
 * Application entry point.
 */

#include "nvs_flash.h"
#include "wifi_app.h"
#include "DHT11.h"
#include "esp_log.h"
#include "rgb_led.h"
void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Start WiFi
    wifi_app_start();

    dht11_t sensor;
    dht11_init(&sensor, 4);
    /* Block for 1min. */
    const TickType_t xDelay = 60000 / portTICK_PERIOD_MS;
    vTaskDelay(pdMS_TO_TICKS(2000));
    while (1) 
    {
        if (dht11_read(&sensor) == ESP_OK) 
        {
            ESP_LOGI("DHT11", "Temperature: %d°C, Humidity: %d%%",
                     dht11_get_temperature(&sensor),
                     dht11_get_humidity(&sensor));
        } else 
        {
            ESP_LOGI("DHT11", "Failed to read from sensor");
            rgb_led_error();
        }
        vTaskDelay(xDelay);
    }
}