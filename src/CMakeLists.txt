add_library(ImageReader_driver
    ILI9341.c
    ImageReader.c
)
    
target_include_directories(ImageReader_driver PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/ImageReader
)

target_link_libraries(ImageReader_driver
    hardware_spi 
    pico_stdlib
)