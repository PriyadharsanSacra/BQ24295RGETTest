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

		k_sleep(K_SECONDS(2));
	}

	return 0;
}