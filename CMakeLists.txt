# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
cmake_minimum_required(VERSION 3.13)

# Include the Pico SDK
include(pico_sdk_import.cmake)

project(DualSensor C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Target the RP2350-based board (pico2). This ensures the Pico SDK
# configures the correct platform (rp2350-arm-s) and uses the right
# USB/clock settings. You can override this with -DPICO_BOARD if needed.
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Initialize the SDK
pico_sdk_init()

# Define your executable and the source files it uses
add_executable(DualSensor 
    DualSensor.cpp
    driver/Adafruit_VL53L1X.cpp
    driver/vl53l1x_class.cpp
)

# Pull in the hardware libraries we need from the SDK
target_link_libraries(DualSensor 
    pico_stdlib 
    hardware_gpio 
    hardware_i2c 
    hardware_timer
)

# Enable USB Serial Monitor output, disable UART
pico_enable_stdio_usb(DualSensor 1)
pico_enable_stdio_uart(DualSensor 0)

# Create the .uf2 file for flashing
pico_add_extra_outputs(DualSensor)