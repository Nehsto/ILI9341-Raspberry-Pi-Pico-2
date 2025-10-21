add_library(ImageReader_driver 
    ILI9341.c
    ImageReader.c
)
    
target_include_directories(ImageReader_driver PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/ImageReader
    $ENV{HOME}/pico/libraries/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/include
    $ENV{HOME}/libraries/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/ff15/source
    $ENV{HOME}/pico/libraries/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src
)

target_link_libraries(ImageReader_driver
    hardware_spi 
    pico_stdlib
)