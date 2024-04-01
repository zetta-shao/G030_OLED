/**
 * @file    softspi.h
 * @author  Myth
 * @version 0.2
 * @date    2021.10.12
 * @brief   STM32 SoftSPI Library
 */

#ifndef __SOFTSPI_H
#define __SOFTSPI_H

#include <stdint.h>
#include <gpiodef.h>

#define SWSPI_DELAY 10 //10uS

typedef enum {
    HAL_IO_OPT_SET_MOSI_LOW = 0,
    HAL_IO_OPT_SET_MOSI_HIGH,
    HAL_IO_OPT_SET_CLK_LOW,
    HAL_IO_OPT_SET_CLK_HIGH,
    HAL_IO_OPT_GET_MISO_LEVEL,
    HAL_IO_OPT_SET_SS_LOW,
    HAL_IO_OPT_SET_SS_HIGH,
    HAL_IO_OPT_SET_GPIO_LOW,
    HAL_IO_OPT_SET_GPIO_HIGH,
    HAL_IO_OPT_GET_GPIO_LEVEL,
    HAL_IO_OPT_SET_GPIO_INPUT,
    HAL_IO_OPT_SET_GPIO_OUTPUT,
    HAL_IO_OPT_DELAY_US,
    HAL_IO_OPT_DELAY_MS,
    HAL_IO_OPT_HWSPI_READ,
	HAL_IO_OPT_HWSPI_WRITE,
	HAL_IO_OPT_HWSPI_RW,
} hal_spiio_opt_e;

typedef struct hwspi_stm32 {
	void		*hWND;
	uint8_t		*bufin;
	uint8_t		*bufout;
	uint16_t	datasize;
} hwspi_t;

typedef struct tag_swspi {
    int (*hal_init)(struct tag_swspi*);
    int (*hal_io_ctl)(hal_spiio_opt_e opt, void *arg);
	uint32_t	        Delay_Time;
    stm32_gpio_t        CLK;
    stm32_gpio_t        MOSI;
    stm32_gpio_t        MISO;
    stm32_gpio_t        SS;
    uint16_t			bitmask;
} swspi_t;


/* functions */
void swspi_SWinit(swspi_t *d, stm32_gpio_t *clk, stm32_gpio_t *mosi, stm32_gpio_t *miso, stm32_gpio_t *ss);
void swspi_setbits(swspi_t *d, uint8_t val);
void swspi_HWinit(swspi_t *d, void *hWND);
void swspi_read(swspi_t *d, uint8_t *pRead, uint32_t len);
void swspi_write(swspi_t *d, uint8_t *pWrite, uint32_t len);
void swspi_readwrite(swspi_t *d, uint8_t *pWrite, uint8_t *pRead, uint32_t len);
//HAL_StatusTypeDef SoftSPI_Init(SoftSPI_TypeDef *SoftSPIx);

#endif
