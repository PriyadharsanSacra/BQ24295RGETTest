/*
 * Copyright 2026 Sacra Systems Private Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/charger.h>

#define BQ24295_NODE DT_NODELABEL(bq24295)
#define BQ24295_INT_NODE  DT_NODELABEL(bq24295_int)
#define BQ24295_STAT_NODE DT_NODELABEL(bq24295_stat)
#define BQ24295_CE_NODE   DT_NODELABEL(bq24295_ce)

static const struct gpio_dt_spec int_gpio =
	GPIO_DT_SPEC_GET(BQ24295_INT_NODE, gpios);

static const struct gpio_dt_spec stat_gpio =
	GPIO_DT_SPEC_GET(BQ24295_STAT_NODE, gpios);

static struct gpio_callback int_cb_data;
static struct gpio_callback stat_cb_data;

static void int_callback(const struct device *port,
			 struct gpio_callback *cb,
			 gpio_port_pins_t pins)
{
	printk("INT interrupt\n");
}

static void stat_callback(const struct device *port,
			  struct gpio_callback *cb,
			  gpio_port_pins_t pins)
{
	// printk("STAT changed: %d\n", gpio_pin_get_dt(&stat_gpio));
}

int main(void)
{
	const struct device *charger = DEVICE_DT_GET(BQ24295_NODE);
	union charger_propval val;
	int ret;

	/*INT gpioS validation Start*/

	printk("BQ24295 GPIO Test\n");

	/* Check GPIO devices */
	if (!gpio_is_ready_dt(&int_gpio) ||
	    !gpio_is_ready_dt(&stat_gpio)) {
		printk("GPIO device not ready\n");
		return -ENODEV;
	}

	/* Configure INT as input */
	ret = gpio_pin_configure_dt(&int_gpio, GPIO_INPUT);
	if (ret) {
		printk("Failed to configure INT (%d)\n", ret);
		return ret;
	}

	/* Configure STAT as input */
	ret = gpio_pin_configure_dt(&stat_gpio, GPIO_INPUT);
	if (ret) {
		printk("Failed to configure STAT (%d)\n", ret);
		return ret;
	}

	/* Register INT callback */
	gpio_init_callback(&int_cb_data,
			   int_callback,
			   BIT(int_gpio.pin));

	gpio_add_callback(int_gpio.port, &int_cb_data);

	ret = gpio_pin_interrupt_configure_dt(&int_gpio,
					      GPIO_INT_EDGE_FALLING);
	if (ret) {
		printk("Failed to configure INT interrupt (%d)\n", ret);
		return ret;
	}

	/* Register STAT callback */
	gpio_init_callback(&stat_cb_data,
			   stat_callback,
			   BIT(stat_gpio.pin));

	gpio_add_callback(stat_gpio.port, &stat_cb_data);

	ret = gpio_pin_interrupt_configure_dt(&stat_gpio,
					      GPIO_INT_EDGE_BOTH);
	if (ret) {
		printk("Failed to configure STAT interrupt (%d)\n", ret);
		return ret;
	}

	printk("Initialization complete\n");

	/*INT gpioS validation End*/

	printk("\n=== BQ24295 Charger Driver Validation ===\n");

	if (!device_is_ready(charger)) {
		printk("BQ24295 device not ready\n");
		return 0;
	}

	printk("BQ24295 device ready\n");

	uint32_t new_current = 512000; /* 512 mA */
	val.const_charge_current_ua = new_current;

	ret = charger_set_prop(charger,
				CHARGER_PROP_CONSTANT_CHARGE_CURRENT_UA,
				&val);
	if (ret == 0) {
		printk("CONSTANT CHARGE CURRENT set to : %u uA\n",
			val.const_charge_current_ua);
	} else if (ret == -ENOTSUP) {
		printk("CONSTANT CHARGE CURRENT property not supported\n");
	} else {
		printk("charger_set_prop() failed (%d)\n", ret);
	}

	uint32_t new_voltage = 4208000; /* 4.2 V */
	val.const_charge_voltage_uv = new_voltage;

	ret = charger_set_prop(charger,
				CHARGER_PROP_CONSTANT_CHARGE_VOLTAGE_UV,
				&val);
	if (ret == 0) {
		printk("CONSTANT CHARGE VOLTAGE set to : %u uV\n",
			val.const_charge_voltage_uv);
	} else if (ret == -ENOTSUP) {
		printk("CONSTANT CHARGE VOLTAGE property not supported\n");
	} else {
		printk("charger_set_prop() failed (%d)\n", ret);
	}

	uint32_t new_input_current = 500000; /* 500 mA */
	val.input_current_regulation_current_ua = new_input_current;

	ret = charger_set_prop(charger,
				CHARGER_PROP_INPUT_REGULATION_CURRENT_UA,
				&val);
	if (ret == 0) {
		printk("INPUT REGULATION CURRENT set to : %u uA\n",
			val.input_current_regulation_current_ua);
	} else if (ret == -ENOTSUP) {
		printk("INPUT REGULATION CURRENT property not supported\n");
	} else {
		printk("charger_set_prop() failed (%d)\n", ret);
	}

	uint32_t new_input_voltage = 4760000; /* 4.76 V */
	val.input_voltage_regulation_voltage_uv = new_input_voltage;

	ret = charger_set_prop(charger,
				CHARGER_PROP_INPUT_REGULATION_VOLTAGE_UV,
				&val);
	if (ret == 0) {
		printk("INPUT REGULATION VOLTAGE set to : %u uV\n",
			val.input_voltage_regulation_voltage_uv);
	} else if (ret == -ENOTSUP) {
		printk("INPUT REGULATION VOLTAGE property not supported\n");
	} else {
		printk("charger_set_prop() failed (%d)\n", ret);
	}

	
	uint32_t precharge_current = 256000; /* 256 mA */
	val.precharge_current_ua = precharge_current;
	ret = charger_set_prop(charger, CHARGER_PROP_PRECHARGE_CURRENT_UA, &val);
	if (ret == 0) {
		printk("PRECHARGE CURRENT set to : %u uA\n", val.precharge_current_ua);
	} else if (ret == -ENOTSUP) {
		printk("PRECHARGE CURRENT property not supported\n");
	} else {
		printk("charger_set_prop() failed (%d)\n", ret);
	}
	
	uint32_t termination_current = 256000; /* 256 mA */
	val.charge_term_current_ua = termination_current;
	ret = charger_set_prop(charger, CHARGER_PROP_CHARGE_TERM_CURRENT_UA, &val);
	if (ret == 0) {
		printk("CHARGE TERMINATION CURRENT set to : %u uA\n", val.charge_term_current_ua);
	} else if (ret == -ENOTSUP) {
		printk("CHARGE TERMINATION CURRENT property not supported\n");
	} else {
		printk("charger_set_prop() failed (%d)\n", ret);
	}
	
	ret = charger_charge_enable(charger, true);
	if (ret == 0) {
		printk("CHARGING ENABLED\n");
	} else if (ret == -ENOTSUP) {
		printk("CHARGING ENABLE property not supported\n");
	} else {
		printk("charger_charge_enable() failed (%d)\n", ret);
	}

	while (1) {

		printk("INT=%d STAT=%d\n",
			gpio_pin_get_dt(&int_gpio),
			gpio_pin_get_dt(&stat_gpio));
 
		 k_sleep(K_SECONDS(1));

		ret = charger_get_prop(charger,
				       CHARGER_PROP_ONLINE,
				       &val);

		if (ret == 0) {
			printk("ONLINE : %s\n",
			       val.online == CHARGER_ONLINE_FIXED ? "YES" : "NO");
		} else if (ret == -ENOTSUP) {
			printk("ONLINE property not supported\n");
		} else {
			printk("charger_get_prop() failed (%d)\n", ret);
		}

		ret = charger_get_prop(charger,
				       CHARGER_PROP_STATUS,
				       &val);

		if (ret == 0) {
			switch (val.status) {
			case CHARGER_STATUS_UNKNOWN:
				printk("STATUS : UNKNOWN\n");
				break;
			case CHARGER_STATUS_CHARGING:
				printk("STATUS : CHARGING\n");
				break;
			case CHARGER_STATUS_NOT_CHARGING:
				printk("STATUS : NOT CHARGING\n");
				break;
			case CHARGER_STATUS_FULL:
				printk("STATUS : CHARGE DONE\n");
				break;
			default:
				printk("STATUS : UNKNOWN\n");
				break;
			}
		} else if (ret == -ENOTSUP) {
			printk("STATUS property not supported\n");
		} else {
			printk("charger_get_prop() failed (%d)\n", ret);
		}

		ret = charger_get_prop(charger,
				       CHARGER_PROP_CHARGE_TYPE,
				       &val);
		if (ret == 0) {
			switch (val.charge_type) {
			case CHARGER_CHARGE_TYPE_UNKNOWN:
				printk("CHARGE TYPE : UNKNOWN\n");
				break;
			case CHARGER_CHARGE_TYPE_STANDARD:
				printk("CHARGE TYPE : STANDARD\n");
				break;
			default:
				printk("CHARGE TYPE : UNKNOWN\n");
				break;
			}
		} else if (ret == -ENOTSUP) {
			printk("CHARGE TYPE property not supported\n");
		} else {
			printk("charger_get_prop() failed (%d)\n", ret);
		}

		ret = charger_get_prop(charger,
				       CHARGER_PROP_HEALTH,
				       &val);
		if (ret == 0) {
			switch (val.health) {
			case CHARGER_HEALTH_UNKNOWN:
				printk("HEALTH : UNKNOWN\n");
				break;
			case CHARGER_HEALTH_GOOD:
				printk("HEALTH : GOOD\n");
				break;
			case CHARGER_HEALTH_COLD:
				printk("HEALTH : COLD\n");
				break;
			case CHARGER_HEALTH_HOT:
				printk("HEALTH : HOT\n");
				break;
			case CHARGER_HEALTH_OVERVOLTAGE:
				printk("HEALTH : OVERVOLTAGE\n");
				break;
			case CHARGER_HEALTH_UNSPEC_FAILURE:
				printk("HEALTH : UNSPEC_FAILURE\n");
				break;
			case CHARGER_HEALTH_WATCHDOG_TIMER_EXPIRE:
				printk("HEALTH : WATCHDOG_TIMER_EXPIRE\n");
				break;
			case CHARGER_HEALTH_SAFETY_TIMER_EXPIRE:
				printk("HEALTH : SAFETY_TIMER_EXPIRE\n");
				break;
			default:
				printk("HEALTH : UNKNOWN\n");
				break;
			}
		} else if (ret == -ENOTSUP) {
			printk("HEALTH property not supported\n");
		} else {
			printk("charger_get_prop() failed (%d)\n", ret);
		}

		ret = charger_get_prop(charger,
				       CHARGER_PROP_CONSTANT_CHARGE_CURRENT_UA,
				       &val);
		if (ret == 0) {
			printk("CONSTANT CHARGE CURRENT : %u uA\n",
			       val.const_charge_current_ua);
		} else if (ret == -ENOTSUP) {
			printk("CONSTANT CHARGE CURRENT property not supported\n");
		} else {
			printk("charger_get_prop() failed (%d)\n", ret);
		}

		ret = charger_get_prop(charger,
				       CHARGER_PROP_PRECHARGE_CURRENT_UA,
				       &val);
		if (ret == 0) {
			printk("PRECHARGE CURRENT : %u uA\n",
			       val.precharge_current_ua);
		} else if (ret == -ENOTSUP) {
			printk("PRECHARGE CURRENT property not supported\n");
		} else {
			printk("charger_get_prop() failed (%d)\n", ret);
		}

		ret = charger_get_prop(charger,
				       CHARGER_PROP_CHARGE_TERM_CURRENT_UA,
				       &val);
		if (ret == 0) {
			printk("CHARGE TERMINATION CURRENT : %u uA\n",
			       val.charge_term_current_ua);
		} else if (ret == -ENOTSUP) {
			printk("CHARGE TERMINATION CURRENT property not supported\n");
		} else {
			printk("charger_get_prop() failed (%d)\n", ret);
		}

		ret = charger_get_prop(charger,
				       CHARGER_PROP_CONSTANT_CHARGE_VOLTAGE_UV,
				       &val);
		if (ret == 0) {
			printk("CONSTANT CHARGE VOLTAGE : %u uV\n",
			       val.const_charge_voltage_uv);
		} else if (ret == -ENOTSUP) {
			printk("CONSTANT CHARGE VOLTAGE property not supported\n");
		} else {
			printk("charger_get_prop() failed (%d)\n", ret);
		}

		ret = charger_get_prop(charger,
				       CHARGER_PROP_INPUT_REGULATION_CURRENT_UA,
				       &val);
		if (ret == 0) {
			printk("INPUT REGULATION CURRENT : %u uA\n",
			       val.input_current_regulation_current_ua);
		} else if (ret == -ENOTSUP) {
			printk("INPUT REGULATION CURRENT property not supported\n");
		} else {
			printk("charger_get_prop() failed (%d)\n", ret);
		}

		ret = charger_get_prop(charger,
				       CHARGER_PROP_INPUT_REGULATION_VOLTAGE_UV,
				       &val);
		if (ret == 0) {
			printk("INPUT REGULATION VOLTAGE : %u uV\n",
			       val.input_voltage_regulation_voltage_uv);
		} else if (ret == -ENOTSUP) {
			printk("INPUT REGULATION VOLTAGE property not supported\n");
		} else {
			printk("charger_get_prop() failed (%d)\n", ret);
		}

		k_sleep(K_SECONDS(10));
	}

	return 0;
}
