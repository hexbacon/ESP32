
#include "freertos/FreeRTOS.h"
#include "rgb_led.h"
void app_main(void)
{
    while (true) 
	{
        rgb_led_wifi_app_started();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        rgb_led_http_server_started();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        rgb_led_wifi_connected();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}