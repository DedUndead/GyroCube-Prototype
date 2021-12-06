# RPI Pico doesn't work with version newer than 3.20
cmake_minimum_required(VERSION 3.12)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Set name of project and other envs
project(gyrocube C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

add_executable(${PROJECT_NAME})

# Generate pio header files
pico_generate_pio_header(${PROJECT_NAME} 
    ${CMAKE_CURRENT_LIST_DIR}/src/actuator/ws2812.pio 
    OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/inc/actuator
)

# Executables
target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/interface/i2c.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/sensor/Hih6020.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/actuator/NeoPixel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/inc/state_machine/Event.h
    ${CMAKE_CURRENT_LIST_DIR}/src/state_machine/Gyrocube.cpp
)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC
    inc/
)

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
    hardware_i2c
    hardware_pio
    hardware_adc
)

# Additionally generate python and hex pioasm outputs for inclusion in the RP2040 datasheet
add_custom_target(pio_ws2812_datasheet DEPENDS inc/actuator/ws2812.py)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_LIST_DIR}/inc/actuator/ws2812.py
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/src/actuator/ws2812.pio
    COMMAND Pioasm -o python ${CMAKE_CURRENT_LIST_DIR}/src/actuator/ws2812.pio ${CMAKE_CURRENT_LIST_DIR}/inc/actuator/ws2812.py
)
add_dependencies(${PROJECT_NAME} pio_ws2812_datasheet)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)