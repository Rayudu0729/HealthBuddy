#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
void app_main(void) {
    i2c_master_bus_handle_t bus;
    i2c_master_dev_handle_t sensor;
    i2c_master_bus_config_t cfg = {
        .i2c_port = I2C_NUM_0, .sda_io_num = 5, .scl_io_num = 6,
        .clk_source = I2C_CLK_SRC_DEFAULT, .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&cfg, &bus));
    i2c_device_config_t dev = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x29, .scl_speed_hz = 50000
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus, &dev, &sensor));
    vTaskDelay(pdMS_TO_TICKS(500));
    ESP_LOGI("TOF_CHECK", "Communication-only test: SDA=D4/GPIO5, SCL=D5/GPIO6, ID read at 50 kHz; no ranging or calibration");
    while (1) {
        ESP_LOGI("TOF_CHECK", "Idle digital levels: SDA=%d, SCL=%d (1=high; not a voltage measurement)", gpio_get_level(GPIO_NUM_5), gpio_get_level(GPIO_NUM_6));
        esp_err_t err = i2c_master_probe(bus, 0x29, 200);
        if (err != ESP_OK) {
            ESP_LOGE("TOF_CHECK", "No response at 0x29: %s", esp_err_to_name(err));
            unsigned found = 0, bus_errors = 0;
            for (uint16_t addr = 0x08; addr <= 0x77; ++addr) {
                esp_err_t probe = i2c_master_probe(bus, addr, 20);
                if (probe == ESP_OK) {
                    ESP_LOGI("TOF_SCAN", "Device acknowledges at 0x%02X", (unsigned)addr);
                    ++found;
                } else if (probe != ESP_ERR_NOT_FOUND) {
                    ++bus_errors;
                    ESP_LOGW("TOF_SCAN", "Address 0x%02X: %s", (unsigned)addr, esp_err_to_name(probe));
                }
            }
            ESP_LOGI("TOF_SCAN", "Scan complete: %u device(s), %u bus error(s)", found, bus_errors);
        } else {
            ESP_LOGI("TOF_CHECK", "Address 0x29 acknowledges");
        }
        {
            ESP_LOGI("TOF_CHECK", "Attempting ID read at 50 kHz regardless of 100 kHz probe result");
            uint8_t reg[] = {0x01, 0x0F}, id[2] = {0};
            err = i2c_master_transmit_receive(sensor, reg, sizeof(reg), id, sizeof(id), 200);
            if (err == ESP_OK) {
                ESP_LOGI("TOF_CHECK", "Sensor ID: 0x%02X%02X (expected VL53L1X: 0xEACC)", id[0], id[1]);
            } else {
                ESP_LOGE("TOF_CHECK", "Read ID failed: %s", esp_err_to_name(err));
            }
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
