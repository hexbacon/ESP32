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

// RGB LED configuration
typedef struct {
	int channel;
	int gpio;
	int mode;
	int timer_index;
} ledc_info_t;

/**
 * Color to indicate WiFi application has started.
 */
void rgb_led_wifi_app_started(void);

/**
 * Color to indicate HTTP server has started.
 */
void rgb_led_http_server_started(void);

/**
 * Color to indicate that the EPS32 is connected to an access point.
 */
void rgb_led_wifi_connected(void);

#endif /* MAIN_RGB_LED_H_ */