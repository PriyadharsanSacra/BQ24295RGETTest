#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>

#include <stdio.h>

#define I2C_NODE DT_NODELABEL(i2c0)

static void dump_registers(const struct device *i2c)
{
    uint8_t reg;
    uint8_t val;

    printk("\nBQ24295 Register Dump\n");
    printk("---------------------\n");

    for (reg = 0; reg <= 0x0A; reg++) {

        if (i2c_write_read(i2c,
                           0x6B,
                           &reg,
                           1,
                           &val,
                           1) == 0) {

            printk("Reg[0x%02X] = 0x%02X\n", reg, val);

        } else {

            printk("Reg[0x%02X] = ERROR\n", reg);
        }
    }
}

int main(void) {
    k_msleep(2000); 
    printk("\n*** BQ24295 Validator Active ***\n");

    const struct device *i2c = DEVICE_DT_GET(I2C_NODE);

    if (!device_is_ready(i2c)) {
        printk("I2C not ready\n");
        return 0;
    }

    printk("I2C ready\n");

    uint8_t reg = 0x00;
    int ret;

    ret = i2c_write(i2c, &reg, 1, 0x6B);
    printk("write ret = %d\n", ret);

    uint8_t val;
    ret = i2c_read(i2c, &val, 1, 0x6B);
    printk("read ret = %d val = 0x%02X\n", ret, val);

    dump_registers(i2c);

    while (1) {
        printk("BQ24295 Validator Running...\n");
        k_msleep(1000);
    }

    return 0;
}