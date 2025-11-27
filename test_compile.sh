#!/bin/bash

# Test compilation script for DA7281 HAL on Raspberry Pi 5
# This verifies the source files compile correctly with the Qorvo SDK

SDK_PATH="${HOME}/DW3_QM33_SDK/SDK_BSP/Nordic/SDK_17_1_0"

# Compiler flags
CFLAGS="-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16"
CFLAGS="${CFLAGS} -Wall -Werror -O2"
CFLAGS="${CFLAGS} -DNRF52833_XXAA -DFREERTOS -DBOARD_CUSTOM"

# Include paths
INCLUDES="-I include/"
INCLUDES="${INCLUDES} -I config/"
INCLUDES="${INCLUDES} -I ${SDK_PATH}/components/toolchain/cmsis/include"
INCLUDES="${INCLUDES} -I ${SDK_PATH}/components/drivers_nrf/twi_master"
INCLUDES="${INCLUDES} -I ${SDK_PATH}/components/libraries/util"
INCLUDES="${INCLUDES} -I ${SDK_PATH}/components/libraries/log"
INCLUDES="${INCLUDES} -I ${SDK_PATH}/components/libraries/log/src"
INCLUDES="${INCLUDES} -I ${SDK_PATH}/components/libraries/experimental_section_vars"
INCLUDES="${INCLUDES} -I ${SDK_PATH}/components/libraries/strerror"
INCLUDES="${INCLUDES} -I ${SDK_PATH}/components/libraries/delay"
INCLUDES="${INCLUDES} -I ${SDK_PATH}/components/libraries/atomic"
INCLUDES="${INCLUDES} -I ${SDK_PATH}/components/libraries/memobj"
INCLUDES="${INCLUDES} -I ${SDK_PATH}/components/libraries/balloc"
INCLUDES="${INCLUDES} -I ${SDK_PATH}/components/libraries/ringbuf"
INCLUDES="${INCLUDES} -I ${SDK_PATH}/components/softdevice/s113/headers"
INCLUDES="${INCLUDES} -I ${SDK_PATH}/components/softdevice/s113/headers/nrf52"
INCLUDES="${INCLUDES} -I ${SDK_PATH}/external/freertos/source/include"
INCLUDES="${INCLUDES} -I ${SDK_PATH}/external/freertos/portable/GCC/nrf52"
INCLUDES="${INCLUDES} -I ${SDK_PATH}/external/freertos/portable/CMSIS/nrf52"
INCLUDES="${INCLUDES} -I ${SDK_PATH}/examples/peripheral/blinky_freertos/config"
INCLUDES="${INCLUDES} -I ${SDK_PATH}/modules/nrfx"
INCLUDES="${INCLUDES} -I ${SDK_PATH}/modules/nrfx/mdk"
INCLUDES="${INCLUDES} -I ${SDK_PATH}/modules/nrfx/hal"
INCLUDES="${INCLUDES} -I ${SDK_PATH}/modules/nrfx/drivers/include"
INCLUDES="${INCLUDES} -I ${SDK_PATH}/integration/nrfx"
INCLUDES="${INCLUDES} -I ${SDK_PATH}/integration/nrfx/legacy"

echo "========================================="
echo "DA7281 HAL Compilation Test"
echo "========================================="
echo ""

# Clean previous build
rm -f *.o

# Compile da7281.c
echo "[1/2] Compiling da7281.c..."
arm-none-eabi-gcc -c src/da7281.c ${CFLAGS} ${INCLUDES} -o da7281.o
if [ $? -eq 0 ]; then
    echo "✓ da7281.c compiled successfully"
    ls -lh da7281.o
else
    echo "✗ da7281.c compilation FAILED"
    exit 1
fi

echo ""

# Compile da7281_i2c.c
echo "[2/2] Compiling da7281_i2c.c..."
arm-none-eabi-gcc -c src/da7281_i2c.c ${CFLAGS} ${INCLUDES} -o da7281_i2c.o
if [ $? -eq 0 ]; then
    echo "✓ da7281_i2c.c compiled successfully"
    ls -lh da7281_i2c.o
else
    echo "✗ da7281_i2c.c compilation FAILED"
    exit 1
fi

echo ""
echo "========================================="
echo "✓ ALL FILES COMPILED SUCCESSFULLY!"
echo "========================================="
echo ""
echo "Object files created:"
ls -lh *.o
echo ""
echo "Total size:"
size *.o

