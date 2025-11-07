message("Name of library is ${DIR_NAME_NOPATH}_driver")
if(ADD_SD)
    message("ADD_SD is TRUE")
else()
    message("ADD_SD is FALSE")
endif()
if(ADD_I2S)
    message("ADD_I2S is TRUE")
else()
    message("ADD_I2S is FALSE")
endif()

if(ADD_SD AND ADD_I2S)
    add_library(${DIR_NAME_NOPATH}_driver
        ILI9341.c
        ImageReader.c
        fileread.c
        i2sPlayer.c
        i2s.c
    )
elseif(ADD_SD)
    add_library(${DIR_NAME_NOPATH}_driver
        ILI9341.c
        ImageReader.c
        fileread.c
    )
elseif(ADD_I2S)
    add_library(${DIR_NAME_NOPATH}_driver
        ILI9341.c
        i2s.c
        i2sPlayer.c
    )
else()
    add_library(${DIR_NAME_NOPATH}_driver
        ILI9341.c
    )
endif()

if(ADD_SD AND ADD_I2S)
    target_sources(${DIR_NAME_NOPATH}_driver INTERFACE
        $ENV{HOME}/pico/libraries/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/include
        $ENV{HOME}/pico/libraries/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/ff15/source
        $ENV{HOME}/pico/libraries/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src
    )
    target_include_directories(${DIR_NAME_NOPATH}_driver PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/../include
        ${CMAKE_CURRENT_LIST_DIR}/../include/ImageReader
        ${CMAKE_CURRENT_LIST_DIR}/../include/i2sPlayer
        ${DIR_NAME}/build
    )
elseif(ADD_SD)
    target_sources(${DIR_NAME_NOPATH}_driver INTERFACE
        $ENV{HOME}/pico/libraries/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/include
        $ENV{HOME}/pico/libraries/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/ff15/source
        $ENV{HOME}/pico/libraries/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src
    )
    target_include_directories(${DIR_NAME_NOPATH}_driver PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/../include
        ${CMAKE_CURRENT_LIST_DIR}/../include/ImageReader
    )
elseif(ADD_I2S)
    target_include_directories(${DIR_NAME_NOPATH}_driver PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/../include
        ${CMAKE_CURRENT_LIST_DIR}/../include/i2sPlayer
        ${DIR_NAME}/build
    )
else()
    target_include_directories(${DIR_NAME_NOPATH}_driver PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/../include
    )
endif()

if(ADD_SD)
    target_link_libraries(${DIR_NAME_NOPATH}_driver
        hardware_spi
        hardware_pwm
        pico_stdlib
        pico_stdio_usb
        no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
    )
else()
    target_link_libraries(${DIR_NAME_NOPATH}_driver
        hardware_spi
        hardware_pwm
        pico_stdlib
        pico_stdio_usb
    )
endif()