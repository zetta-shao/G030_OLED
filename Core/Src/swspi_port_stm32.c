
#include "swspi.h"

// #include "stm32f1xx_hal.h"
#include "gpiodef.h"

//#define SW_I2C1_SCL_PORT    GPIOB
//#define SW_I2C1_SDA_PORT    GPIOB
//#define SW_I2C1_SCL_PIN     GPIO_PIN_6
//#define SW_I2C1_SDA_PIN     GPIO_PIN_7

void swspi_initial(swspi_t *d);
static int swspi_port_initial(struct tag_swspi *pSPI);

void swspi_delay_us(uint32_t us) { STM32_DELAY_US(us); }
void swspi_delay_ms(uint32_t ms) { STM32_DELAY_MS(ms); }

static int spi_transmit(hwspi_t *d) {
	return HAL_SPI_Transmit(d->hWND, d->bufout, d->datasize, HAL_MAX_DELAY);
}

static int spi_receive(hwspi_t *d) {
	return HAL_SPI_Receive(d->hWND, d->bufin, d->datasize, HAL_MAX_DELAY);
}

static int spi_transmit_receive(hwspi_t *d) {
	return HAL_SPI_TransmitReceive(d->hWND, d->bufout, d->bufin, d->datasize, HAL_MAX_DELAY);
}

static int swspi_port_io_ctl(uint8_t opt, void *param);

void __HAL_init_spi__(swspi_t *d) {
    d->hal_init = swspi_port_initial;
    d->hal_io_ctl = swspi_port_io_ctl;
    if(d->CLK.port != NULL) swspi_initial(d);
}

static void GPIOset_stm32(stm32_gpio_t *d, uint8_t val) { HAL_GPIO_WritePin(d->port, d->pin, (val==0)?GPIO_PIN_RESET:GPIO_PIN_SET); }
static void GPIOget_stm32(stm32_gpio_t *d, uint8_t *val) { *val=HAL_GPIO_ReadPin(d->port, d->pin); }
static void GPIOmode_in_stm32(stm32_gpio_t *d) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Pin = d->pin;
    HAL_GPIO_Init(d->port, &GPIO_InitStruct);
}

static void GPIOmode_out_stm32(stm32_gpio_t *d) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Pin = d->pin;
    HAL_GPIO_Init(d->port, &GPIO_InitStruct);
}

static int swspi_port_initial(struct tag_swspi *pSPI) {
	if(!pSPI) return 1;
	//enable_clock(pIIC->SDAport);
	//enable_clock(pIIC->SCLport);
	if(pSPI->CLK.port) GPIOmode_out_stm32(&pSPI->CLK);
	if(pSPI->MOSI.port) GPIOmode_out_stm32(&pSPI->MOSI);
	if(pSPI->MISO.port) GPIOmode_in_stm32(&pSPI->MISO);
	if(pSPI->SS.port) GPIOmode_out_stm32(&pSPI->SS);
    return 0;
}
static int swspi_port_io_ctl(uint8_t opt, void *param) {
    int ret = 0;
    swspi_t *d=(swspi_t*)param;
    switch(opt)
    {
    case HAL_IO_OPT_SET_MOSI_HIGH: GPIOset_stm32(&d->MOSI, 1); break;
    case HAL_IO_OPT_SET_MOSI_LOW: GPIOset_stm32(&d->MOSI, 0); break;
    case HAL_IO_OPT_GET_MISO_LEVEL: GPIOget_stm32(&d->MISO, (uint8_t*)&ret); break;
    case HAL_IO_OPT_SET_CLK_HIGH: GPIOset_stm32(&d->CLK, 1); break;
    case HAL_IO_OPT_SET_CLK_LOW: GPIOset_stm32(&d->CLK, 0); break;
    case HAL_IO_OPT_SET_SS_HIGH: GPIOset_stm32(&d->SS, 1); break;
    case HAL_IO_OPT_SET_SS_LOW: GPIOset_stm32(&d->SS, 0); break;
    case HAL_IO_OPT_SET_GPIO_LOW: GPIOset_stm32((stm32_gpio_t*)param, 0); break;
    case HAL_IO_OPT_SET_GPIO_HIGH: GPIOset_stm32((stm32_gpio_t*)param, 1); break;
    case HAL_IO_OPT_GET_GPIO_LEVEL: GPIOget_stm32((stm32_gpio_t*)param, (uint8_t*)&ret); break;
    case HAL_IO_OPT_SET_GPIO_INPUT: GPIOmode_in_stm32((stm32_gpio_t*)param); break;
    case HAL_IO_OPT_SET_GPIO_OUTPUT: GPIOmode_out_stm32((stm32_gpio_t*)param); break;
    case HAL_IO_OPT_DELAY_US: swspi_delay_us(*(uint32_t*)param); break;
    case HAL_IO_OPT_DELAY_MS: swspi_delay_ms(*(uint32_t*)param); break;
    case HAL_IO_OPT_HWSPI_READ: spi_receive((hwspi_t*)param); break;
    case HAL_IO_OPT_HWSPI_WRITE: spi_transmit((hwspi_t*)param); break;
    case HAL_IO_OPT_HWSPI_RW: spi_transmit_receive((hwspi_t*)param); break;

    default:
        break;
    }
    return ret;
}
