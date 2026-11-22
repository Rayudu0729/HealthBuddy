#pragma once

// Single shared I2C master bus (MPU-6050, DRV2605L, ...). The ESP-IDF driver
// locks the bus per transaction, so several tasks may use it safely.

#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"
#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

#define I2C_BUS_PORT        I2C_NUM_0
#define I2C_BUS_SDA_GPIO    5       // D4
#define I2C_BUS_SCL_GPIO    6       // D5
#define I2C_BUS_FREQ_HZ     100000
#define I2C_BUS_TIMEOUT_MS  20

esp_err_t i2c_bus_init(void);   // idempotent
esp_err_t i2c_bus_probe(uint8_t addr);
esp_err_t i2c_bus_add_device(uint8_t addr, i2c_master_dev_handle_t *out);
esp_err_t i2c_bus_write_reg(i2c_master_dev_handle_t dev, uint8_t reg, uint8_t val);
esp_err_t i2c_bus_read_regs(i2c_master_dev_handle_t dev, uint8_t reg, uint8_t *buf, size_t len);

#ifdef __cplusplus
}
#endif
