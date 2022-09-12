#include "nrf24.h"

NRF24 *NRFmodule;

void NRF_Init(NRF24 *nrf)
{
    sleep_ms(11);

    NRFmodule = nrf;
    
    spi_init(NRFmodule->spi, 1000 * 1000);

    gpio_set_function(NRFmodule->spi_CLK, GPIO_FUNC_SPI);
    gpio_set_function(NRFmodule->spi_MISO, GPIO_FUNC_SPI);
    gpio_set_function(NRFmodule->spi_MOSI, GPIO_FUNC_SPI);

    gpio_init(NRFmodule->spi_CSn);
    gpio_init(NRFmodule->pin_NRF_CS);

    gpio_set_dir(NRFmodule->spi_CSn, GPIO_OUT);
    gpio_set_dir(NRFmodule->pin_NRF_CS, GPIO_OUT);

    NRF_CE_LOW();
    CSn_HIGH();
}

void CSn_LOW()
{
    asm volatile("nop \n nop \n nop");
    gpio_put(NRFmodule->spi_CSn, 0);
    asm volatile("nop \n nop \n nop");
}

void CSn_HIGH()
{
    asm volatile("nop \n nop \n nop");
    gpio_put(NRFmodule->spi_CSn, 1);
    asm volatile("nop \n nop \n nop");
}

void NRF_CE_LOW()
{
    asm volatile("nop \n nop \n nop");
    gpio_put(NRFmodule->pin_NRF_CS, 0);
    asm volatile("nop \n nop \n nop");
}

void NRF_CE_HIGH()
{
    asm volatile("nop \n nop \n nop");
    gpio_put(NRFmodule->pin_NRF_CS, 1);
    asm volatile("nop \n nop \n nop");
}

uint8_t ReadReg(uint8_t reg)
{
    uint8_t val = 0;
    
    CSn_LOW();
    spi_write_blocking(NRFmodule->spi, &reg ,1);
    spi_read_blocking(NRFmodule->spi, RF24_NOP, &val, 1);
    CSn_HIGH();

    return val;
}

void WriteReg(uint8_t reg, uint8_t *data, uint8_t length)
{
    reg |= W_REGISTER;
    CSn_LOW();
    spi_write_blocking(NRFmodule->spi, &reg, 1);
    spi_write_blocking(NRFmodule->spi,data, length);
    CSn_HIGH();
}

void NRF_Config(char *name, uint8_t channel, uint8_t payloadsize)
{
    CSn_HIGH();
    NRF_CE_LOW();
    
    sleep_ms(11);

    uint8_t nrf_config = PWR_UP | EN_CRC;

    WriteReg(NRF_CONFIG, &nrf_config, 1); // Enable Power and CRC and waith for 1.5ms

    sleep_us(1500);

    uint8_t nrf_en_aa = ACK_DISABLE;
    WriteReg(EN_AA, &nrf_en_aa, 1); // Disable auto acknowledge
    
    WriteReg(RF_CH, &channel, 1); // set channel number/frequency

    WriteReg(RX_ADDR_P0, (uint8_t *)name, 5); // set RX pipe0 name
    WriteReg(TX_ADDR, (uint8_t *)name, 5); // set TX pipe0 name

    WriteReg(RX_PW_P0, &payloadsize, 1); //set pipe0 payload size
}

void NRF_TX_Mode()
{
    uint8_t config = ReadReg(NRF_CONFIG); //read Config register before switching bits
    config &= PRIM_RX_TX; // set bit 0 to 0
    WriteReg(NRF_CONFIG, &config, 1);
    NRF_CE_LOW();
    sleep_us(130);
}

void NRF_RX_Mode()
{
    uint8_t config = ReadReg(NRF_CONFIG); //read Config register before switching bits
    config |= PRIM_RX_RX; // set bit 0 to 1
    WriteReg(NRF_CONFIG, &config, 1);
    NRF_CE_HIGH();
    sleep_us(130);
}

void NRF_Transmit(char *data)
{   
    uint8_t w_tx_reg = W_TX_PAYLOAD;
    CSn_LOW();
    spi_write_blocking(NRFmodule->spi, &w_tx_reg, 1);
    spi_write_blocking(NRFmodule->spi, (uint8_t *)data, 32);
    CSn_HIGH();

    NRF_CE_HIGH();
    sleep_us(20);
    NRF_CE_LOW();   
}

void NRF_ReadData(char *data)
{
    uint8_t r_rx_reg = R_RX_PAYLOAD;
    CSn_LOW();
    spi_write_blocking(NRFmodule->spi, &r_rx_reg, 1);
    spi_read_blocking(NRFmodule->spi, RF24_NOP, (uint8_t*)data, 32);
    CSn_HIGH();
}

bool NRF_Data_Available()
{
    uint8_t Reg_FIFO = ReadReg(FIFO_STATUS);
    return !(Reg_FIFO & 0x01);
}