#include <zephyr/kernel.h>
#include <stdio.h>

int main(void) {
    k_msleep(2000); 
    printk("\n*** BQ24295 Validator Active ***\n");

    while (1) {
        printk("\n*** BQ24295 Validator Running ***\n");
        k_msleep(1000); 
    }

    return 0;
}