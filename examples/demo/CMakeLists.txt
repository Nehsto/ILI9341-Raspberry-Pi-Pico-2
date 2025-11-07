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
set(PICO_BOARD pico2 CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

get_filename_component(DIR_NAME_NOPATH ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${DIR_NAME_NOPATH}_example C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

set(ADD_SD TRUE)
set(ADD_I2S FALSE)

add_subdirectory(../../src 
    ${CMAKE_CURRENT_BINARY_DIR}/src
)

add_executable(${PROJECT_NAME}
	main.c
)

# Pull in common dependencies
target_link_libraries(${PROJECT_NAME} 
    pico_stdlib 
    ${DIR_NAME_NOPATH}_driver
)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} pico_stdlib)