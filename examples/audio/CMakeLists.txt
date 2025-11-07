# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
cmake_minimum_required(VERSION 3.13)

set(picotool_DIR $MY_INSTALL_DIR/picotool)
set(PICO_BOARD pico2)
set(PICO_PLATFORM rp2350)

include(pico_sdk_import.cmake)

set(DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR})
get_filename_component(DIR_NAME_NOPATH ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${DIR_NAME_NOPATH}_example C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

set(ADD_SD TRUE)
set(ADD_I2S TRUE)

add_subdirectory(../../src 
    ${CMAKE_CURRENT_BINARY_DIR}/src
)

message("PROJECT_NAME = ${PROJECT_NAME}")
add_executable(${PROJECT_NAME}
	main.c
    hw_config.c
)

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/../../src/i2s.pio)

add_subdirectory($ENV{HOME}/pico/libraries/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src
    ${CMAKE_CURRENT_BINARY_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
)

# Pull in common dependencies
target_link_libraries(${PROJECT_NAME} 
    ${DIR_NAME_NOPATH}_driver
)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(${PROJECT_NAME})