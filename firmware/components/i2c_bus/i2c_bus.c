#include "esp_log.h"
#include "i2c_bus.h"

static const char *TAG = "i2c_bus";
static i2c_master_bus_handle_t s_bus;

esp_err_t i2c_bus_init(void)
{
    if (s_bus != NULL) {
        return ESP_OK;
    }
    i2c_master_bus_config_t cfg = {
        .i2c_port = I2C_BUS_PORT,
        .sda_io_num = I2C_BUS_SDA_GPIO,
        .scl_io_num = I2C_BUS_SCL_GPIO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    esp_err_t err = i2c_new_master_bus(&cfg, &s_bus);
    if (err != ESP_OK) {
        s_bus = NULL;
        ESP_LOGE(TAG, "bus init failed: %s", esp_err_to_name(err));
        return err;
    }
    ESP_LOGI(TAG, "I2C ready: SDA GPIO%d SCL GPIO%d %d Hz",
             I2C_BUS_SDA_GPIO, I2C_BUS_SCL_GPIO, I2C_BUS_FREQ_HZ);
    return ESP_OK;
}

esp_err_t i2c_bus_probe(uint8_t addr)
{
    if (s_bus == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    return i2c_master_probe(s_bus, addr, I2C_BUS_TIMEOUT_MS);
}

esp_err_t i2c_bus_add_device(uint8_t addr, i2c_master_dev_handle_t *out)
{
    if (s_bus == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    i2c_device_config_t dev = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = addr,
        .scl_speed_hz = I2C_BUS_FREQ_HZ,
    };
    return i2c_master_bus_add_device(s_bus, &dev, out);
}

esp_err_t i2c_bus_write_reg(i2c_master_dev_handle_t dev, uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = { reg, val };
    return i2c_master_transmit(dev, buf, sizeof(buf), I2C_BUS_TIMEOUT_MS);
}

esp_err_t i2c_bus_read_regs(i2c_master_dev_handle_t dev, uint8_t reg, uint8_t *buf, size_t len)
{
    return i2c_master_transmit_receive(dev, &reg, 1, buf, len, I2C_BUS_TIMEOUT_MS);
}
