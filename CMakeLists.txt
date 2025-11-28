cmake_minimum_required(VERSION 3.20)

# ======================================================================
# Auto-select ARM toolchain if not provided (Christopher's addition)
# ======================================================================

if (NOT CMAKE_C_COMPILER)
    set(ARM_TOOLCHAIN arm-none-eabi-gcc)
    set(ARM_ASM_TOOLCHAIN arm-none-eabi-gcc)

    message(STATUS "Using default ARM compiler: ${ARM_TOOLCHAIN}")

    # Tell CMake we are cross-compiling
    set(CMAKE_SYSTEM_NAME Generic)
    set(CMAKE_SYSTEM_PROCESSOR ARM)

    # Direct compiler selection
    set(CMAKE_C_COMPILER   ${ARM_TOOLCHAIN})
    set(CMAKE_ASM_COMPILER ${ARM_ASM_TOOLCHAIN})

    # Needed to avoid running test binaries
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
endif()

# Set toolchain before project()
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# Toolchain settings
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(CMAKE_SIZE arm-none-eabi-size)

# Skip compiler tests (embedded targets can't run test programs)
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

project(da7281_hal VERSION 1.0.0 LANGUAGES C ASM)

# Qorvo SDK path
if(NOT DEFINED QORVO_SDK_PATH)
    set(QORVO_SDK_PATH "$ENV{HOME}/DW3_QM33_SDK/SDK_BSP/Nordic/SDK_17_1_0")
endif()

message(STATUS "Qorvo SDK path: ${QORVO_SDK_PATH}")

# MCU settings
set(MCU_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")

# Compiler flags
set(CMAKE_C_FLAGS "${MCU_FLAGS} -Wall -Werror -O2 -g")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNRF52833_XXAA -DFREERTOS -DBOARD_CUSTOM")

# Linker flags (for future linking)
set(CMAKE_EXE_LINKER_FLAGS "${MCU_FLAGS} -Wl,--gc-sections")

# Include directories
set(INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/config
    ${QORVO_SDK_PATH}/components/toolchain/cmsis/include
    ${QORVO_SDK_PATH}/components/drivers_nrf/twi_master
    ${QORVO_SDK_PATH}/components/libraries/util
    ${QORVO_SDK_PATH}/components/libraries/log
    ${QORVO_SDK_PATH}/components/libraries/log/src
    ${QORVO_SDK_PATH}/components/libraries/experimental_section_vars
    ${QORVO_SDK_PATH}/components/libraries/strerror
    ${QORVO_SDK_PATH}/components/libraries/delay
    ${QORVO_SDK_PATH}/components/libraries/atomic
    ${QORVO_SDK_PATH}/components/libraries/memobj
    ${QORVO_SDK_PATH}/components/libraries/balloc
    ${QORVO_SDK_PATH}/components/libraries/ringbuf
    ${QORVO_SDK_PATH}/components/softdevice/s113/headers
    ${QORVO_SDK_PATH}/components/softdevice/s113/headers/nrf52
    ${QORVO_SDK_PATH}/external/freertos/source/include
    ${QORVO_SDK_PATH}/external/freertos/portable/GCC/nrf52
    ${QORVO_SDK_PATH}/external/freertos/portable/CMSIS/nrf52
    ${QORVO_SDK_PATH}/examples/peripheral/blinky_freertos/config
    ${QORVO_SDK_PATH}/modules/nrfx
    ${QORVO_SDK_PATH}/modules/nrfx/mdk
    ${QORVO_SDK_PATH}/modules/nrfx/hal
    ${QORVO_SDK_PATH}/modules/nrfx/drivers/include
    ${QORVO_SDK_PATH}/integration/nrfx
    ${QORVO_SDK_PATH}/integration/nrfx/legacy
)

# DA7281 HAL library (object files only)
add_library(da7281_hal OBJECT
    src/da7281.c
    src/da7281_i2c.c
)

target_include_directories(da7281_hal PUBLIC ${INCLUDE_DIRS})

# Print build info
message(STATUS "")
message(STATUS "DA7281 HAL Configuration:")
message(STATUS "  Version:        ${PROJECT_VERSION}")
message(STATUS "  C Compiler:     ${CMAKE_C_COMPILER}")
message(STATUS "  MCU Flags:      ${MCU_FLAGS}")
message(STATUS "")

# Custom target to show size
add_custom_target(size
    COMMAND ${CMAKE_SIZE} $<TARGET_OBJECTS:da7281_hal>
    DEPENDS da7281_hal
    COMMENT "Showing object file sizes"
)

# Install headers and objects
install(FILES
    include/da7281.h
    include/da7281_registers.h
    include/da7281_config.h
    DESTINATION include
)

install(FILES
    $<TARGET_OBJECTS:da7281_hal>
    DESTINATION lib
)

# Print instructions
add_custom_target(info
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "========================================="
    COMMAND ${CMAKE_COMMAND} -E echo "DA7281 HAL Build Complete"
    COMMAND ${CMAKE_COMMAND} -E echo "========================================="
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Object files created:"
    COMMAND ls -lh $<TARGET_OBJECTS:da7281_hal>
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "To see sizes: make size"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    DEPENDS da7281_hal
)
