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

		k_sleep(K_SECONDS(2));
	}

	return 0;
}