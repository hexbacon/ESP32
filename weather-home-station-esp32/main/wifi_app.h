/*
 * rgb_led.h
 *
 *  Created on: Jul 15, 2025
 *      Author: christophermena
 */

#ifndef MAIN_WIFI_APP_H_
#define MAIN_WIFI_APP_H_

#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
// WiFi application settings
#define WIFI_AP_SSID                "ESP32"             // AP name
#define WIFI_AP_PASSWORD            "password"          // AP password
#define WIFI_AP_CHANNEL             1                   // AP channe;
#define WIFI_AP_SSID_HIDDEN         0                   // AP visibility
#define WIFI_AP_MAX_CONNECTIONS     5                   // AP max connections
#define WIFI_AP_BEACON_INTERVAL     100                 // AP beacon: 100ms as recommend
#define WIFI_AP_IP                  "192.168.0.1"       // AP default ip
#define WIFI_AP_GATEWAY             "192.168.0.1"       // AP default gatewate (should be the same as the IP)
#define WIFI_AP_NETMASK             "255.255.2255.0"    // AP netmask
#define WIFI_AP_BANDWIDTH           WIFI_BW_HT20        // AP bandwidth 20 MHz (other option is 40 MHz) 
#define WIFI_STA_POWER_SAVE         WIFI_PS_NONE        // Power save is not used
#define MAX_SSID_LENGTH             32                  // IEEE stadard maximum
#define MAX_PASSWORD_LENGTH         64                  // IEEE stadard maximum
#define MAX_CONNECTION_RETRIES      5                   // Retry number on disconnect

// netif object for the Station and Access Point
extern esp_netif_t* esp_netif_sta;
extern esp_netif_t* esp_netif_ap;

/**
 * Message IDs fot the WiFi application task
 * @note Expand this based on thee application requirements
 */
typedef enum wifi_app_message
{
    WIFI_APP_MSG_START_HTTP_SERVER = 0,
    WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER,
    WIFI_APP_MSG_STA_CONNECTED_GOT_IP,

} wifi_app_message_e;

/**
 * Structure for the message queque
 * @note Expand this based on the application requirements e.g. add another type and parameters as required
 */
typedef struct  wifi_app_queue_message
{
    wifi_app_message_e msgID;
} wifi_app_queue_message_t;

/**
 * Sends a message to the queque
 * @param msgID message ID from the wifi_app_message_e enum.
 * @return pdTRUE if an item was succesfully sent to the queue, otherwise pdFALSE.
 * @note Expand the parameter list based on  your requiredments e.g. how you've expanded the wifi_app_queue_message_t
 */
BaseType_t wifi_app_send_message(wifi_app_message_e msgID);

/**
 * Starts the WiFi RROS task
 */
void wifi_app_start(void);


#endif /* MAIN_WIFI_APP_H_ */