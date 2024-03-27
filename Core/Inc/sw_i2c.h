#ifndef _SW_I2C_H_
#define _SW_I2C_H_

#include <stdint.h>

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
}hal_io_opt_e;

typedef struct sw_i2c_g {
	int (*hal_transmit)(void*, uint16_t, uint16_t, uint8_t, uint8_t*, uint16_t);
	int (*hal_receive)(void*, uint16_t, uint16_t, uint8_t, uint8_t*, uint16_t);
	void*		SDAport;
	void*		SCLport;
	uint16_t	SDApin;
	uint16_t	SCLpin;
} sw_i2c_g_t;

typedef struct sw_i2c_s {
    int (*hal_init)(struct sw_i2c_g*);
    int (*hal_io_ctl)(hal_io_opt_e opt, void *arg);
    void (*hal_delay_us)(uint32_t us);
    void (*hal_delay_ms)(uint32_t ms);
    struct sw_i2c_g		gpio;
} sw_i2c_t;


/* functions */
void SW_I2C_initial(sw_i2c_t *d);
void SW_I2C_SWinit(sw_i2c_t *d, void* SCLport, uint16_t SCLpin, void* SDAport, uint16_t SDApin);
void SW_I2C_HWinit(sw_i2c_t *d, void *hWND);
uint8_t SW_I2C_Read_0addr(sw_i2c_t *d, uint8_t IICID, uint8_t *pdata, uint8_t rcnt);
uint8_t SW_I2C_Read_8addr(sw_i2c_t *d, uint8_t IICID, uint8_t regaddr, uint8_t *pdata, uint8_t rcnt);
uint8_t SW_I2C_Read_16addr(sw_i2c_t *d, uint8_t IICID, uint16_t regaddr, uint8_t *pdata, uint8_t rcnt);
uint8_t SW_I2C_Write_0addr(sw_i2c_t *d, uint8_t IICID, uint8_t *pdata, uint8_t rcnt);
uint8_t SW_I2C_Write_8addr(sw_i2c_t *d, uint8_t IICID, uint8_t regaddr, uint8_t *pdata, uint8_t rcnt);
uint8_t SW_I2C_Write_16addr(sw_i2c_t *d, uint8_t IICID, uint16_t regaddr, uint8_t *pdata, uint8_t rcnt);
uint8_t SW_I2C_Check_SlaveAddr(sw_i2c_t *d, uint8_t IICID);
void SW_I2C_delay_us(sw_i2c_t *d, uint32_t time);
void SW_I2C_delay_ms(sw_i2c_t *d, uint32_t time);

#endif  /* __I2C_SW_H */
