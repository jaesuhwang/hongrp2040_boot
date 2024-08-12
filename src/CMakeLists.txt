
file(GLOB SRC_FILES 
  bsp/*.c
  hw/*.c
  hw/driver/*.c
  ap/modules/*.c
  ap/modules/boot/*.c
  ap/modules/cmd/*.c
)

file(GLOB_RECURSE SRC_FILES_RECURSE
  ap/*.c
  ap/*.cpp
  common/*.c 
)

add_executable(
  ${PRJ_NAME}
  
  main.cpp

  ${SRC_FILES}
  ${SRC_FILES_RECURSE}
)

target_include_directories(${PRJ_NAME} PUBLIC 
  ap
  ap/modules
  ap/modules/boot
  ap/modules/cmd
  bsp
  common 
  common/core
  common/hw/include 
  hw
)
target_link_libraries(${PRJ_NAME} 
  cmsis_core
  pico_stdlib
  hardware_spi
  hardware_dma
  hardware_irq
  hardware_i2c
  hardware_flash

)

if (WS2812_NEOPIXEL_USE )
  target_link_libraries(${PRJ_NAME} 
    hardware_pio
  )
endif()


pico_add_extra_outputs(${PRJ_NAME})

if(BOARD_ONLY_UART)
  pico_enable_stdio_usb(${PRJ_NAME}  0)
  pico_enable_stdio_uart(${PRJ_NAME} 1)
else()
  pico_enable_stdio_usb(${PRJ_NAME}  1)
  pico_enable_stdio_uart(${PRJ_NAME} 0)
endif()


add_compile_definitions(BOARD_ONLY_UART_USAGE=${BOARD_ONLY_UART})

pico_set_linker_script(${PRJ_NAME} "${CMAKE_SOURCE_DIR}/src/bsp/ldscript/pico_flash.ld")

if ( WS2812_NEOPIXEL_USE )
  pico_generate_pio_header(${PRJ_NAME} ${CMAKE_CURRENT_LIST_DIR}/hw/driver/pio/ws2812.pio)
endif()


target_link_options(${PRJ_NAME} PRIVATE -Xlinker --print-memory-usage)

set_target_properties(${PRJ_NAME} PROPERTIES SUFFIX ".elf")
add_custom_command(TARGET ${PRJ_NAME} POST_BUILD
        COMMAND arm-none-eabi-size --format=berkeley --totals "$<TARGET_FILE:${PRJ_NAME}>"
        COMMENT "Invoking: Cross ARM GNU Print Size")


        