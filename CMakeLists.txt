cmake_minimum_required(VERSION 3.12)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(PRJ_NAME hong_rp2040_boot)

set(PICO_SDK_PATH $ENV{PICO_SDK_LIB_PATH})
set(PICO_EXTRAS_PATH $ENV{PICO_SDK_LIB_EXTRAS_PATH})

# set(PICOSYSTEM_DIR $ENV{PICOSYSTEM_DIR})
set(PICO_BOARD_HEADER_DIRS "${CMAKE_SOURCE_DIR}/src/bsp/board")
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
# include(${PICO_EXTRAS_PATH}/external/pico_extras_import.cmake)

set( BOARD_ONLY_UART "0")
set( WS2812_NEOPIXEL_USE "0")


project(${PRJ_NAME})

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized 
        )

add_subdirectory(src)
