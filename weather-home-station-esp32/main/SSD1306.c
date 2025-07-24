/*
 * SSH1306.c
 *
 *  Created on: Jul 24, 2025
 *      Author: christophermena
 */

#include "SSD1306.h"

static const char TAG [] = "SSD1306";
static i2c_master_bus_handle_t i2c_bus = NULL;
static esp_lcd_panel_io_handle_t io_handle = NULL;


static esp_err_t ssd1306_init_i2c(void)
{
    ESP_LOGI(TAG, "Initialize I2C bus");
    i2c_master_bus_config_t bus_config = 
    {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .i2c_port = I2C_HOST,
        .sda_io_num = LCD_PIN_NUM_SDA,
        .scl_io_num = LCD_PIN_NUM_SCL,
        .flags.enable_internal_pullup = true,
    };
    esp_err_t err = i2c_new_master_bus(&bus_config, &i2c_bus);
    if (err == ESP_OK) return ESP_OK;
    else return ESP_FAIL;
}

static esp_err_t ssd1306_intall_io_panel(void)
{
    esp_lcd_panel_io_i2c_config_t io_config = 
    {
        
    };
    return ESP_FAIL;
}

void ssd1306_init(void)
{
    // Initialize I2C bus
    if (ssd1306_init_i2c() != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize I2C bus");
        return;
    }

    // Install panel IO
}