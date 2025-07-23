/*
 * DHT11.h
 *
 *  Created on: Jul 16, 2025
 *      Author: christophermena
 */

#ifndef MAIN_DHT11_H_
#define MAIN_DHT11_H_

#include "esp_err.h"

#define DHT11_GPIO_SENSOR_PIN               4
#define DHT11_START_SIGNAL_LOW_MS           20
#define DHT11_TIMEOUT                       100  // Increased timeout
#define DHT11_START_SIGNAL_TIMEOUT_US       2000  // Increased timeout

#include "esp_err.h"

/**
 * @brief Structure for the DHT.
 */
typedef struct dht11
{
    int gpio_num;
    int temperature; // in Celsius
    int humidity;    // in percentage
} dht11_t;

/**
 * @brief Initialize the DHT11 driver with the GPIO pin.
 */
void dht11_init(dht11_t *sensor, int gpio_num);

/**
 * @brief Read temperature and humidity from DHT11 sensor.
 */
esp_err_t dht11_read(dht11_t *sensor);

/**
 * @brief Get the last read temperature in Celsius.
 */
int dht11_get_temperature(dht11_t *sensor);

/**
 * @brief Get the last read humidity in percentage.
 */
int dht11_get_humidity(dht11_t *sensor);

/**
 * @brief Convert Celsius temperature to Fahrenheit.
 */
float dht11_celsius_to_fahrenheit(int celsius);

#endif /*MAIN_DHT11_H_*/