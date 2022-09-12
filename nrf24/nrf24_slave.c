#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "nrf24.h"

#define NRF_SPI_PORT spi0
#define SPI0_CLK  18
#define SPI0_MOSI 19
#define SPI0_MISO 16
#define SPI0_CSn  17
#define NRF_CS    20


int main() {
    stdio_init_all();

    const uint LED_PIN = 25;

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    printf("Initializing NRF24...\n");
    
    NRF24 nrf24 = { .spi = NRF_SPI_PORT, 
                    .pin_NRF_CS = NRF_CS,
                    .spi_CLK = SPI0_CLK,
                    .spi_CSn = SPI0_CSn,
                    .spi_MISO = SPI0_MISO,
                    .spi_MOSI = SPI0_MOSI
                    };
    
    NRF_Init(&nrf24);
    NRF_Config("pico1", 15, 32);
    NRF_TX_Mode();
    
    //char TX_Buffer[32] = {0};
    char RX_Buffer[32] = {0};

    while (true) {
        NRF_RX_Mode();
        if(NRF_Data_Available())
        {
            NRF_ReadData(RX_Buffer);
            printf("\nRX Data : ");
            printf("%s",RX_Buffer);
        }
        sleep_ms(10);
    }
}
