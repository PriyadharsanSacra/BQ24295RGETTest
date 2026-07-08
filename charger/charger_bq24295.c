/*
* Copyright 2026 Sacra Systems Private Limited.
*
* SPDX-License-Identifier: Apache-2.0
*/

#define DT_DRV_COMPAT ti_bq24295

#include <errno.h>

#include <zephyr/device.h>
#include <zephyr/drivers/charger.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>

LOG_MODULE_REGISTER(ti_bq24295, CONFIG_LOG_DEFAULT_LEVEL);

#define BQ24295_REG_INPUT_SRC_CTRL          0x00
#define BQ24295_REG_POWER_ON_CONFIG         0x01
#define BQ24295_REG_CHARGE_CURRENT          0x02
#define BQ24295_REG_PRECHG_TERM_CURRENT     0x03
#define BQ24295_REG_CHARGE_VOLTAGE          0x04
#define BQ24295_REG_CHARGE_TERM_TIMER       0x05
#define BQ24295_REG_BOOST_THERMAL           0x06
#define BQ24295_REG_MISC_OPERATION          0x07
#define BQ24295_REG_SYSTEM_STATUS           0x08
#define BQ24295_REG_FAULT                   0x09
#define BQ24295_REG_VENDOR                  0x0A

struct bq24295_config {
	struct i2c_dt_spec i2c;
	struct gpio_dt_spec ce_gpio;
	struct gpio_dt_spec int_gpio;
	struct gpio_dt_spec stat_gpio;
};

struct bq24295_data {
	uint8_t status_reg;
};

static int bq24295_reg_read(const struct device *dev,
                            uint8_t reg,
                            uint8_t *val)
{
	const struct bq24295_config *config = dev->config;

	return i2c_reg_read_byte_dt(&config->i2c, reg, val);
}

static int bq24295_reg_write(const struct device *dev,
                             uint8_t reg,
                             uint8_t val)
{
	const struct bq24295_config *config = dev->config;

	return i2c_reg_write_byte_dt(&config->i2c, reg, val);
}

static int bq24295_reg_update(const struct device *dev,
                              uint8_t reg,
                              uint8_t mask,
                              uint8_t val)
{
	const struct bq24295_config *config = dev->config;

	return i2c_reg_update_byte_dt(&config->i2c,
					reg,
					mask,
					val);
}

int bq24295_get_system_status(const struct device *dev,
				     uint8_t *status)
{
	return bq24295_reg_read(dev,
				BQ24295_REG_SYSTEM_STATUS,
				status);
}

int bq24295_get_fault_status(const struct device *dev,
				    uint8_t *fault)
{
	return bq24295_reg_read(dev,
				BQ24295_REG_FAULT,
				fault);
}

int bq24295_get_vendor_info(const struct device *dev,
				   uint8_t *vendor)
{
	return bq24295_reg_read(dev,
				BQ24295_REG_VENDOR,
				vendor);
}

static int bq24295_init(const struct device *dev)
{
	const struct bq24295_config *config = dev->config;

	if (!device_is_ready(config->i2c.bus)) {
		LOG_ERR("I2C bus not ready");
		return -ENODEV;
	}

	LOG_INF("BQ24295 initialized");

	return 0;
}

#define BQ24295_DEFINE(inst)                                     \
    static struct bq24295_data data_##inst;                      \
    static const struct bq24295_config config_##inst = {         \
        .i2c = I2C_DT_SPEC_INST_GET(inst),                       \
        .int_gpio = GPIO_DT_SPEC_INST_GET_OR(inst, int_gpios, {}), \
        .stat_gpio = GPIO_DT_SPEC_INST_GET_OR(inst, stat_gpios, {}), \
        .ce_gpio = GPIO_DT_SPEC_INST_GET_OR(inst, ce_gpios, {}), \
    };                                                           \
                                                                 \
    DEVICE_DT_INST_DEFINE(inst,                                  \
                          bq24295_init,                           \
                          NULL,                                   \
                          &data_##inst,                           \
                          &config_##inst,                         \
                          POST_KERNEL,                            \
                          CONFIG_KERNEL_INIT_PRIORITY_DEVICE,      \
                          NULL);

DT_INST_FOREACH_STATUS_OKAY(BQ24295_DEFINE)