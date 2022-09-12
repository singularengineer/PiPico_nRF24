add_executable(nrf24_master
            nrf24_master.c
            nrf24.h
            nrf24.c
        )
add_executable(nrf24_slave
        nrf24_slave.c
        nrf24.h
        nrf24.c
    )

#pull in common dependencies and additional spi hardware support
target_link_libraries(nrf24_master pico_stdlib hardware_spi)
target_link_libraries(nrf24_slave pico_stdlib hardware_spi)

#target_compile_definitions(nrf24 PRIVATE PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS=10)

pico_enable_stdio_usb(nrf24_master 1)
pico_enable_stdio_uart(nrf24_master 0)
pico_enable_stdio_usb(nrf24_slave 1)
pico_enable_stdio_uart(nrf24_slave 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(nrf24_master)
pico_add_extra_outputs(nrf24_slave)
