#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "esp_err.h"
#include "VL53L1X_api.h"

#define ST_DEVICE 0x52
static i2c_master_dev_handle_t sensor;
static esp_err_t last_io_error = ESP_OK;

static int8_t io_status(esp_err_t err, uint16_t reg, const char *operation) {
    if (err == ESP_OK) return 0;
    last_io_error = err;
    ESP_LOGE("I2C_DIAG", "%s register 0x%04X: %s", operation, reg, esp_err_to_name(err));
    return -1;
}
int8_t VL53L1_WriteMulti(uint16_t dev, uint16_t index, uint8_t *data, uint32_t count) {
    if (dev != ST_DEVICE || !data || count == 0 || count > 256) return io_status(ESP_ERR_INVALID_ARG, index, "Write");
    uint8_t tx[258];
    tx[0] = index >> 8; tx[1] = index & 0xff;
    memcpy(tx + 2, data, count);
    return io_status(i2c_master_transmit(sensor, tx, count + 2, 200), index, "Write");
}
int8_t VL53L1_ReadMulti(uint16_t dev, uint16_t index, uint8_t *data, uint32_t count) {
    if (dev != ST_DEVICE || !data || count == 0 || count > 256) return io_status(ESP_ERR_INVALID_ARG, index, "Read");
    uint8_t reg[2] = {index >> 8, index & 0xff};
    memset(data, 0, count);
    return io_status(i2c_master_transmit_receive(sensor, reg, 2, data, count, 200), index, "Read");
}
int8_t VL53L1_WrByte(uint16_t dev, uint16_t reg, uint8_t data) { return VL53L1_WriteMulti(dev, reg, &data, 1); }
int8_t VL53L1_WrWord(uint16_t dev, uint16_t reg, uint16_t data) {
    uint8_t bytes[2] = {data >> 8, data & 0xff};
    return VL53L1_WriteMulti(dev, reg, bytes, 2);
}
int8_t VL53L1_WrDWord(uint16_t dev, uint16_t reg, uint32_t data) {
    uint8_t bytes[4] = {data >> 24, data >> 16, data >> 8, data};
    return VL53L1_WriteMulti(dev, reg, bytes, 4);
}
int8_t VL53L1_RdByte(uint16_t dev, uint16_t reg, uint8_t *data) { return VL53L1_ReadMulti(dev, reg, data, 1); }
int8_t VL53L1_RdWord(uint16_t dev, uint16_t reg, uint16_t *data) {
    if (!data) return -1;
    uint8_t bytes[2] = {0};
    int8_t status = VL53L1_ReadMulti(dev, reg, bytes, 2);
    *data = ((uint16_t)bytes[0] << 8) | bytes[1];
    return status;
}
int8_t VL53L1_RdDWord(uint16_t dev, uint16_t reg, uint32_t *data) {
    if (!data) return -1;
    uint8_t bytes[4] = {0};
    int8_t status = VL53L1_ReadMulti(dev, reg, bytes, 4);
    *data = ((uint32_t)bytes[0] << 24) | ((uint32_t)bytes[1] << 16) | ((uint32_t)bytes[2] << 8) | bytes[3];
    return status;
}
int8_t VL53L1_WaitMs(uint16_t dev, int32_t ms) {
    (void)dev;
    if (ms < 0) return -1;
    if (ms) { TickType_t ticks = pdMS_TO_TICKS(ms); vTaskDelay(ticks ? ticks : 1); }
    return 0;
}
static bool checked(uint8_t status, const char *step) {
    if (status == 0 && last_io_error == ESP_OK) return true;
    ESP_LOGE("TOF_RANGE", "%s failed: ULD=%u, I2C=%s; stopping", step, status, esp_err_to_name(last_io_error));
    return false;
}
#define STEP(call) do { if (!checked((call), #call)) return; } while (0)

void app_main(void) {
    i2c_master_bus_handle_t bus;
    i2c_master_bus_config_t cfg = {
        .i2c_port = I2C_NUM_0, .sda_io_num = 5, .scl_io_num = 6,
        .clk_source = I2C_CLK_SRC_DEFAULT, .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&cfg, &bus));
    i2c_device_config_t dev = {.dev_addr_length = I2C_ADDR_BIT_LEN_7, .device_address = 0x29, .scl_speed_hz = 50000};
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus, &dev, &sensor));
    vTaskDelay(pdMS_TO_TICKS(100));
    uint16_t id = 0;
    STEP(VL53L1X_GetSensorId(ST_DEVICE, &id));
    ESP_LOGI("TOF_RANGE", "Sensor ID: 0x%04X; ST ULD 3.5.5", id);
    if (id != 0xEACC) { ESP_LOGE("TOF_RANGE", "Unexpected sensor ID; stopping"); return; }
    uint8_t booted = 0;
    for (unsigned attempt = 0; attempt < 100 && !booted; ++attempt) {
        STEP(VL53L1X_BootState(ST_DEVICE, &booted));
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    if (!booted) { ESP_LOGE("TOF_RANGE", "Boot timeout; stopping"); return; }
    STEP(VL53L1X_SensorInit(ST_DEVICE));
    STEP(VL53L1X_SetDistanceMode(ST_DEVICE, 2));
    STEP(VL53L1X_SetTimingBudgetInMs(ST_DEVICE, 50));
    STEP(VL53L1X_SetInterMeasurementInMs(ST_DEVICE, 100));
    int16_t offset = 0; uint16_t xtalk = 0;
    STEP(VL53L1X_GetOffset(ST_DEVICE, &offset));
    STEP(VL53L1X_GetXtalk(ST_DEVICE, &xtalk));
    ESP_LOGI("TOF_RANGE", "Settings after init: offset=%d mm, xtalk=%u; no added correction", offset, xtalk);
    STEP(VL53L1X_StartRanging(ST_DEVICE));
    ESP_LOGI("TOF_RANGE", "Ranging started: long mode, 50 ms budget, 100 ms period");
    unsigned valid = 0, invalid = 0;
    while (1) {
        uint8_t ready = 0;
        TickType_t start = xTaskGetTickCount();
        while (!ready) {
            if (!checked(VL53L1X_CheckForDataReady(ST_DEVICE, &ready), "Check data ready")) goto stopped;
            if ((xTaskGetTickCount() - start) >= pdMS_TO_TICKS(2000)) {
                ESP_LOGE("TOF_RANGE", "No fresh measurement within 2 seconds; stopping");
                goto stopped;
            }
            if (!ready) vTaskDelay(pdMS_TO_TICKS(10));
        }
        VL53L1X_Result_t result = {0};
        if (!checked(VL53L1X_GetResult(ST_DEVICE, &result), "Read result")) goto stopped;
        if (!checked(VL53L1X_ClearInterrupt(ST_DEVICE), "Clear interrupt")) goto stopped;
        ESP_LOGI("TOF_QUALITY", "status=%u, raw_mm=%u, ambient=%u, signal_per_spad=%u, spads=%u (ULD result fields)",
                 (unsigned)result.Status, (unsigned)result.Distance,
                 (unsigned)result.Ambient, (unsigned)result.SigPerSPAD,
                 (unsigned)result.NumSPADs);
        if (result.Status == 0) {
            ++valid;
            ESP_LOGI("TOF_RANGE", "Distance: %.1f cm (raw: %u mm), status=0", result.Distance / 10.0, result.Distance);
        } else {
            ++invalid;
            ESP_LOGW("TOF_RANGE", "Invalid measurement: status=%u, raw=%u mm (not a valid distance)", result.Status, result.Distance);
        }
        if ((valid + invalid) % 50 == 0) ESP_LOGI("TOF_RANGE", "Counts: valid=%u, invalid=%u", valid, invalid);
    }
stopped:
    ESP_LOGE("TOF_RANGE", "Stopped: valid=%u, invalid=%u; reset after resolving the fault", valid, invalid);
    (void)VL53L1X_StopRanging(ST_DEVICE);
}
