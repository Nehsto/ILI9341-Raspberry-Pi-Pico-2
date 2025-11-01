message("LIBRARY_NAME = ${DIR_NAME_NOPATH}_driver")

if(SKIP_SD)
    add_library(${DIR_NAME_NOPATH}_driver
        ILI9341.c
    )

    target_include_directories(${DIR_NAME_NOPATH}_driver PUBLIC
        ../include
    )

    target_link_libraries(${DIR_NAME_NOPATH}_driver
        hardware_spi 
        pico_stdlib
        pico_stdio_usb
    )

else()
    add_library(${DIR_NAME_NOPATH}_driver
        ILI9341.c
        ImageReader.c
    )
        
    target_sources(${DIR_NAME_NOPATH}_driver INTERFACE
        $ENV{HOME}/pico/libraries/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/include
        $ENV{HOME}/pico/libraries/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/ff15/source
        $ENV{HOME}/pico/libraries/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src
    )

    target_include_directories(${DIR_NAME_NOPATH}_driver PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/../include
        ${CMAKE_CURRENT_LIST_DIR}/../include/ImageReader
    )

    target_link_libraries(${DIR_NAME_NOPATH}_driver
        pico_stdlib
        hardware_spi
        hardware_pwm
        pico_stdio_usb
        no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
    )
endif()