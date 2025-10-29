# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
# if(WIN32)
#     set(USERHOME $ENV{USERPROFILE})
# else()
#     set(USERHOME $ENV{HOME})
# endif()
# set(sdkVersion 2.2.0)
# set(toolchainVersion 14_2_Rel1)
# set(picotoolVersion 2.2.0)
# set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
# if (EXISTS ${picoVscode})
#     include(${picoVscode})
# endif()
# ====================================================================================
cmake_minimum_required(VERSION 3.28)

set(picotool_DIR $MY_INSTALL_DIR/picotool)
set(PICO_BOARD pico2)
set(PICO_PLATFORM rp2350)

# Pull in Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(ImageReader_example C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

set(SKIP_SD False)

add_subdirectory(../../src 
    ${CMAKE_CURRENT_BINARY_DIR}/src
)

add_executable(ImageReader_example
	main.c
    hw_config.c
)
# Can leave these off for silent mode:
# add_compile_definitions(USE_PRINTF USE_DBG_PRINTF).
# add_compile_definitions(USE_PRINTF)

add_subdirectory($ENV{HOME}/pico/libraries/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src
    ${CMAKE_CURRENT_BINARY_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
)

# Pull in common dependencies
target_link_libraries(ImageReader_example 
    pico_stdlib 
    hardware_spi
    pico_stdio_usb
    ImageReader_driver
    no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
)

pico_enable_stdio_usb(ImageReader_example 0)
pico_enable_stdio_uart(ImageReader_example 1)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(ImageReader_example)