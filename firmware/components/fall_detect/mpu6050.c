#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "i2c_bus.h"
#include "mpu6050.h"

static const char *TAG = "mpu6050";

#define MPU_ADDR          0x68
#define REG_SMPLRT_DIV    0x19
#define REG_CONFIG        0x1A
#define REG_ACCEL_CONFIG  0x1C
#define REG_ACCEL_XOUT_H  0x3B
#define REG_PWR_MGMT_1    0x6B
#define REG_WHO_AM_I      0x75

#define ACCEL_LSB_PER_G   4096.0f   // +/-8 g

static i2c_master_dev_handle_t s_dev;

esp_err_t mpu6050_init(void)
{
    esp_err_t err = i2c_bus_probe(MPU_ADDR);
    if (err != ESP_OK) {
        return err;
    }
    err = i2c_bus_add_device(MPU_ADDR, &s_dev);
    if (err != ESP_OK) {
        return err;
    }

    uint8_t who = 0;
    err = i2c_bus_read_regs(s_dev, REG_WHO_AM_I, &who, 1);
    if (err != ESP_OK) {
        return err;
    }
    if (who == 0x68 || who == 0x70 || who == 0x72 || who == 0x74) {
        ESP_LOGI(TAG, "WHO_AM_I = 0x%02X (MPU-6050 or compatible)", who);
    } else {
        ESP_LOGW(TAG, "unexpected WHO_AM_I 0x%02X - continuing anyway", who);
    }

    err = i2c_bus_write_reg(s_dev, REG_PWR_MGMT_1, 0x00);       // wake from sleep
    if (err != ESP_OK) {
        return err;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
    if ((err = i2c_bus_write_reg(s_dev, REG_SMPLRT_DIV, 9)) != ESP_OK ||        // 1 kHz/(1+9) = 100 Hz
        (err = i2c_bus_write_reg(s_dev, REG_CONFIG, 0x03)) != ESP_OK ||         // DLPF ~44 Hz
        (err = i2c_bus_write_reg(s_dev, REG_ACCEL_CONFIG, 0x10)) != ESP_OK) {   // +/-8 g
        return err;
    }
    ESP_LOGI(TAG, "ready: accel +/-8 g, DLPF 44 Hz");
    return ESP_OK;
}

esp_err_t mpu6050_read_accel_g(float *x, float *y, float *z)
{
    uint8_t raw[6];
    esp_err_t err = i2c_bus_read_regs(s_dev, REG_ACCEL_XOUT_H, raw, sizeof(raw));
    if (err != ESP_OK) {
        return err;
    }
    *x = (int16_t)((raw[0] << 8) | raw[1]) / ACCEL_LSB_PER_G;
    *y = (int16_t)((raw[2] << 8) | raw[3]) / ACCEL_LSB_PER_G;
    *z = (int16_t)((raw[4] << 8) | raw[5]) / ACCEL_LSB_PER_G;
    return ESP_OK;
}
