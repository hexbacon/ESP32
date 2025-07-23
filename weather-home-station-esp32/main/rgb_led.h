/*
 * rgb_led.h
 *
 *  Created on: Jul 15, 2025
 *      Author: christophermena
 */

#ifndef MAIN_RGB_LED_H_
#define MAIN_RGB_LED_H_

// RGB LED GPIOs
#define RGB_LED_RED_GPIO 21
#define RGB_LED_GREEN_GPIO 22
#define RGB_LED_BLUE_GPIO 23

// RGB LED color mix channel
#define RGB_LED_CHANNEL_NUM 3

// BLINK
#define RED_BLINK_ON_DUTY   255
#define RED_BLINK_OFF_DUTY  0
#define RED_BLINK_DELAY_MS  1000

// RGB LED configuration
typedef struct {
	int channel;
	int gpio;
	int mode;
	int timer_index;
} ledc_info_t;

/**
 * @brief Color to indicate WiFi application has started.
 */
void rgb_led_wifi_app_started(void);

/**
 * @brief Color to indicate HTTP server has started.
 */
void rgb_led_http_server_started(void);

/**
 * @brief Color to indicate that the EPS32 is connected to an access point.
 */
void rgb_led_wifi_connected(void);

/**
 * @brief Color to indicate that the EPS32 had an error.
 */
void rgb_led_dht11_started(void);

/**
 * @brief Color to indicate that the EPS32 had an error.
 */
void rgb_led_error(void);

/**
 * @brief Color to indicate DHT11 reading
 */
void rgb_led_dht11_read(void);
#endif /* MAIN_RGB_LED_H_ */