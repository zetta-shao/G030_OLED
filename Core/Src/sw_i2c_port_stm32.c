
#include "sw_i2c.h"

// #include "stm32f1xx_hal.h"
#include "gpiodef.h"

//#define SW_I2C1_SCL_PORT    GPIOB
//#define SW_I2C1_SDA_PORT    GPIOB
//#define SW_I2C1_SCL_PIN     GPIO_PIN_6
//#define SW_I2C1_SDA_PIN     GPIO_PIN_7

void SW_I2C_initial(swi2c_t *d);
static int sw_i2c_port_initial(struct tag_swi2c *pIIC);

#if 0
static void enable_clock(void* GPIOport) {
	switch((uint32_t)GPIOport) {
	case (uint32_t)GPIOA: __HAL_RCC_GPIOA_CLK_ENABLE(); break;
	case (uint32_t)GPIOB: __HAL_RCC_GPIOB_CLK_ENABLE(); break;
	case (uint32_t)GPIOC: __HAL_RCC_GPIOC_CLK_ENABLE(); break;
	}
}
#endif

void swi2c_delay_us(uint32_t us) { STM32_DELAY_US(us); }

void swi2c_delay_ms(uint32_t ms) { STM32_DELAY_MS(ms); }

static int i2c_transmit_mem_t(hwi2c_t *d) {
	if(d->devaddrsize == 0)
		return (HAL_I2C_Master_Transmit((I2C_HandleTypeDef *)d->hWND, (d->i2cdev_addr<<1), d->data, d->datasize, 0x100) == HAL_OK) ? 0 : 1;
	else
		return (HAL_I2C_Mem_Write((I2C_HandleTypeDef *)d->hWND, (d->i2cdev_addr<<1), d->i2cmem_addr, d->devaddrsize, d->data, d->datasize, 0x100) == HAL_OK) ? 0 : 1;
}

static int i2c_receive_mem_t(hwi2c_t *d) {
	if(d->devaddrsize == 0)
		return (HAL_I2C_Master_Transmit((I2C_HandleTypeDef *)d->hWND, (d->i2cdev_addr<<1), d->data, d->datasize, 0x100) == HAL_OK) ? 0 : 1;
	else
		return (HAL_I2C_Mem_Read((I2C_HandleTypeDef *)d->hWND, (d->i2cdev_addr<<1), d->i2cmem_addr, d->devaddrsize, d->data, d->datasize, 0x100) == HAL_OK) ? 0 : 1;
}

static int sw_i2c_port_io_ctl(uint8_t opt, void *param);

void __HAL_init__(swi2c_t *d) {
		d->hal_init = sw_i2c_port_initial;
	    d->hal_io_ctl = sw_i2c_port_io_ctl;
	    if(d->SCL.port != NULL) SW_I2C_initial(d);
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

static int sw_i2c_port_initial(struct tag_swi2c *pIIC) {
	if(!pIIC) return 1;
	//enable_clock(pIIC->SDAport);
	//enable_clock(pIIC->SCLport);
	GPIOmode_out_stm32(&pIIC->SDA);
	GPIOmode_out_stm32(&pIIC->SCL);
    return 0;
}
static int sw_i2c_port_io_ctl(uint8_t opt, void *param) {
    int ret = 0;
    struct tag_swi2c *d=(struct tag_swi2c*)param;
    switch (opt)
    {
    case HAL_IO_OPT_SET_SDA_HIGH: GPIOset_stm32(&d->SDA, 1); break;
    case HAL_IO_OPT_SET_SDA_LOW: GPIOset_stm32(&d->SDA, 0); break;
    case HAL_IO_OPT_GET_SDA_LEVEL: GPIOget_stm32(&d->SDA, (uint8_t*)&ret); break;
    case HAL_IO_OPT_SET_SDA_INPUT: GPIOmode_in_stm32(&d->SDA); break;
    case HAL_IO_OPT_SET_SDA_OUTPUT: GPIOmode_out_stm32(&d->SDA); break;
    case HAL_IO_OPT_SET_SCL_HIGH: GPIOset_stm32(&d->SCL, 1); break;
    case HAL_IO_OPT_SET_SCL_LOW: GPIOset_stm32(&d->SCL, 0); break;
    case HAL_IO_OPT_GET_SCL_LEVEL: GPIOget_stm32(&d->SCL, (uint8_t*)&ret); break;
    case HAL_IO_OPT_SET_SCL_INPUT: GPIOmode_in_stm32(&d->SCL); break;
    case HAL_IO_OPT_SET_SCL_OUTPUT: GPIOmode_out_stm32(&d->SCL); break;
    case HAL_IO_OPT_SET_GPIO_LOW: GPIOset_stm32((stm32_gpio_t*)param, 0); break;
    case HAL_IO_OPT_SET_GPIO_HIGH: GPIOset_stm32((stm32_gpio_t*)param, 1); break;
    case HAL_IO_OPT_GET_GPIO_LEVEL: GPIOget_stm32((stm32_gpio_t*)param, (uint8_t*)&ret); break;
    case HAL_IO_OPT_SET_GPIO_INPUT: GPIOmode_in_stm32((stm32_gpio_t*)param); break;
    case HAL_IO_OPT_SET_GPIO_OUTPUT: GPIOmode_out_stm32((stm32_gpio_t*)param); break;
    case HAL_IO_OPT_DELAY_US: swi2c_delay_us(*(uint32_t*)param); break;
    case HAL_IO_OPT_DELAY_MS: swi2c_delay_ms(*(uint32_t*)param); break;
    case HAL_IO_OPT_HWI2C_READ: i2c_receive_mem_t((hwi2c_t*)param); break;
    case HAL_IO_OPT_HWI2C_WRITE: i2c_transmit_mem_t((hwi2c_t*)param); break;
    default:
        break;
    }
    return ret;
}
