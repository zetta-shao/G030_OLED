
#include "swspi.h"
// #include "stm32f1xx_hal.h"
//#include "gpiodef.h"

//void swspi_initial(swspi_t *d);
void swspi_hal_delay_us(uint32_t us) { STM32_DELAY_US(us); }
void swspi_hal_delay_ms(uint32_t ms) { STM32_DELAY_MS(ms); }
#if 0
static int spi_transmit(hwspi_t *d) {
	return HAL_SPI_Transmit(d->hWND, d->bufout, d->datasize, HAL_MAX_DELAY);
}

static int spi_receive(hwspi_t *d) {
	return HAL_SPI_Receive(d->hWND, d->bufin, d->datasize, HAL_MAX_DELAY);
}

static int spi_transmit_receive(hwspi_t *d) {
	return HAL_SPI_TransmitReceive(d->hWND, d->bufout, d->bufin, d->datasize, HAL_MAX_DELAY);
}

static void GPIOset_stm32(swgpio_t *d, uint8_t val) { HAL_GPIO_WritePin(d->port, d->pin, (val)?GPIO_PIN_SET:GPIO_PIN_RESET); }
static void GPIOget_stm32(swgpio_t *d, uint8_t *val) { *val=HAL_GPIO_ReadPin(d->port, d->pin); }
static void GPIOmode_in_stm32(swgpio_t *d) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Pin = d->pin;
    HAL_GPIO_Init(d->port, &GPIO_InitStruct);
}

static void GPIOmode_out_stm32(swgpio_t *d) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Pin = d->pin;
    HAL_GPIO_Init(d->port, &GPIO_InitStruct);
}

static int swspi_port_io_ctl(uint8_t opt, void *param) {
    int ret = 0;
    //swspi_t *d=(swspi_t*)param;
    switch(opt)
    {
    //case IOCTL_SWSPI_SET_MOSI_HIGH: GPIOset_stm32(&d->MOSI, 1); break;
    //case IOCTL_SWSPI_SET_MOSI_LOW: GPIOset_stm32(&d->MOSI, 0); break;
    //case IOCTL_SWSPI_GET_MISO_LEVEL: GPIOget_stm32(&d->MISO, (uint8_t*)&ret); break;
    //case IOCTL_SWSPI_SET_CLK_HIGH: GPIOset_stm32(&d->CLK, 1); break;
    //case IOCTL_SWSPI_SET_CLK_LOW: GPIOset_stm32(&d->CLK, 0); break;
    case IOCTL_SWSPI_SET_GPIO_LOW: GPIOset_stm32((swgpio_t*)param, 0); break;
    case IOCTL_SWSPI_SET_GPIO_HIGH: GPIOset_stm32((swgpio_t*)param, 1); break;
    case IOCTL_SWSPI_GET_GPIO_LEVEL: GPIOget_stm32((swgpio_t*)param, (uint8_t*)&ret); break;
    case IOCTL_SWSPI_SET_GPIO_INPUT: GPIOmode_in_stm32((swgpio_t*)param); break;
    case IOCTL_SWSPI_SET_GPIO_OUTPUT: GPIOmode_out_stm32((swgpio_t*)param); break;
    case IOCTL_SWSPI_DELAY_US: swspi_delay_us(*(uint32_t*)param); break;
    case IOCTL_SWSPI_DELAY_MS: swspi_delay_ms(*(uint32_t*)param); break;
    case IOCTL_SWSPI_HWSPI_READ: spi_receive((hwspi_t*)param); break;
    case IOCTL_SWSPI_HWSPI_WRITE: spi_transmit((hwspi_t*)param); break;
    case IOCTL_SWSPI_HWSPI_RW: spi_transmit_receive((hwspi_t*)param); break;

    default:
        break;
    }
    return ret;
}
#endif
void swspi_hal_gpio_out(swgpio_t *d, uint8_t val) {
	//if(!d || !d->port || d->pin==65535) return;
	HAL_GPIO_WritePin(d->port, d->pin, val?1:0);
}
uint8_t swspi_hal_gpio_in(swgpio_t *d) {
	//if(!d || !d->port || d->pin==65535) return 0;
	return HAL_GPIO_ReadPin(d->port, d->pin)?1:0;
}
void swspi_hal_gpio_mode(swgpio_t *d, uint8_t val) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(val!=0) { // 1=output, 0=input
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Pin = d->pin;
    } else {
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Pin = d->pin;
    }
    HAL_GPIO_Init(d->port, &GPIO_InitStruct);
}

int swspi_hal_transmit(void *hWND, uint8_t *pWrite, uint16_t datasize) {
	return HAL_SPI_Transmit(hWND, pWrite, datasize, HAL_MAX_DELAY);
}

int swspi_hal_receive(void *hWND, uint8_t *pRead, uint16_t datasize) {
	return HAL_SPI_Receive(hWND, pRead, datasize, HAL_MAX_DELAY);
}

int swspi_hal_transmit_receive(void *hWND, uint8_t *pRead, uint8_t *pWrite, uint16_t datasize) {
	return HAL_SPI_TransmitReceive(hWND, pWrite, pRead, datasize, HAL_MAX_DELAY);
}

int swspi_hal_init(swspi_t *d, swgpio_t *clk, swgpio_t *mosi, swgpio_t *miso) {
    //d->hal_io_ctl = swspi_port_io_ctl;
    //if(d->bitmask == 0) { // h/w spi
    if(d->Delay_Time == 0) { // h/w spi
    	d->CLK.port = (void*)clk; d->CLK.pin = 65535;
    } else { // s/w spi
    	if(clk) {
    		d->CLK.port = clk->port; d->CLK.pin = clk->pin;
    		swspi_hal_gpio_mode(&d->CLK, 1);
		//swspi_hal_gpio_out(&d->CLK, d->cpol);
    	}
    	if(mosi) {
    		d->MOSI.port = mosi->port; d->MOSI.pin = mosi->pin;
    		swspi_hal_gpio_mode(&d->MOSI, 1);
		//swspi_hal_gpio_out(&d->MOSI, d->cpha);
    	}
    	if(miso) {
    		d->MISO.port = miso->port; d->MISO.pin = miso->pin;
    		swspi_hal_gpio_mode(&d->MISO, 0);
    	}
    }
    return 0;
}


int swspi_hal_setmode(swspi_t *d, uint8_t val) {
	((SPI_HandleTypeDef *)d->CLK.port)->Instance->CR1 &= ~ (SPI_CR1_CPHA_Msk|SPI_CR1_CPOL_Msk);
	if(val & 1) ((SPI_HandleTypeDef *)d->CLK.port)->Instance->CR1 |= val << SPI_CR1_CPOL_Pos;
	if(val & 2) ((SPI_HandleTypeDef *)d->CLK.port)->Instance->CR1 |= val << SPI_CR1_CPHA_Pos;
	return 0;
}

int swspi_hal_setbits(swspi_t *d, uint8_t val) { (void)d; (void)val; return 0; } //too complex.

void swspi_hal_spiclose(swspi_t *d) { (void)d; }
