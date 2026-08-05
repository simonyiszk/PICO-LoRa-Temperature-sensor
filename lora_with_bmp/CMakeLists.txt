cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

set(PICO_BOARD pico CACHE STRING "Board type")

include(pico_sdk_import.cmake)

project(lora_with_bmp C CXX ASM)

pico_sdk_init()

# List all source files here
add_executable(lora_with_bmp 
    lora_with_bmp.c
    lora_handler.c
    bmp_handler.c
    battery.c
)

pico_set_program_name(lora_with_bmp "lora_with_bmp")
pico_set_program_version(lora_with_bmp "0.1")

pico_enable_stdio_uart(lora_with_bmp 1)
pico_enable_stdio_usb(lora_with_bmp 1)

# Direct CMake to look in the current directory for custom header files (.h)
target_include_directories(lora_with_bmp PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Combine all Pico SDK hardware libraries into one block
target_link_libraries(lora_with_bmp
    pico_stdlib
    hardware_i2c
    hardware_uart
    hardware_adc
)

pico_add_extra_outputs(lora_with_bmp)