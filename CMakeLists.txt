# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(bitdoglab-app C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

file(GLOB INCLUDE "include/*.c")
file(GLOB INTERFACE "interface/*.c")
file(GLOB SRC "src/*.c")

add_executable(bitdoglab-app ${INCLUDE} ${INTERFACE} ${SRC} )

pico_set_program_name(bitdoglab-app "bitdoglab-app")
pico_set_program_version(bitdoglab-app "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(bitdoglab-app 1)
pico_enable_stdio_usb(bitdoglab-app 1)

pico_generate_pio_header(bitdoglab-app ${CMAKE_CURRENT_LIST_DIR}/include/ws2818b.pio)

# Add the standard library to the build
target_link_libraries(bitdoglab-app
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(bitdoglab-app PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(bitdoglab-app 
        hardware_clocks
        hardware_pio
        hardware_pwm
        hardware_i2c
        hardware_adc
        )

pico_add_extra_outputs(bitdoglab-app)

