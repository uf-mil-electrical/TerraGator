# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

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
set(PICO_BOARD none CACHE STRING "Board type")
set(PICO_PLATFORM rp2350-arm-s CACHE STRING "Platform")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Rover-Control-Board C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(Rover-Control-Board main.c)

# Add all .c files here
target_sources(Rover-Control-Board PRIVATE
    communication/src/data_parser.c
    communication/src/rover_i2c.c

    control/src/auto_control.c
    control/src/rover_cli.c

    peripherals/src/motor_speed_control.c
    peripherals/src/safety_devices.c
)

pico_set_program_name(Rover-Control-Board "Rover-Control-Board")
pico_set_program_version(Rover-Control-Board "2.0")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Rover-Control-Board 0)
pico_enable_stdio_usb(Rover-Control-Board 1)

# Add the standard library to the build
target_link_libraries(Rover-Control-Board
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(Rover-Control-Board PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(Rover-Control-Board 
        hardware_spi
        hardware_i2c
        hardware_pwm
        )

pico_add_extra_outputs(Rover-Control-Board)

