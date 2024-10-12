#ifndef _swi2c_H_
#define _swi2c_H_

#include <stdint.h>
#include <gpiodef.h>

//typedef stm32_gpio_t swgpio_t;
#define SWI2C_DELAY_TIME    10 // 10us 100kHz

#define I2C_READ            0x01
#define READ_CMD            1
#define WRITE_CMD           0

typedef enum
{
    IOCTL_SWI2C_SET_SDA_LOW = 0,
    IOCTL_SWI2C_SET_SDA_HIGH,
    IOCTL_SWI2C_SET_SCL_LOW,
    IOCTL_SWI2C_SET_SCL_HIGH,
    IOCTL_SWI2C_SET_SDA_INPUT,
    IOCTL_SWI2C_SET_SDA_OUTPUT,
    IOCTL_SWI2C_SET_SCL_INPUT,
    IOCTL_SWI2C_SET_SCL_OUTPUT,
    IOCTL_SWI2C_GET_SDA_LEVEL,
    IOCTL_SWI2C_GET_SCL_LEVEL,
    IOCTL_SWI2C_SET_GPIO_LOW,
    IOCTL_SWI2C_SET_GPIO_HIGH,
	IOCTL_SWI2C_GET_GPIO_LEVEL,
    IOCTL_SWI2C_SET_GPIO_INPUT,
    IOCTL_SWI2C_SET_GPIO_OUTPUT,
	IOCTL_SWI2C_DELAY_US,
	IOCTL_SWI2C_DELAY_MS,
	IOCTL_SWI2C_HWI2C_READ,
	IOCTL_SWI2C_HWI2C_WRITE,
}hal_io_opt_e;

typedef struct hwi2c_stm32 {
	void		*hWND;
	uint8_t		i2cdev_addr;
	uint16_t	i2cmem_addr;
	uint8_t		devaddrsize;
	uint8_t		*data;
	uint16_t	datasize;
	uint8_t		unused1;
} hwi2c_t;

typedef struct tag_swi2c {
    int (*hal_init)(struct tag_swi2c*);
    int (*hal_io_ctl)(hal_io_opt_e opt, void *arg);
	swgpio_t	SDA;
	swgpio_t	SCL;
} swi2c_t;


/* functions */
//void swi2c_initial(swi2c_t *d);
void swi2c_SWinit(swi2c_t *d, swgpio_t *CLK, swgpio_t *DATA);
void swi2c_HWinit(swi2c_t *d, void *hWND);

uint8_t swi2c_Read_0addr(swi2c_t *d, uint8_t IICID, uint8_t *pdata, uint8_t rcnt);
uint8_t swi2c_Read_8addr(swi2c_t *d, uint8_t IICID, uint8_t regaddr, uint8_t *pdata, uint8_t rcnt);
uint8_t swi2c_Read_16addr(swi2c_t *d, uint8_t IICID, uint16_t regaddr, uint8_t *pdata, uint8_t rcnt);
uint8_t swi2c_Write_0addr(swi2c_t *d, uint8_t IICID, uint8_t *pdata, uint8_t rcnt);
uint8_t swi2c_Write_8addr(swi2c_t *d, uint8_t IICID, uint8_t regaddr, uint8_t *pdata, uint8_t rcnt);
uint8_t swi2c_Write_16addr(swi2c_t *d, uint8_t IICID, uint16_t regaddr, uint8_t *pdata, uint8_t rcnt);
uint8_t swi2c_Check_SlaveAddr(swi2c_t *d, uint8_t IICID);
void swi2c_delay_us(uint32_t time);
void swi2c_delay_ms(uint32_t time);
void swi2c_dummy_clock(swi2c_t *d);
void swi2c_setgpo(swi2c_t *d, swgpio_t *gpiogrp, uint8_t val);
uint8_t swi2c_getgpi(swi2c_t *d, swgpio_t *gpiogrp);
#endif  /* __I2C_SW_H */
