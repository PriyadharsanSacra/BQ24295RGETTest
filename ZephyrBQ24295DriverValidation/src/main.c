/*
 * Copyright 2026 Sacra Systems Private Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/charger.h>

#define BQ24295_NODE DT_NODELABEL(bq24295)

int main(void)
{
	const struct device *charger = DEVICE_DT_GET(BQ24295_NODE);
	union charger_propval val;
	int ret;

	printk("\n=== BQ24295 Charger Driver Validation ===\n");

	if (!device_is_ready(charger)) {
		printk("BQ24295 device not ready\n");
		return 0;
	}

	printk("BQ24295 device ready\n");

	while (1) {

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

		uint32_t new_input_current = 900000; /* 3000 mA */
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

		ret = charger_charge_enable(charger, false);
		if (ret == 0) {
			printk("CHARGING DISABLED\n");
		} else if (ret == -ENOTSUP) {
			printk("CHARGING DISABLE property not supported\n");
		} else {
			printk("charger_charge_enable() failed (%d)\n", ret);
		}

		uint32_t precharge_current = 512000; /* 512 mA */
		val.precharge_current_ua = precharge_current;
		ret = charger_set_prop(charger, CHARGER_PROP_PRECHARGE_CURRENT_UA, &val);
		if (ret == 0) {
			printk("PRECHARGE CURRENT set to : %u uA\n", val.precharge_current_ua);
		} else if (ret == -ENOTSUP) {
			printk("PRECHARGE CURRENT property not supported\n");
		} else {
			printk("charger_set_prop() failed (%d)\n", ret);
		}

		uint32_t termination_current = 512000; /* 512 mA */
		val.charge_term_current_ua = termination_current;
		ret = charger_set_prop(charger, CHARGER_PROP_CHARGE_TERM_CURRENT_UA, &val);
		if (ret == 0) {
			printk("CHARGE TERMINATION CURRENT set to : %u uA\n", val.charge_term_current_ua);
		} else if (ret == -ENOTSUP) {
			printk("CHARGE TERMINATION CURRENT property not supported\n");
		} else {
			printk("charger_set_prop() failed (%d)\n", ret);
		}

		k_sleep(K_SECONDS(10));
	}

	return 0;
}
