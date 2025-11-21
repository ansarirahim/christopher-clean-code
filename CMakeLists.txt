cmake_minimum_required(VERSION 3.20)

# Project definition
project(da7281_dwm3001c_hal 
        VERSION 1.0.0
        LANGUAGES C ASM)

# C Standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Options
option(ENABLE_TESTS "Enable unit tests" OFF)
option(ENABLE_COVERAGE "Enable code coverage" OFF)
option(ENABLE_EXAMPLES "Build example applications" ON)

# ========================================================================
# Compiler Settings
# ========================================================================

# ARM Cortex-M4 with FPU
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# Compiler flags for nRF52833
set(CPU_FLAGS "-mcpu=cortex-m4 -mthumb -mabi=aapcs -mfloat-abi=hard -mfpu=fpv4-sp-d16")

set(CMAKE_C_FLAGS "${CPU_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-strict-aliasing -fno-builtin")

set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS "${CPU_FLAGS} -Wl,--gc-sections")

# ========================================================================
# Qorvo SDK Configuration
# ========================================================================

# Set Qorvo SDK path (user should set this)
set(QORVO_SDK_PATH "${CMAKE_SOURCE_DIR}/../qorvo_sdk" CACHE PATH "Path to Qorvo DWM3001CDK SDK")

if(EXISTS "${QORVO_SDK_PATH}")
    message(STATUS "Qorvo SDK found at: ${QORVO_SDK_PATH}")
    
    # SDK include directories
    set(SDK_INCLUDES
        ${QORVO_SDK_PATH}/components/drivers_nrf/twi_master
        ${QORVO_SDK_PATH}/components/libraries/util
        ${QORVO_SDK_PATH}/components/libraries/log
        ${QORVO_SDK_PATH}/modules/nrfx
        ${QORVO_SDK_PATH}/modules/nrfx/hal
        ${QORVO_SDK_PATH}/modules/nrfx/drivers/include
        ${QORVO_SDK_PATH}/integration/nrfx
        ${QORVO_SDK_PATH}/external/freertos/source/include
        ${QORVO_SDK_PATH}/external/freertos/portable/GCC/ARM_CM4F
    )
else()
    message(WARNING "Qorvo SDK not found. Set QORVO_SDK_PATH to build.")
endif()

# ========================================================================
# DA7281 HAL Library
# ========================================================================

# Source files
set(DA7281_SOURCES
    src/da7281.c
    src/da7281_i2c.c
)

# Header files
set(DA7281_HEADERS
    include/da7281.h
    include/da7281_registers.h
    include/da7281_config.h
)

# Create library
add_library(da7281_hal STATIC ${DA7281_SOURCES})

target_include_directories(da7281_hal
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${SDK_INCLUDES}
)

# Compiler definitions
target_compile_definitions(da7281_hal
    PUBLIC
        NRF52833_XXAA
        BOARD_DWM3001CDK
        FREERTOS
        DA7281_ENABLE_DEBUG_LOG=1
        DA7281_ENABLE_PARAM_CHECK=1
)

# ========================================================================
# Examples
# ========================================================================

if(ENABLE_EXAMPLES AND EXISTS "${QORVO_SDK_PATH}")
    add_executable(haptics_demo
        examples/haptics_demo.c
    )
    
    target_link_libraries(haptics_demo
        PRIVATE
            da7281_hal
    )
    
    target_include_directories(haptics_demo
        PRIVATE
            ${SDK_INCLUDES}
    )
endif()

# ========================================================================
# Unit Tests
# ========================================================================

if(ENABLE_TESTS)
    enable_testing()
    
    # Unity test framework (would need to be downloaded)
    set(UNITY_PATH "${CMAKE_SOURCE_DIR}/external/unity" CACHE PATH "Path to Unity test framework")
    
    if(EXISTS "${UNITY_PATH}")
        add_subdirectory(tests/unit)
    else()
        message(WARNING "Unity test framework not found. Tests disabled.")
    endif()
endif()

# ========================================================================
# Code Coverage
# ========================================================================

if(ENABLE_COVERAGE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    
    add_custom_target(coverage
        COMMAND lcov --capture --directory . --output-file coverage.info
        COMMAND lcov --remove coverage.info '/usr/*' --output-file coverage.info
        COMMAND lcov --list coverage.info
        COMMAND genhtml coverage.info --output-directory coverage
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating code coverage report"
    )
endif()

# ========================================================================
# Installation
# ========================================================================

install(TARGETS da7281_hal
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib)

install(FILES ${DA7281_HEADERS}
        DESTINATION include/da7281)

# ========================================================================
# Summary
# ========================================================================

message(STATUS "")
message(STATUS "DA7281 HAL Configuration Summary:")
message(STATUS "  Version:        ${PROJECT_VERSION}")
message(STATUS "  Build type:     ${CMAKE_BUILD_TYPE}")
message(STATUS "  C Compiler:     ${CMAKE_C_COMPILER}")
message(STATUS "  Enable tests:   ${ENABLE_TESTS}")
message(STATUS "  Enable coverage: ${ENABLE_COVERAGE}")
message(STATUS "  Enable examples: ${ENABLE_EXAMPLES}")
message(STATUS "")

