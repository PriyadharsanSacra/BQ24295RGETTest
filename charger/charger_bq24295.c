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
#define BQ24295_IPRECHG_MIN_UA         	128000
#define BQ24295_IPRECHG_MAX_UA        	2048000
#define BQ24295_ITERM_OFFSET_UA        	128000
#define BQ24295_ITERM_STEP_UA          	128000
#define BQ24295_ITERM_MIN_UA           	128000
#define BQ24295_ITERM_MAX_UA          	2048000

/* Register 0x04 - Charge Voltage Control */
#define BQ24295_REG_CHARGE_VOLTAGE	0x04

#define BQ24295_VREG_MASK             	GENMASK(7, 2)
#define BQ24295_BATLOWV               	BIT(1)
#define BQ24295_VRECHG                	BIT(0)

#define BQ24295_VREG_OFFSET_UV       	3504000
#define BQ24295_VREG_STEP_UV         	16000
#define BQ24295_VREG_MIN_UV          	3504000
#define BQ24295_VREG_MAX_UV          	4400000
#define BQ24295_BATLOWV_SEL_2800MV     	0
#define BQ24295_BATLOWV_SEL_3000MV     	1
#define BQ24295_VRECHG_SEL_100MV       	0
#define BQ24295_VRECHG_SEL_300MV       	1

/* Register 0x05 - Charge Termination/Timer Control */
#define BQ24295_REG_CHARGE_TERM_TIMER  	0x05

#define BQ24295_EN_TERM               	BIT(7)
#define BQ24295_WATCHDOG_MASK         	GENMASK(5, 4)
#define BQ24295_EN_TIMER              	BIT(3)
#define BQ24295_CHG_TIMER_MASK        	GENMASK(2, 1)

#define BQ24295_WATCHDOG_SEL_DISABLE   	0x0
#define BQ24295_WATCHDOG_SEL_40S       	0x1
#define BQ24295_WATCHDOG_SEL_80S       	0x2
#define BQ24295_WATCHDOG_SEL_160S      	0x3
#define BQ24295_CHG_TIMER_SEL_5H       	0x0
#define BQ24295_CHG_TIMER_SEL_8H       	0x1
#define BQ24295_CHG_TIMER_SEL_12H      	0x2
#define BQ24295_CHG_TIMER_SEL_20H      	0x3

/* Register 0x06 - Boost Voltage / Thermal Regulation Control */
#define BQ24295_REG_BOOST_THERMAL      	0x06

#define BQ24295_BOOSTV_MASK            	GENMASK(7, 4)
#define BQ24295_BHOT_MASK              	GENMASK(3, 2)
#define BQ24295_TREG_MASK              	GENMASK(1, 0)

#define BQ24295_BOOSTV_OFFSET_UV       	4550000
#define BQ24295_BOOSTV_STEP_UV         	64000
#define BQ24295_BOOSTV_MIN_UV          	4550000
#define BQ24295_BOOSTV_MAX_UV          	5510000
#define BQ24295_BHOT_SEL_VBHOT1        	0x0
#define BQ24295_BHOT_SEL_VBHOT0        	0x1
#define BQ24295_BHOT_SEL_VBHOT2        	0x2
#define BQ24295_BHOT_SEL_DISABLE       	0x3
#define BQ24295_TREG_SEL_60C           	0x0
#define BQ24295_TREG_SEL_80C           	0x1
#define BQ24295_TREG_SEL_100C          	0x2
#define BQ24295_TREG_SEL_120C          	0x3

/* Register 0x07 - Misc Operation Control */
#define BQ24295_REG_MISC_OPERATION     	0x07

#define BQ24295_DPDM_EN              	BIT(7)
#define BQ24295_TMR2X_EN             	BIT(6)
#define BQ24295_BATFET_DISABLE       	BIT(5)
#define BQ24295_INT_MASK_MASK        	GENMASK(1, 0)

#define BQ24295_INT_MASK_NONE          	0x0
#define BQ24295_INT_MASK_BAT_FAULT     	BIT(0)
#define BQ24295_INT_MASK_CHRG_FAULT    	BIT(1)
#define BQ24295_INT_MASK_ALL           	GENMASK(1, 0)

/* Register 0x08 - System Status */
#define BQ24295_REG_SYSTEM_STATUS      	0x08

#define BQ24295_VBUS_STAT_MASK         	GENMASK(7, 6)
#define BQ24295_CHRG_STAT_MASK         	GENMASK(5, 4)
#define BQ24295_DPM_STAT               	BIT(3)
#define BQ24295_PG_STAT                	BIT(2)
#define BQ24295_THERM_STAT             	BIT(1)
#define BQ24295_VSYS_STAT              	BIT(0)

#define BQ24295_VBUS_STAT_UNKNOWN      	0x0
#define BQ24295_VBUS_STAT_USB_HOST     	0x1
#define BQ24295_VBUS_STAT_ADAPTER      	0x2
#define BQ24295_VBUS_STAT_OTG          	0x3
#define BQ24295_CHRG_STAT_NOT_CHARGING 	0x0
#define BQ24295_CHRG_STAT_PRECHARGE    	0x1
#define BQ24295_CHRG_STAT_FASTCHARGE   	0x2
#define BQ24295_CHRG_STAT_DONE         	0x3

/* Register 0x09 - Fault Register */
#define BQ24295_REG_FAULT              		0x09

#define BQ24295_WATCHDOG_FAULT         		BIT(7)
#define BQ24295_OTG_FAULT              		BIT(6)
#define BQ24295_CHRG_FAULT_MASK        		GENMASK(5, 4)
#define BQ24295_BAT_FAULT              		BIT(3)
#define BQ24295_NTC_FAULT_MASK         		GENMASK(1, 0)

#define BQ24295_CHRG_FAULT_NORMAL      		0x0
#define BQ24295_CHRG_FAULT_INPUT       		0x1
#define BQ24295_CHRG_FAULT_THERMAL     		0x2
#define BQ24295_CHRG_FAULT_TIMER       		0x3

#define BQ24295_NTC_FAULT_NORMAL    		0x0
#define BQ24295_NTC_FAULT_COLD      		0x1
#define BQ24295_NTC_FAULT_HOT       		0x2
#define BQ24295_NTC_FAULT_RESERVED  		0x3


/* Register 0x0A - Vendor / Part / Revision Status */
#define BQ24295_REG_VENDOR             0x0A

#define BQ24295_REVISION_MASK          GENMASK(2, 0)
#define BQ24295_PART_NUMBER_MASK       GENMASK(7, 5)

#define BQ24295_PART_NUMBER            0x6

struct bq24295_config {
	struct i2c_dt_spec i2c;
	struct gpio_dt_spec ce_gpio;
	struct gpio_dt_spec int_gpio;
	struct gpio_dt_spec stat_gpio;
};

struct bq24295_data {
	uint8_t part_no;
	uint8_t revision;
};

static const uint32_t bq24295_iinlim_table[] = {
	100000,
	150000,
	500000,
	900000,
	1000000,
	1500000,
	2000000,
	3000000,
};

static int bq24295_reg_read(const struct device *dev, uint8_t reg, uint8_t *val)
{
	const struct bq24295_config *config = dev->config;

	return i2c_reg_read_byte_dt(&config->i2c, reg, val);
}

static int bq24295_reg_write(const struct device *dev, uint8_t reg, uint8_t val)
{
	const struct bq24295_config *config = dev->config;

	return i2c_reg_write_byte_dt(&config->i2c, reg, val);
}

static int bq24295_reg_update(const struct device *dev, uint8_t reg, uint8_t mask, uint8_t val)
{
	const struct bq24295_config *config = dev->config;

	return i2c_reg_update_byte_dt(&config->i2c, reg, mask, val);
}

static int bq24295_field_read(const struct device *dev, uint8_t reg, uint8_t mask, uint8_t *value)
{
	uint8_t tmp;
	int ret;

	ret = bq24295_reg_read(dev, reg, &tmp);

	printk("bq24295_field_read: reg=0x%02x, mask=0x%02x, tmp=0x%02x\n", reg, mask, tmp);

	if (ret < 0) {
		return ret;
	}

	*value = FIELD_GET(mask, tmp);

	return 0;
}

static int bq24295_field_write(const struct device *dev, uint8_t reg, uint8_t mask, uint8_t value)
{
	return bq24295_reg_update(dev, reg, mask, FIELD_PREP(mask, value));
}

static int bq24295_test_bit(const struct device *dev, uint8_t reg, uint8_t bit, bool *set)
{
	uint8_t value;
	int ret;

	ret = bq24295_reg_read(dev, reg, &value);
	if (ret < 0) {
		return ret;
	}

	*set = (value & bit) != 0U;

	return 0;
}

static int bq24295_identify(const struct device *dev)
{
	struct bq24295_data *data = dev->data;
	uint8_t reg;
	int ret;

	ret = bq24295_reg_read(dev, BQ24295_REG_VENDOR, &reg);

	if (ret < 0) {
		LOG_ERR("Failed to read vendor register (%d)", ret);
		return ret;
	}

	data->part_no = FIELD_GET(BQ24295_PART_NUMBER_MASK, reg);
	data->revision = FIELD_GET(BQ24295_REVISION_MASK, reg);

	if (data->part_no != BQ24295_PART_NUMBER) {
		LOG_ERR("Unexpected part number: 0x%x", data->part_no);
		return -ENODEV;
	}

	LOG_INF("Detected BQ24295 (PN=0x%x, Rev=%u)", data->part_no, data->revision);

	return 0;
}

static int bq24295_get_online(const struct device *dev, enum charger_online *online)
{
	uint8_t vbus;
	int ret;

	ret = bq24295_field_read(dev,
				BQ24295_REG_SYSTEM_STATUS,
				BQ24295_VBUS_STAT_MASK,
				&vbus);

	printk("bq24295_get_online: vbus=%u\n", vbus);

	if (ret < 0) {
		return ret;
	}

	switch (vbus) {
	case BQ24295_VBUS_STAT_UNKNOWN:
		*online = CHARGER_ONLINE_OFFLINE;
		break;
	case BQ24295_VBUS_STAT_USB_HOST:
	case BQ24295_VBUS_STAT_ADAPTER:
		*online = CHARGER_ONLINE_FIXED;
		break;
	case BQ24295_VBUS_STAT_OTG:
		*online = CHARGER_ONLINE_OFFLINE;
		break;

	default:
		return -EIO;
	}

	return 0;
}

static int bq24295_get_status(const struct device *dev, enum charger_status *status)
{
	uint8_t chrg;
	int ret;

	ret = bq24295_field_read(dev,
				BQ24295_REG_SYSTEM_STATUS,
				BQ24295_CHRG_STAT_MASK,
				&chrg);

	printk("bq24295_get_status: chrg=%u\n", chrg);

	if (ret < 0) {
		return ret;
	}

	switch (chrg) {
	case BQ24295_CHRG_STAT_NOT_CHARGING:
		*status = CHARGER_STATUS_NOT_CHARGING;
		break;
	case BQ24295_CHRG_STAT_PRECHARGE:
	case BQ24295_CHRG_STAT_FASTCHARGE:
		*status = CHARGER_STATUS_CHARGING;
		break;
	case BQ24295_CHRG_STAT_DONE:
		*status = CHARGER_STATUS_FULL;
		break;
	default:
		return -EIO;
	}

	return 0;
}

static int bq24295_charger_get_charge_type(const struct device *dev,
					   enum charger_charge_type *charge_type)
{
	*charge_type = CHARGER_CHARGE_TYPE_UNKNOWN;
	return 0;
}

static int bq24295_get_health(const struct device *dev,
			      enum charger_health *health)
{
	uint8_t fault;
	uint8_t chrg_fault;
	uint8_t ntc_fault;
	int ret;

	/*
	* REG09 is a latched fault register.
	* Reading it acknowledges previously latched faults.
	*/
	ret = bq24295_reg_read(dev, BQ24295_REG_FAULT, &fault);
	if (ret < 0) {
		return ret;
	}

	printk("bq24295_get_health: fault=0x%02x\n", fault);

	if (fault & BQ24295_BAT_FAULT) {
		*health = CHARGER_HEALTH_OVERVOLTAGE;
		return 0;
	}
	
	ntc_fault = FIELD_GET(BQ24295_NTC_FAULT_MASK, fault);
	
	switch (ntc_fault) {
	case BQ24295_NTC_FAULT_NORMAL:
		break;
	case BQ24295_NTC_FAULT_COLD:
		*health = CHARGER_HEALTH_COLD;
		return 0;
	case BQ24295_NTC_FAULT_HOT:
		*health = CHARGER_HEALTH_HOT;
		return 0;
	default:
		LOG_WRN("Reserved NTC fault value: %u", ntc_fault);
		*health = CHARGER_HEALTH_UNSPEC_FAILURE;
		return 0;
	}

	if (fault & BQ24295_WATCHDOG_FAULT) {
		*health = CHARGER_HEALTH_WATCHDOG_TIMER_EXPIRE;
		return 0;
	}

	chrg_fault = FIELD_GET(BQ24295_CHRG_FAULT_MASK, fault);

	switch (chrg_fault) {
	case BQ24295_CHRG_FAULT_NORMAL:
		break;
	case BQ24295_CHRG_FAULT_INPUT:
		*health = CHARGER_HEALTH_UNSPEC_FAILURE;
		return 0;
	case BQ24295_CHRG_FAULT_THERMAL:
		*health = CHARGER_HEALTH_OVERHEAT;
		return 0;
	case BQ24295_CHRG_FAULT_TIMER:
		*health = CHARGER_HEALTH_SAFETY_TIMER_EXPIRE;
		return 0;
	default:
		LOG_ERR("Invalid CHRG_FAULT value: %u", chrg_fault);
		return -EIO;
	}

	*health = CHARGER_HEALTH_GOOD;

	return 0;
}

static int bq24295_get_constant_charge_current(const struct device *dev, uint32_t *current_ua)
{
	uint8_t ichg;
	int ret;

	ret = bq24295_field_read(dev,
				BQ24295_REG_CHARGE_CURRENT,
				BQ24295_ICHG_MASK,
				&ichg);
	if (ret < 0) {
		return ret;
	}

	*current_ua = BQ24295_ICHG_OFFSET_UA + (ichg * BQ24295_ICHG_STEP_UA);

	return 0;
}

static int bq24295_get_constant_charge_voltage(const struct device *dev, uint32_t *voltage_uv)
{
	uint8_t vreg;
	int ret;

	ret = bq24295_field_read(dev,
				BQ24295_REG_CHARGE_VOLTAGE,
				BQ24295_VREG_MASK,
				&vreg);
	if (ret < 0) {
		return ret;
	}

	*voltage_uv = BQ24295_VREG_OFFSET_UV + (vreg * BQ24295_VREG_STEP_UV);

	return 0;
}

static int bq24295_get_input_current_limit(const struct device *dev, uint32_t *current_ua)
{
	uint8_t iinlim;
	int ret;

	ret = bq24295_field_read(dev,
				BQ24295_REG_INPUT_SRC_CTRL,
				BQ24295_IINLIM_MASK,
				&iinlim);
	if (ret < 0) {
		return ret;
	}

	if (iinlim >= ARRAY_SIZE(bq24295_iinlim_table)) {
		return -EIO;
	}

	*current_ua = bq24295_iinlim_table[iinlim];

	return 0;

}

static int bq24295_get_vindpm(const struct device *dev, uint32_t *voltage_uv)
{
	uint8_t vindpm;
	int ret;

	ret = bq24295_field_read(dev,
				BQ24295_REG_INPUT_SRC_CTRL,
				BQ24295_VINDPM_MASK,
				&vindpm);
	if (ret < 0) {
		return ret;
	}

	*voltage_uv = BQ24295_VINDPM_OFFSET_UV + (vindpm * BQ24295_VINDPM_STEP_UV);

	return 0;
}

static int bq24295_set_constant_charge_current(const struct device *dev, uint32_t current_ua)
{
	uint8_t ichg;

	if (current_ua < BQ24295_ICHG_MIN_UA || current_ua > BQ24295_ICHG_MAX_UA) {
		LOG_WRN("Charge current %u uA out of range, clamping",
			current_ua);
	}

	current_ua = CLAMP(current_ua, BQ24295_ICHG_MIN_UA, BQ24295_ICHG_MAX_UA);
	
	ichg = (current_ua - BQ24295_ICHG_OFFSET_UA) / BQ24295_ICHG_STEP_UA;

	return bq24295_field_write(dev,
				BQ24295_REG_CHARGE_CURRENT,
				BQ24295_ICHG_MASK,
				ichg);
}

static int bq24295_gpio_init(const struct device *dev)
{
	const struct bq24295_config *config = dev->config;
	int ret;

	/* CE GPIO (optional) */
	if (config->ce_gpio.port != NULL) {
		if (!gpio_is_ready_dt(&config->ce_gpio)) {
			LOG_ERR("CE GPIO not ready");
			return -ENODEV;
		}

		ret = gpio_pin_configure_dt(&config->ce_gpio,
					    GPIO_OUTPUT_INACTIVE);
		if (ret < 0) {
			LOG_ERR("Failed to configure CE GPIO (%d)", ret);
			return ret;
		}
	}

	/* INT GPIO (optional) */
	if (config->int_gpio.port != NULL) {
		if (!gpio_is_ready_dt(&config->int_gpio)) {
			LOG_ERR("INT GPIO not ready");
			return -ENODEV;
		}

		ret = gpio_pin_configure_dt(&config->int_gpio,
					    GPIO_INPUT);
		if (ret < 0) {
			LOG_ERR("Failed to configure INT GPIO (%d)", ret);
			return ret;
		}
	}

	/* STAT GPIO (optional) */
	if (config->stat_gpio.port != NULL) {
		if (!gpio_is_ready_dt(&config->stat_gpio)) {
			LOG_ERR("STAT GPIO not ready");
			return -ENODEV;
		}

		ret = gpio_pin_configure_dt(&config->stat_gpio,
					    GPIO_INPUT);
		if (ret < 0) {
			LOG_ERR("Failed to configure STAT GPIO (%d)", ret);
			return ret;
		}
	}

	return 0;
}

static int bq24295_get_property(const struct device *dev, const charger_prop_t prop, 
				union charger_propval *val)
{
	uint8_t reg;
	int ret;

	switch (prop) {
	case CHARGER_PROP_ONLINE:
		return bq24295_get_online(dev, &val->online);
	case CHARGER_PROP_STATUS:
		return bq24295_get_status(dev, &val->status);
	case CHARGER_PROP_CHARGE_TYPE:
		return bq24295_charger_get_charge_type(dev, &val->charge_type);
	case CHARGER_PROP_HEALTH:
		return bq24295_get_health(dev, &val->health);
	case CHARGER_PROP_CONSTANT_CHARGE_CURRENT_UA:
		return bq24295_get_constant_charge_current(dev, &val->const_charge_current_ua);
	case CHARGER_PROP_CONSTANT_CHARGE_VOLTAGE_UV:
		return bq24295_get_constant_charge_voltage(dev, &val->const_charge_voltage_uv);
	case CHARGER_PROP_INPUT_REGULATION_CURRENT_UA:
		return bq24295_get_input_current_limit(dev, &val->input_current_regulation_current_ua);
	case CHARGER_PROP_INPUT_REGULATION_VOLTAGE_UV:
		return bq24295_get_vindpm(dev, &val->input_voltage_regulation_voltage_uv);
	default:
		return -ENOTSUP;
	}
}

static int bq24295_set_property(const struct device *dev, const charger_prop_t prop,
				const union charger_propval *val)
{
	switch (prop) {
	case CHARGER_PROP_CONSTANT_CHARGE_CURRENT_UA:
		return bq24295_set_constant_charge_current(dev, val->const_charge_current_ua);
	default:
		return -ENOTSUP;
	}
}

static int bq24295_charge_enable(const struct device *dev,
				 const bool enable)
{
	ARG_UNUSED(dev);
	ARG_UNUSED(enable);

	return -ENOTSUP;
}

static int test_helpers(const struct device *dev)

{
	int ret;
	/*Validate all registers*/
	for (uint8_t reg = 0; reg <= 0x0A; reg++) {
		uint8_t value;
		int ret;

		ret = bq24295_reg_read(dev, reg, &value);
		if (ret) {
			printk("REG%02X read failed\n", reg);
		} else {
			printk("REG%02X = 0x%02X\n", reg, value);
		}
	}

	uint8_t part;

	ret = bq24295_field_read(dev,
				BQ24295_REG_VENDOR,
				BQ24295_PART_NUMBER_MASK,
				&part);

	printk("Part = %u\n", part);

	bool pg;

	ret = bq24295_test_bit(dev,
			BQ24295_REG_SYSTEM_STATUS,
			BQ24295_PG_STAT,
			&pg);

	printk("PG=%d\n", pg);

	uint8_t reg_value;

	return 0;
}

static int bq24295_init(const struct device *dev)
{
	const struct bq24295_config *config = dev->config;

	int ret = 0;

	if (!device_is_ready(config->i2c.bus)) {
		LOG_ERR("I2C bus not ready");
		return -ENODEV;
	}

	ret = bq24295_identify(dev);
	if (ret < 0) {
		LOG_ERR("Failed to identify BQ24295 (%d)", ret);
		return ret;
	}

	ret = bq24295_gpio_init(dev);
	if (ret < 0) {
		LOG_ERR("Failed to initialize GPIOs (%d)", ret);
		return ret;
	} else {
		LOG_INF("GPIOs initialized successfully");
	}

	test_helpers(dev);

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
