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

LOG_MODULE_REGISTER(ti_bq24295, CONFIG_CHARGER_LOG_LEVEL);

/* Register 0x00 - Input Source Control */
#define BQ24295_REG_INPUT_SRC_CTRL     	0x00

#define BQ24295_IINLIM_MASK            	GENMASK(2, 0)
#define BQ24295_VINDPM_MASK            	GENMASK(6, 3)
#define BQ24295_EN_HIZ                 	BIT(7)

#define BQ24295_IINLIM_SEL_100MA   	0x0
#define BQ24295_IINLIM_SEL_150MA   	0x1
#define BQ24295_IINLIM_SEL_500MA   	0x2
#define BQ24295_IINLIM_SEL_900MA   	0x3
#define BQ24295_IINLIM_SEL_1000MA  	0x4
#define BQ24295_IINLIM_SEL_1500MA  	0x5
#define BQ24295_IINLIM_SEL_2000MA  	0x6
#define BQ24295_IINLIM_SEL_3000MA  	0x7
#define BQ24295_VINDPM_OFFSET_UV       	3880000
#define BQ24295_VINDPM_STEP_UV         	80000
#define BQ24295_VINDPM_MIN_UV          	3880000
#define BQ24295_VINDPM_MAX_UV          	5080000

/* Register 0x01 - Power-On Configuration */
#define BQ24295_REG_POWER_ON_CONFIG      0x01

#define BQ24295_BOOST_LIM              	BIT(0)
#define BQ24295_SYS_MIN_MASK           	GENMASK(3, 1)
#define BQ24295_CHG_CONFIG             	BIT(4)
#define BQ24295_OTG_CONFIG             	BIT(5)
#define BQ24295_WDT_RESET              	BIT(6)
#define BQ24295_REG_RESET              	BIT(7)

#define BQ24295_SYS_MIN_OFFSET_UV	3000000
#define BQ24295_SYS_MIN_STEP_UV  	100000
#define BQ24295_SYS_MIN_MIN_UV   	3000000
#define BQ24295_SYS_MIN_MAX_UV   	3700000

/* Register 0x02 - Charge Current Control */
#define BQ24295_REG_CHARGE_CURRENT      0x02

#define BQ24295_FORCE_20PCT             BIT(0)
#define BQ24295_BCOLD                   BIT(1)
#define BQ24295_ICHG_MASK               GENMASK(7, 2)

#define BQ24295_BCOLD_SEL_76_PERCENT    0
#define BQ24295_BCOLD_SEL_79_PERCENT    1
#define BQ24295_ICHG_OFFSET_UA          512000
#define BQ24295_ICHG_STEP_UA            64000
#define BQ24295_ICHG_MIN_UA             512000
#define BQ24295_ICHG_MAX_UA            	3008000

/* Register 0x03 - Pre-Charge and Termination Current Control */
#define BQ24295_REG_PRECHG_TERM_CURRENT	0x03
#define BQ24295_IPRECHG_MASK           	GENMASK(7,4)
#define BQ24295_ITERM_MASK             	GENMASK(3,0)
#define BQ24295_IPRECHG_OFFSET_UA      	128000
#define BQ24295_IPRECHG_STEP_UA        	128000
#define BQ24295_ITERM_OFFSET_UA        	128000
#define BQ24295_ITERM_STEP_UA          	128000

/* Register 0x04 - Charge Voltage Control */
#define BQ24295_REG_CHARGE_VOLTAGE	0x04
#define BQ24295_VREG_MASK              	GENMASK(7,2)
#define BQ24295_VREG_OFFSET_UV         	3504000
#define BQ24295_VREG_STEP_UV           	16000

#define BQ24295_REG_CHARGE_TERM_TIMER	0x05
#define BQ24295_REG_BOOST_THERMAL	0x06
#define BQ24295_REG_MISC_OPERATION	0x07

/* Register 0x08 - System Status */
#define BQ24295_REG_SYSTEM_STATUS     	0x08
#define BQ24295_VBUS_STAT_MASK         	GENMASK(7,6)
#define BQ24295_CHRG_STAT_MASK         	GENMASK(5,4)

#define BQ24295_DPM_STAT               	BIT(3)
#define BQ24295_PG_STAT                	BIT(2)
#define BQ24295_THERM_STAT             	BIT(1)
#define BQ24295_VSYS_STAT              	BIT(0)

/* Register 0x09 - Fault Status */
#define BQ24295_REG_FAULT             	0x09
#define BQ24295_WATCHDOG_FAULT         	BIT(7)
#define BQ24295_OTG_FAULT              	BIT(6)
#define BQ24295_CHRG_FAULT_MASK        	GENMASK(5,4)
#define BQ24295_BAT_FAULT              	BIT(3)
#define BQ24295_NTC_FAULT_MASK         	GENMASK(2,0)

#define BQ24295_REG_VENDOR            	0x0A

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

static int bq24295_get_property(const struct device *dev,
				const charger_prop_t prop,
				union charger_propval *val)
{
	uint8_t reg;
	int ret;

	switch (prop) {
	case CHARGER_PROP_ONLINE:
		ret = bq24295_reg_read(dev, BQ24295_REG_SYSTEM_STATUS, &reg);
		if (ret < 0) {
			return ret;
		}

		/* We'll decode REG08 later */
		val->online = CHARGER_ONLINE_FIXED;
		return 0;

	default:
		return -ENOTSUP;
	}
}

static int bq24295_set_property(const struct device *dev,
				const charger_prop_t prop,
				const union charger_propval *val)
{
	ARG_UNUSED(dev);
	ARG_UNUSED(prop);
	ARG_UNUSED(val);

	return -ENOTSUP;
}

static int bq24295_charge_enable(const struct device *dev,
				 const bool enable)
{
	ARG_UNUSED(dev);
	ARG_UNUSED(enable);

	return -ENOTSUP;
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

static DEVICE_API(charger, bq24295_api) = {
	.get_property = bq24295_get_property,
	.set_property = bq24295_set_property,
	.charge_enable = bq24295_charge_enable,
};

#define BQ24295_INIT(inst)                                     \
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
                          CONFIG_CHARGER_INIT_PRIORITY,      \
                          &bq24295_api);

DT_INST_FOREACH_STATUS_OKAY(BQ24295_INIT)