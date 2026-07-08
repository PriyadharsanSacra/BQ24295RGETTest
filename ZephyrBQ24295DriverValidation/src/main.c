#include <zephyr/kernel.h>
#include <zephyr/device.h>

#define BQ24295_NODE DT_NODELABEL(bq24295)

int main(void)
{
	const struct device *dev = DEVICE_DT_GET(BQ24295_NODE);

	printk("\nBQ24295 Driver Validation\n");

	if (!device_is_ready(dev)) {
		printk("BQ24295 device not ready\n");
		return 0;
	}

	printk("BQ24295 device ready\n");

	while (1) {
		k_sleep(K_SECONDS(5));
	}
}