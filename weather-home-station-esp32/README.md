# Home Weather Station

In this project, we will build a simple home weather station using the ESP32 microcontroller and DHT11 temperature and humidity sensor. This project will allow you to monitor the temperature and humidity in your home and display the data on a web page. We will also have a simple web server with a simple interface that will allows us to update the firmware over the air (OTA).

## Components Required

- ESP32 development board
- DHT11 temperature and humidity sensor
- Jumper wires
- Breadboard (optional)
- USB cable for programming the ESP32

## Part 1: RGB LED Control

### Wiring Diagram

<img src="./assets/img/Schematic.svg" alt="Wiring Diagram" width="600" height="400">

### Code 

1. `rgb_led.h`

   - This header containds the definitions for the RGB LED GPIO pins and the channel number.

   ```c
   // RGB LED GPIOs
    #define RGB_LED_RED_GPIO 21
    #define RGB_LED_GREEN_GPIO 22
    #define RGB_LED_BLUE_GPIO 23

    // RGB LED color mix channel
    #define RGB_LED_CHANNEL_NUM 3
   ```

   - This header also contains the structure definition for the RGB LED configuration.

   ```c
   // RGB LED configuration

## Part 3: HTTP Server

This section implements the HTTP server functionality for the ESP32 weather station. The server provides a web interface for monitoring the station's data and managing firmware updates over the air (OTA). The implementation uses FreeRTOS tasks for server monitoring and event handling.

### Task Configuration

The HTTP server uses the following FreeRTOS task configurations (defined in `tasks_common.h`):

```c
// HTTP Server task
#define HTTP_SERVER_TASK_STACK_SIZE         8192
#define HTTP_SERVER_TASK_PRIORITY           4
#define HTTP_SERVER_TASK_CORE_ID            0

// HTTP Server Monitor task
#define HTTP_SERVER_MONITOR_STACK_SIZE      4096
#define HTTP_SERVER_MONITOR_PRIORITY        3
#define HTTP_SERVER_MONITOR_CORE_ID         0
```

### `http_server.c` and `http_server.h` - HTTP Server Logic

These files implement the HTTP server and its monitor, providing a web interface for the ESP32 weather station. The server serves static files (HTML, CSS, JS, favicon, JQuery) and handles OTA update and WiFi connection events via a message queue.

#### Key Features

- **HTTP Server Initialization and Control:**
  - `http_server_start()`: Starts the HTTP server if not already running.
  - `http_server_stop()`: Stops the HTTP server and its monitor task.
  - `http_server_configure()`: Configures the HTTP server, sets up URI handlers, and creates the monitor task and queue.

- **Static File Serving:**
  - Serves embedded static files for the web interface (index.html, app.css, app.js, JQuery, favicon.ico) using dedicated URI handlers.

- **Monitor Task and Queue:**
  - `http_server_monitor()`: FreeRTOS task that processes messages from the HTTP server queue, logging events such as WiFi connection attempts and OTA update results.
  - `http_server_monitor_send_message()`: Sends messages to the HTTP server monitor queue for asynchronous event handling.

#### Function Reference (`http_server.c` and `http_server.h`)

- **`http_server_start(void)`:**
  Starts the HTTP server if not already running.

- **`http_server_stop(void)`:**
  Stops the HTTP server and its monitor task.

- **`http_server_monitor_send_message(http_server_message_e msgID)`:**
  Sends a message to the HTTP server monitor queue for event-driven processing.

- **`http_server_monitor(void *pvParameters)`:**
  Static. FreeRTOS task that processes messages from the HTTP server queue, logging WiFi and OTA events.

- **`http_server_configure(void)`:**
  Static. Configures the HTTP server, sets up URI handlers for static files, and creates the monitor task and queue.

- **Static URI Handlers:**
  - `http_server_jquery_handler`, `http_server_index_html_handler`, `http_server_app_css_handler`, `http_server_app_js_handler`, `http_server_favicon_ico_handler`: Serve the respective embedded static files.

### Message Queue Structure

The HTTP server uses a message queue for event handling, defined in `http_server.h`:

```c
/**
 * Messages for the HTTP monitor.
 */
typedef enum http_server_message
{
    HTTP_MSG_WIFI_CONNECT_INIT = 0,
    HTTP_MSG_WIFI_CONNECT_SUCCESS,
    HTTP_MSG_WIFI_CONNECT_FAIL,
    HTTP_MSG_OTA_UPDATE_SUCCESSFUL,
    HTTP_MSG_OTA_UPDATE_FAILED,
    HTTP_MSG_OTA_UPDATE_INITIALIZED,
} http_server_message_e;

/**
 * Structure for the message queue.
*/
typedef struct http_server_queue_message
{
    http_server_message_e msgID;
} http_server_queue_message_t;
```

### Integration with WiFi Application

The HTTP server is started by the WiFi application when it receives the `WIFI_APP_MSG_START_HTTP_SERVER` message. This integration ensures that the server only starts after the WiFi setup is complete. The status of both the WiFi connection and HTTP server is indicated through the RGB LED:

- Purple: WiFi application started
- Orange: HTTP server started
- Green: WiFi connected

### Design Notes

- The HTTP server is tightly integrated with the WiFi application, allowing for real-time status updates and OTA firmware updates via the web interface.
- The use of a monitor task and message queue enables asynchronous event handling and decouples HTTP server logic from other application components.
- Static file serving is handled efficiently using embedded binary data and dedicated URI handlers.

### OTA (Over-The-Air) Update Feature

The HTTP server includes robust OTA update functionality that allows firmware updates through the web interface.

#### Key OTA Components

- **Update Status Tracking:**
  - `g_fw_update_status`: Global variable tracking the firmware update status (pending, successful, or failed)
  - Status codes: `OTA_UPDATE_PENDING (0)`, `OTA_UPDATE_SUCCESSFUL (1)`, `OTA_UPDATE_FAILED (-1)`

- **OTA Handlers:**
  - `http_server_OTA_update_handler`: Receives and processes the .bin file upload for firmware updates
  - `http_server_OTA_status_handler`: Provides firmware update status and compilation information

- **Reset Timer:**
  - Automatically restarts the ESP32 after a successful update
  - Configured through `fw_update_reset_args` with the `http_server_fw_update_reset_callback`

#### OTA Update Process

1. **File Upload:**
   - Binary firmware file (.bin) is uploaded through the web interface
   - Server receives and processes the file in chunks
   - OTA handle manages the update partition writing

2. **Validation and Installation:**
   - Validates received firmware data
   - Writes to the next available OTA partition
   - Updates status through the monitor queue

3. **Status Reporting:**
   - Real-time status updates via `http_server_OTA_status_handler`
   - Provides compilation time/date information
   - Returns JSON-formatted status response

4. **Post-Update Actions:**
   - Automatic ESP32 restart on successful update
   - Error handling for failed updates
   - Status LED indicators for update progress

Refer to the source code in `main/http_server.c` and `main/http_server.h` for further details and implementation specifics.


## Part 2: WiFi - SoftAp

## `rgb_led.c` and `rgb_led.h` - RGB LED Control Logic

These files implement the RGB LED control logic, providing functions to initialize the LED hardware and set its color to indicate different system/application states. The implementation uses the ESP32's LEDC (LED Controller) peripheral for PWM-based color mixing.

### Key Features

  - Defines a configuration structure (`ledc_info_t`) for each color channel (red, green, blue), including channel, GPIO, mode, and timer index.
  - Uses macros to specify the GPIO pins and number of channels for the RGB LED.

  - The static function `rgb_led_pmw_init()` sets up the LEDC timer and configures each color channel for PWM output. It ensures the timer and channels are only initialized once per boot.

  - The static function `rgb_led_set_color(uint8_t red, uint8_t green, uint8_t blue)` sets the duty cycle for each color channel, allowing for 8-bit color mixing (0-255 per channel).

  - `rgb_led_wifi_app_started()`: Sets the LED to a specific color to indicate the WiFi application has started.
  - `rgb_led_http_server_started()`: Sets the LED to a different color to indicate the HTTP server has started.
  - `rgb_led_wifi_connected()`: Sets the LED to another color to indicate a successful WiFi connection.
  - Each of these functions ensures the LED is initialized before setting the color.

### Design Notes

- The use of static functions for initialization and color setting ensures encapsulation and prevents accidental re-initialization or misuse.
- The color values for each status are chosen for clear visual distinction and can be customized as needed.
- The header file (`rgb_led.h`) provides only the public API for status indication, hiding the lower-level details from other modules.

Refer to the source code in `main/rgb_led.c` and `main/rgb_led.h` for further details and implementation specifics.


## `wifi_app.c` - WiFi Application Logic
The `wifi_app.c` file contains the main logic for initializing, configuring, and managing the WiFi application on the ESP32. It handles WiFi and IP events, manages the FreeRTOS task and message queue for WiFi operations, and controls the RGB LED to indicate system status. Below is an overview of its structure and functionality:

### Key Features


- **`wifi_app_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)`**:
  Handles WiFi and IP events. Logs event types such as AP start/stop, station connect/disconnect, and IP acquisition. Can be extended to trigger additional actions (e.g., LED updates or queue messages) based on event type.
- **`wifi_app_event_handler_init()`:**
  Registers the event handler for both WiFi and IP events using ESP-IDF's event system. Ensures all relevant events are captured and processed by the application.

- **`wifi_app_default_wifi_init()`:**
  Initializes the TCP/IP stack and configures the default WiFi settings. Creates network interfaces for both station and access point modes, preparing the ESP32 for dual-mode operation.
- **`wifi_app_soft_ap_config()`:**
  Configures the ESP32 as a WiFi SoftAP (Access Point). Sets up the SSID, password, channel, visibility, authentication mode, and beacon interval. Assigns a static IP, gateway, and netmask, and starts the DHCP server for client devices.
- **`wifi_app_task(void *pvParameters)`:**
  The main FreeRTOS task for the WiFi application. Initializes event handling, network stack, and SoftAP configuration, then starts the WiFi driver. Sends an initial message to start the HTTP server. Enters a loop to process messages from the queue, handling events such as HTTP server start, connection attempts, and successful connections (with corresponding LED updates).
- **`wifi_app_send_message(wifi_app_message_e msgID)`:**
  Sends a message to the WiFi application's FreeRTOS queue. Used for asynchronous, event-driven communication between different parts of the application (e.g., from event handlers to the main task).
- **`wifi_app_start()`:**
  Entry point for starting the WiFi application. Sets the initial LED color, disables default WiFi logging, creates the message queue, and starts the main WiFi application task pinned to a specific core.
  - Implements a static event handler (`wifi_app_event_handler`) for WiFi and IP events, such as AP start/stop, station connect/disconnect, and IP acquisition.
  - Logs each event and can trigger actions (e.g., LED color changes) based on the event type.
- **Initialization Functions:**
  - `wifi_app_event_handler_init()`: Registers the event handler for WiFi and IP events.
  - `wifi_app_soft_ap_config()`: Configures the ESP32 as a WiFi SoftAP, sets static IP, gateway, and netmask, and starts the DHCP server for clients.

- **Task and Queue Management:**
  - Defines a FreeRTOS task (`wifi_app_task`) that initializes the WiFi system, starts the WiFi driver, and processes messages from a queue to handle application-level events.

## `http_server.c` and `http_server.h` - HTTP Server Logic

These files implement the HTTP server and its monitor, providing a web interface for the ESP32 weather station. The server serves static files (HTML, CSS, JS, favicon, JQuery) and handles OTA update and WiFi connection events via a message queue.

### Key Features

- **HTTP Server Initialization and Control:**
  - `http_server_start()`: Starts the HTTP server if not already running.
  - `http_server_stop()`: Stops the HTTP server and its monitor task.
  - `http_server_configure()`: Configures the HTTP server, sets up URI handlers, and creates the monitor task and queue.

- **Static File Serving:**
  - Serves embedded static files for the web interface (index.html, app.css, app.js, JQuery, favicon.ico) using dedicated URI handlers.

- **Monitor Task and Queue:**
  - `http_server_monitor()`: FreeRTOS task that processes messages from the HTTP server queue, logging events such as WiFi connection attempts and OTA update results.
  - `http_server_monitor_send_message()`: Sends messages to the HTTP server monitor queue for asynchronous event handling.

### Function Reference (`http_server.c` and `http_server.h`)

- **`http_server_start(void)`:**
  Starts the HTTP server if not already running.

- **`http_server_stop(void)`:**
  Stops the HTTP server and its monitor task.

- **`http_server_monitor_send_message(http_server_message_e msgID)`:**
  Sends a message to the HTTP server monitor queue for event-driven processing.

- **`http_server_monitor(void *pvParameters)`:**
  Static. FreeRTOS task that processes messages from the HTTP server queue, logging WiFi and OTA events.

- **`http_server_configure(void)`:**
  Static. Configures the HTTP server, sets up URI handlers for static files, and creates the monitor task and queue.

- **Static URI Handlers:**
  - `http_server_jquery_handler`, `http_server_index_html_handler`, `http_server_app_css_handler`, `http_server_app_js_handler`, `http_server_favicon_ico_handler`: Serve the respective embedded static files.

### Design Notes

- The HTTP server is tightly integrated with the WiFi application, allowing for real-time status updates and OTA firmware updates via the web interface.
- The use of a monitor task and message queue enables asynchronous event handling and decouples HTTP server logic from other application components.
- Static file serving is handled efficiently using embedded binary data and dedicated URI handlers.

Refer to the source code in `main/http_server.c` and `main/http_server.h` for further details and implementation specifics.

- **LED Status Indication:**
  - Calls functions from `rgb_led.c` to set the RGB LED color based on the current WiFi application state (e.g., app started, HTTP server started, WiFi connected).

- **Public API:**
  - `wifi_app_send_message()`: Sends messages to the WiFi application queue for event-driven processing.
  - `wifi_app_start()`: Initializes the application, sets up the LED, disables default WiFi logging, creates the message queue, and starts the main WiFi application task.

### Example Flow

1. **Startup:** `wifi_app_start()` is called, which sets the initial LED color, disables verbose WiFi logs, creates the message queue, and starts the main task.
2. **Task Execution:** The main task (`wifi_app_task`) initializes the event handler, network stack, and SoftAP configuration, then starts the WiFi driver.
3. **Event Handling:** As WiFi and IP events occur, the registered event handler logs them and can trigger further actions (e.g., sending messages to the queue).
4. **Queue Processing:** The main task waits for messages (such as starting the HTTP server or indicating a successful connection) and updates the LED or performs other actions accordingly.

This modular approach allows for easy expansion of WiFi-related features and robust event-driven management of the ESP32's networking capabilities.

### Function Reference (`wifi_app.c`)

- **`wifi_app_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)`:**
  Handles WiFi and IP events. Logs event types such as AP start/stop, station connect/disconnect, and IP acquisition. Can be extended to trigger additional actions (e.g., LED updates or queue messages) based on event type.

- **`wifi_app_event_handler_init()`:**
  Registers the event handler for both WiFi and IP events using ESP-IDF's event system. Ensures all relevant events are captured and processed by the application.

- **`wifi_app_default_wifi_init()`:**
  Initializes the TCP/IP stack and configures the default WiFi settings. Creates network interfaces for both station and access point modes, preparing the ESP32 for dual-mode operation.

- **`wifi_app_soft_ap_config()`:**
  Configures the ESP32 as a WiFi SoftAP (Access Point). Sets up the SSID, password, channel, visibility, authentication mode, and beacon interval. Assigns a static IP, gateway, and netmask, and starts the DHCP server for client devices.

- **`wifi_app_task(void *pvParameters)`:**
  The main FreeRTOS task for the WiFi application. Initializes event handling, network stack, and SoftAP configuration, then starts the WiFi driver. Sends an initial message to start the HTTP server. Enters a loop to process messages from the queue, handling events such as HTTP server start, connection attempts, and successful connections (with corresponding LED updates).

- **`wifi_app_send_message(wifi_app_message_e msgID)`:**
  Sends a message to the WiFi application's FreeRTOS queue. Used for asynchronous, event-driven communication between different parts of the application (e.g., from event handlers to the main task).

- **`wifi_app_start()`:**
  Entry point for starting the WiFi application. Sets the initial LED color, disables default WiFi logging, creates the message queue, and starts the main WiFi application task pinned to a specific core.

### Design Notes

- The use of FreeRTOS tasks and queues allows for responsive, event-driven application logic, making it easy to expand with new features (such as OTA updates or web server endpoints).
- The modular structure separates event handling, initialization, and application logic, improving maintainability and clarity.
- LED status indication provides immediate visual feedback for key application states, aiding in debugging and user experience.

Refer to the source code in `main/wifi_app.c` for further details and implementation specifics.

1. `tasks_common.h`

   - This header file contains common definitions for the FreeRTOS tasks, including stack size, priority, and core ID for the WiFi application task.

   ```c
   #ifndef MAIN_TASKS_COMMON_H_
   #define MAIN_TASKS_COMMON_H_

   // WiFi application task
   #define WIFI_APP_TASK_STACK_SIZE            4096
   #define WIFI_APP_TASK_PRIORITY              5
   #define WIFI_APP_TASK_CORE_ID               0

   #endif /* MAIN_TASKS_COMMON_H_ */
   ```

2. `wifi_app.h`

    - This header file defines the WiFi application settings
    ```c
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
    ```

    - The header file also contains the function prototypes for initializing the WiFi application and handling WiFi events.

    ```c
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
    ```

    - We also define an enum for the WiFi application messages that can be sent to the queue.

    ```c
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
    ```





