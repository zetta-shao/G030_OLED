#ifndef _SW_I2C_H_
#define _SW_I2C_H_

#include <stdint.h>
#include <gpiodef.h>

#define SW_I2C_WAIT_TIME    10 // 10us 100kHz

#define I2C_READ            0x01
#define READ_CMD            1
#define WRITE_CMD           0

typedef enum
{
    HAL_IO_OPT_SET_SDA_LOW = 0,
    HAL_IO_OPT_SET_SDA_HIGH,
    HAL_IO_OPT_SET_SCL_LOW,
    HAL_IO_OPT_SET_SCL_HIGH,
    HAL_IO_OPT_SET_SDA_INPUT,
    HAL_IO_OPT_SET_SDA_OUTPUT,
    HAL_IO_OPT_SET_SCL_INPUT,
    HAL_IO_OPT_SET_SCL_OUTPUT,
    HAL_IO_OPT_GET_SDA_LEVEL,
    HAL_IO_OPT_GET_SCL_LEVEL,
    HAL_IO_OPT_SET_GPIO_LOW,
    HAL_IO_OPT_SET_GPIO_HIGH,
	HAL_IO_OPT_GET_GPIO_LEVEL,
    HAL_IO_OPT_SET_GPIO_INPUT,
    HAL_IO_OPT_SET_GPIO_OUTPUT,
	HAL_IO_OPT_DELAY_US,
	HAL_IO_OPT_DELAY_MS,
	HAL_IO_OPT_HWI2C_READ,
	HAL_IO_OPT_HWI2C_WRITE,
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
	stm32_gpio_t	SDA;
	stm32_gpio_t	SCL;
} swi2c_t;


/* functions */
void SW_I2C_initial(swi2c_t *d);
void SW_I2C_SWinit(swi2c_t *d, void* SCLport, uint16_t SCLpin, void* SDAport, uint16_t SDApin);
void SW_I2C_HWinit(swi2c_t *d, void *hWND);

uint8_t SW_I2C_Read_0addr(swi2c_t *d, uint8_t IICID, uint8_t *pdata, uint8_t rcnt);
uint8_t SW_I2C_Read_8addr(swi2c_t *d, uint8_t IICID, uint8_t regaddr, uint8_t *pdata, uint8_t rcnt);
uint8_t SW_I2C_Read_16addr(swi2c_t *d, uint8_t IICID, uint16_t regaddr, uint8_t *pdata, uint8_t rcnt);
uint8_t SW_I2C_Write_0addr(swi2c_t *d, uint8_t IICID, uint8_t *pdata, uint8_t rcnt);
uint8_t SW_I2C_Write_8addr(swi2c_t *d, uint8_t IICID, uint8_t regaddr, uint8_t *pdata, uint8_t rcnt);
uint8_t SW_I2C_Write_16addr(swi2c_t *d, uint8_t IICID, uint16_t regaddr, uint8_t *pdata, uint8_t rcnt);
uint8_t SW_I2C_Check_SlaveAddr(swi2c_t *d, uint8_t IICID);
void swi2c_delay_us(uint32_t time);
void swi2c_delay_ms(uint32_t time);
void swi2c_dummy_clock(swi2c_t *d);
#endif  /* __I2C_SW_H */
