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

   - This header contains the definitions for the RGB LED GPIO pins and the channel number.

   ```c
   // RGB LED GPIOs
    #define RGB_LED_RED_GPIO 21
    #define RGB_LED_GREEN_GPIO 22
    #define RGB_LED_BLUE_GPIO 23

    // RGB LED color mix channel
    #define RGB_LED_CHANNEL_NUM 3
   ```

   - This header also contains the structure definition for the RGB LED configuration and blink parameters.

   ```c
   // BLINK parameters
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
   ```

#### Function Reference (`rgb_led.c` and `rgb_led.h`)

- **`rgb_led_wifi_app_started(void)`:**
  Sets LED to magenta/purple (255, 102, 255) to indicate WiFi application has started.

- **`rgb_led_http_server_started(void)`:**
  Sets LED to orange (204, 102, 51) to indicate HTTP server has started.

- **`rgb_led_wifi_connected(void)`:**
  Sets LED to green (0, 255, 153) to indicate successful WiFi connection.

- **`rgb_led_dht11_started(void)`:**
  Sets LED to teal (32, 66, 63) to indicate DHT11 sensor initialization.

- **`rgb_led_error(void)`:**
  Creates a blinking red LED task to indicate system errors. Uses FreeRTOS task with DHT sensor task parameters.

- **`rgb_led_dht11_read(void)`:**
  Sets LED to light blue (147, 251, 255) to indicate DHT11 sensor reading operation.

#### Static Helper Functions

- **`rgb_led_pmw_init(void)`:**
  Initializes the LEDC (LED Controller) peripheral for PWM-based color mixing. Configures timer and channels for RGB control.

- **`rgb_led_set_color(uint8_t red, uint8_t green, uint8_t blue)`:**
  Sets the RGB LED color using 8-bit values (0-255) for each color channel.

- **`red_led_task(void *pvParameters)`:**
  FreeRTOS task that creates a blinking red LED pattern for error indication. Toggles red LED on/off with configurable delay.

### LED Status Indicators

The RGB LED provides visual feedback for different system states:

- **Magenta/Purple**: WiFi application started
- **Orange**: HTTP server started  
- **Green**: WiFi connected successfully
- **Teal**: DHT11 sensor initialized
- **Blinking Red**: System error detected
- **Light Blue**: DHT11 sensor reading in progress


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

// DHT11 Sensor task
#define DHT_SENSOR_TASK_STACK_SIZE          4096
#define DHT_SENSOR_TASK_PRIORITY            2
#define DHT_SENSOR_TASK__CORE_ID            0
```

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

- **`http_server_fw_reset_timer(void)`:**
  Checks firmware update status and creates reset timer for successful updates.

- **`http_server_fw_update_reset_callback(void* args)`:**
  Timer callback function that restarts the ESP32 after successful firmware update.

#### OTA Update Handlers

- **`http_server_OTA_update_handler(httpd_req_t *req)`:**
  Handles firmware binary (.bin) file uploads. Processes multipart form data, writes to OTA partition, and validates the update.

- **`http_server_OTA_status_handler(httpd_req_t *req)`:**
  Returns JSON response with current OTA update status and firmware compilation information.

#### Static File Handlers

- **`http_server_jquery_handler(httpd_req_t *req)`:**
  Serves embedded jQuery library file.

- **`http_server_index_html_handler(httpd_req_t *req)`:**
  Serves the main web interface HTML file.

- **`http_server_app_css_handler(httpd_req_t *req)`:**
  Serves the application CSS stylesheet.

- **`http_server_app_js_handler(httpd_req_t *req)`:**
  Serves the application JavaScript file.

- **`http_server_favicon_ico_handler(httpd_req_t *req)`:**
  Serves the website favicon icon.

### Integration with Main Application

The DHT11 sensor is integrated into the main application loop with:

- **60-second reading interval**: Prevents over-polling the sensor using `xDelay` variable
- **RGB LED status indication**: 
  - Shows sensor errors with blinking red LED via `rgb_led_error()`
  - Indicates successful readings with appropriate color changes
- **Automatic retry**: Continues operation even after read failures
- **Logging**: Provides detailed information about sensor readings and errors using ESP_LOGI

### Main Application Integration

The `main.c` file demonstrates the integration of all components:

```c
void app_main(void)
{
    // Initialize NVS for WiFi credentials storage
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Start WiFi application (includes HTTP server startup)
    wifi_app_start();

    // Initialize and monitor DHT11 sensor
    dht11_t sensor;
    dht11_init(&sensor, 4);
    const TickType_t xDelay = 60000 / portTICK_PERIOD_MS;  // 60-second intervals
    vTaskDelay(pdMS_TO_TICKS(2000));  // Initial delay

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
            rgb_led_error();  // Activate error LED indication
        }
        vTaskDelay(xDelay);  // Wait 60 seconds between readings
    }
}
```

### Design Notes

- The application uses a modular approach with separate tasks for WiFi, HTTP server, and sensor monitoring
- Visual feedback through RGB LED helps with debugging and status monitoring
- The 60-second sensor reading interval prevents DHT11 overheating and ensures reliable operation
- Error handling includes both logging and visual indication for robust operation
- All components are integrated through FreeRTOS tasks and queues for responsive, event-driven operation


## Part 4: DHT11 Temperature and Humidity Sensor

The DHT11 sensor provides digital temperature and humidity readings through a single-wire communication protocol. The implementation includes proper timing control, error handling, and integration with the RGB LED status system.

### Hardware Configuration

- **GPIO Pin**: GPIO 4 (configurable via `DHT11_GPIO_SENSOR_PIN`)
- **Power**: 3.3V or 5V
- **Pull-up Resistor**: 4.7kΩ or 10kΩ between DATA and VCC (recommended)
- **Communication**: Single-wire digital protocol

### Key Features

- Single-wire communication protocol implementation
- Precise timing control using ESP32's high-resolution timer
- Automatic retry mechanism with configurable attempts
- Integration with RGB LED for visual status indication
- Checksum validation for data integrity
- Non-blocking operation with FreeRTOS task delays

### DHT11 Communication Protocol

The DHT11 uses a custom single-wire protocol:

1. **Start Signal**: MCU pulls line LOW for 20ms, then HIGH for 30μs
2. **Response Signal**: DHT11 pulls line LOW for 80μs, then HIGH for 80μs
3. **Data Transmission**: 40 bits (5 bytes) of data
   - Byte 0: Humidity integer part
   - Byte 1: Humidity decimal part (always 0 for DHT11)
   - Byte 2: Temperature integer part
   - Byte 3: Temperature decimal part (always 0 for DHT11)
   - Byte 4: Checksum (sum of bytes 0-3)

#### Function Reference (`DHT11.c` and `DHT11.h`)

- **`dht11_init(dht11_t *sensor, int gpio_num)`:**
  Initializes the DHT11 sensor with the specified GPIO pin. Configures the pin for input/output operation with pull-up enabled and sets initial state.

- **`dht11_read(dht11_t *sensor)`:**
  Performs a complete sensor reading cycle. Sends start signal, waits for sensor response, reads 40 data bits, validates checksum, and stores temperature/humidity values. Returns `ESP_OK` on success or error code on failure.

- **`dht11_get_temperature(dht11_t *sensor)`:**
  Returns the last successfully read temperature value in Celsius from the sensor structure.

- **`dht11_get_humidity(dht11_t *sensor)`:**
  Returns the last successfully read humidity value as a percentage from the sensor structure.

- **`dht11_celsius_to_fahrenheit(int celsius)`:**
  Utility function to convert Celsius temperature to Fahrenheit using the formula: F = (C * 9/5) + 32.

- **`dht11_get_sersor_temperature(void)`:**
  Global function that returns the last valid temperature reading without requiring a sensor structure reference. Returns -1 if no valid reading is available.

- **`dht11_get_sensor_humidity(void)`:**
  Global function that returns the last valid humidity reading without requiring a sensor structure reference. Returns -1 if no valid reading is available.

#### Static Helper Functions

- **`dht11_wait_for_level(int level, int timeout_us)`:**
  Waits for the GPIO pin to reach the specified level (0 or 1) within the given timeout period. Used during the communication protocol for synchronization.

- **`dht11_set_output(dht11_t *sensor)` / `dht11_set_input(dht11_t *sensor)`:**
  Configure the GPIO pin direction for output or input operation during different phases of communication.

### Data Structure

```c
typedef struct dht11 {
    int gpio_num;        // GPIO pin number
    int temperature;     // Temperature in Celsius
    int humidity;        // Humidity percentage
} dht11_t;
```

### Error Handling

The implementation includes comprehensive error handling:

- **Timeout Errors**: If the sensor doesn't respond within expected timeframes
- **Checksum Validation**: Ensures data integrity by validating the received checksum
- **Communication Errors**: Handles cases where the sensor doesn't follow the expected protocol
- **Retry Mechanism**: Automatically retries failed readings with exponential backoff

### Integration with Main Application

The DHT11 sensor is integrated into the main application loop with:

- **60-second reading interval**: Prevents over-polling the sensor
- **RGB LED status indication**: Shows sensor errors with red color
- **Automatic retry**: Continues operation even after read failures
- **Logging**: Provides detailed information about sensor readings and errors

### Timing Considerations

The DHT11 protocol requires precise timing:

- **Start Signal**: 20ms LOW, 30μs HIGH
- **Response Window**: 80μs LOW, 80μs HIGH from sensor
- **Data Bits**: Each bit represented by pulse width (30μs = 0, 70μs = 1)
- **Reading Interval**: Minimum 1 second between readings
- **Timeout Values**: Configurable via header file constants

### Design Notes

- The implementation uses ESP32's high-resolution timer (`esp_timer_get_time()`) for precise timing measurements
- GPIO direction is dynamically switched between input and output during communication
- The sensor requires a warm-up period after power-on before reliable readings can be obtained
- Data validation includes both protocol timing checks and checksum verification
- The implementation is thread-safe when used with proper FreeRTOS task scheduling

### Troubleshooting

Common issues and solutions:

1. **Timeout Errors**: Check hardware connections and pull-up resistor
2. **Checksum Failures**: Verify power supply stability and wire length
3. **Intermittent Readings**: Ensure adequate delays between readings
4. **No Response**: Check sensor power and GPIO pin configuration





