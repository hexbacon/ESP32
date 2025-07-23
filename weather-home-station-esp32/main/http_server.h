/*
 * http_sever.h
 *
 *  Created on: Jul 16, 2025
 *      Author: christophermena
 */

#ifndef MAIN_HTTP_SERVER_H_
#define MAIN_HTTP_SERVER_H_

#include "freertos/FreeRTOS.h"

#define OTA_UPDATE_PENDING      0
#define OTA_UPDATE_SUCCESSFUL   1
#define OTA_UPDATE_FAILED       -1

/**
 * @brief Messages for the HTTP monitor.
 */
typedef enum http_server_message
{
    HTTP_MSG_WIFI_CONNECT_INIT = 0,
    HTTP_MSG_WIFI_CONNECT_SUCCESS,
    HTTP_MSG_WIFI_CONNECT_FAIL,
    HTTP_MSG_OTA_UPDATE_SUCCESSFUL,
    HTTP_MSG_OTA_UPDATE_FAILED,
} http_server_message_e;

/**
 * @brief Structure for the message queue.
*/
typedef struct http_server_queue_message
{
    http_server_message_e msgID;
} http_server_queue_message_t;

/**
 * @brief Sends a message to the queue
 * @param msgID message ID from the http_server_message_e enum.
 * @return pdTRUE if an item was succesfully sent to the queue, otherwise pdFALSE.
 * @note Expand the parameters liszt based on your requirements.
 */
BaseType_t http_server_monitor_send_message(http_server_message_e msgID);

/**
 * @brief Starts the HTTP server.
 */
void http_server_start(void);

/**
 * @brief Stops the HTTP server.
 */
void http_server_stop(void);

/**
 * @brief Timer callback function which calls esp_restart upon successful firmware update.
 */
void http_server_fw_update_reset_callback(void* args);

#endif /* MAIN_HTTP_SERVER_H_ */