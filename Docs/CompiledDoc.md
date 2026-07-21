# BQ24295 Zephyr Driver - Development Commands

## Build the application

```bash
west build -p always -b esp32s3_devkitc/esp32s3/procpu .
```

---

## Flash the application

```bash
west flash
```

---

## Run Twister build test

```bash
./scripts/twister -T tests/drivers/build_all/charger
```

---

## Find unused variable warnings from the Twister build log

```bash
grep -n "unused variable" \
/home/sacrasystems/zephyrproject/zephyr/twister-out/native_sim_native/host_gnu/tests/drivers/build_all/charger/drivers.charger.build/build.log
```

---

## Format the source code

```bash
clang-format -i drivers/charger/charger_bq24295.c
```

---

## Perform code style review

```bash
./scripts/checkpatch.pl --file drivers/charger/charger_bq24295.c
```