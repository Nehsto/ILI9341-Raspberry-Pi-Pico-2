if(SKIP_SD)
    add_library(ILI9341_driver 
        ILI9341.c
    )

    target_include_directories(ILI9341_driver PUBLIC
        ../include
    )

    target_link_libraries(ILI9341_driver
        hardware_spi 
        pico_stdlib
        pico_stdio_usb
    )

else()
    add_library(ImageReader_driver 
        ILI9341.c
        ImageReader.c
    )
        
    target_sources(ImageReader_driver INTERFACE
        $ENV{HOME}/pico/libraries/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/include
        $ENV{HOME}/pico/libraries/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/ff15/source
        $ENV{HOME}/pico/libraries/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src
    )

    target_include_directories(ImageReader_driver PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/../include
        ${CMAKE_CURRENT_LIST_DIR}/../include/ImageReader
    )

    target_link_libraries(ImageReader_driver
        hardware_spi 
        pico_stdlib
        pico_stdio_usb
        no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
    )
endif()