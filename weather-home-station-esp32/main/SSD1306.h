/*
 * SSH1306.h
 *
 *  Created on: Jul 24, 2025
 *      Author: christophermena
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_log.h"
//#include "lvgl.h"
//#include "esp_lvgl_port.h"

#define I2C_HOST                    0
#define LCD_PIXEL_CLOCK_HZ          (400 * 1000)
#define LCD_PIN_NUM_SDA             18
#define LCD_PIN_NUM_SCL             23
#define LCD_PIN_NUM_RST             -1
#define LCD_I2C_HW_ADDR             0x3C
#define LCD_H_RES                   128
#define LCD_V_RES                   64
#define LCD_CMD_BITS                8
#define LCD_PARAM_BITS              8


void ssdd1306_init(void);