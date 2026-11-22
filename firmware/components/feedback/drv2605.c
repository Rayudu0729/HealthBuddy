#include "esp_log.h"
#include "i2c_bus.h"
#include "drv2605.h"

static const char *TAG = "drv2605";

#define DRV_ADDR        0x5A
#define REG_STATUS      0x00
#define REG_MODE        0x01
#define REG_LIBRARY     0x03
#define REG_WAVESEQ1    0x04
#define REG_GO          0x0C
#define REG_FEEDBACK    0x1A
#define WAVESEQ_SLOTS   8

static i2c_master_dev_handle_t s_dev;

esp_err_t drv2605_init(void)
{
    esp_err_t err = i2c_bus_probe(DRV_ADDR);
    if (err != ESP_OK) {
        return err;
    }
    err = i2c_bus_add_device(DRV_ADDR, &s_dev);
    if (err != ESP_OK) {
        return err;
    }

    uint8_t status = 0;
    err = i2c_bus_read_regs(s_dev, REG_STATUS, &status, 1);
    if (err != ESP_OK) {
        s_dev = NULL;
        return err;
    }
    ESP_LOGI(TAG, "device id %d (3 = DRV2605, 7 = DRV2605L)", status >> 5);

    uint8_t fb = 0;
    if ((err = i2c_bus_write_reg(s_dev, REG_MODE, 0x00)) != ESP_OK ||       // out of standby, internal trigger
        (err = i2c_bus_write_reg(s_dev, REG_LIBRARY, 0x01)) != ESP_OK ||    // ERM library 1
        (err = i2c_bus_read_regs(s_dev, REG_FEEDBACK, &fb, 1)) != ESP_OK ||
        (err = i2c_bus_write_reg(s_dev, REG_FEEDBACK, fb & 0x7F)) != ESP_OK) {   // N_ERM_LRA = 0 (ERM)
        s_dev = NULL;
        return err;
    }
    return ESP_OK;
}

esp_err_t drv2605_play(const uint8_t *effects, size_t n)
{
    if (s_dev == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    if (n > WAVESEQ_SLOTS) {
        n = WAVESEQ_SLOTS;
    }
    esp_err_t err = ESP_OK;
    for (size_t i = 0; i < n && err == ESP_OK; i++) {
        err = i2c_bus_write_reg(s_dev, REG_WAVESEQ1 + i, effects[i]);
    }
    if (err == ESP_OK && n < WAVESEQ_SLOTS) {
        err = i2c_bus_write_reg(s_dev, REG_WAVESEQ1 + n, 0);   // end of sequence
    }
    if (err == ESP_OK) {
        err = i2c_bus_write_reg(s_dev, REG_GO, 0x01);
    }
    return err;
}
