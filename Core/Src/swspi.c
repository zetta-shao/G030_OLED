/**
 * @file    softspi.c
 * @author  Myth
 * @version 0.2
 * @date    2021.10.12
 * @brief   STM32 SoftSPI Library
 * clone from https://github.com/Mythologyli/SoftSPI_HAL_Lib/tree/master
 */
#include "swspi.h"

void __HAL_init_spi__(swspi_t *d);

void swspi_SWinit(swspi_t *d, stm32_gpio_t *clk, stm32_gpio_t *mosi, stm32_gpio_t *miso, stm32_gpio_t *ss) {
	if(clk) { d->CLK.port = clk->port; d->CLK.pin = clk->pin; }
	if(mosi) { d->MOSI.port = mosi->port; d->MOSI.pin = mosi->pin; }
	if(miso) { d->MISO.port = miso->port; d->MISO.pin = miso->pin; }
	if(ss) { d->SS.port = ss->port; d->SS.pin = ss->pin; }
	d->Delay_Time = SWSPI_DELAY;
	d->bitmask = 0x80;
	__HAL_init_spi__(d);
}

void swspi_HWinit(swspi_t *d, void *hWND) {
	d->CLK.port = hWND; d->CLK.pin = 65535;
	d->Delay_Time = 0;
	d->bitmask = 0;
	__HAL_init_spi__(d);
}

void swspi_setbits(swspi_t *d, uint8_t val) { d->bitmask = 1 << val; }

void swspi_setCLK(swspi_t *d) { d->hal_io_ctl(HAL_IO_OPT_SET_CLK_HIGH, d); }
void swspi_clrCLK(swspi_t *d) { d->hal_io_ctl(HAL_IO_OPT_SET_CLK_LOW, d); }
void swspi_setMOSI(swspi_t *d) { d->hal_io_ctl(HAL_IO_OPT_SET_MOSI_HIGH, d); }
void swspi_clrMOSI(swspi_t *d) { d->hal_io_ctl(HAL_IO_OPT_SET_MOSI_LOW, d); }
void swspi_setSS(swspi_t *d) { d->hal_io_ctl(HAL_IO_OPT_SET_SS_HIGH, d); }
void swspi_clrSS(swspi_t *d) { d->hal_io_ctl(HAL_IO_OPT_SET_SS_LOW, d); }
void delay_def(swspi_t *d) { d->hal_io_ctl(HAL_IO_OPT_DELAY_US, &d->Delay_Time); }
uint8_t swspi_getMISO(swspi_t *d) { return d->hal_io_ctl(HAL_IO_OPT_GET_MISO_LEVEL, d); }

uint16_t swspi_rw2(swspi_t *d, uint16_t word) {
    uint16_t data = 0;
    uint16_t i = d->bitmask;
    swspi_clrSS(d);
    for (; i>0; i>>=1) {
    	swspi_clrCLK(d);
    	delay_def(d);

    	if(word & i) swspi_setMOSI(d);
    	else swspi_clrMOSI(d);
        delay_def(d);

        data <<= 1;
        swspi_setCLK(d);
        delay_def(d);

        data |= swspi_getMISO(d) ? 1 : 0;
        delay_def(d);
    }
    swspi_setSS(d);
    return data;
}

static void __swspi_write(swspi_t *d, uint16_t val) {
	uint16_t i;
	if(!d) return;
	i = d->bitmask;
	swspi_clrSS(d);
	for(; i>>0; i>>=1) {
		swspi_clrCLK(d);
		delay_def(d);

		if(val & i) swspi_setMOSI(d);
		else swspi_clrMOSI(d);
		delay_def(d);

		swspi_setCLK(d);
		delay_def(d);
	}
	swspi_setSS(d);
}

static uint16_t __swspi_read(swspi_t *d) {
	uint16_t i, res;
	if(!d) return 0;
	i = d->bitmask;
	swspi_clrSS(d);
	for(res = 0; i>>0; i>>=1) {
		swspi_clrCLK(d);
		delay_def(d);

        res <<= 1;
        swspi_setCLK(d);
        delay_def(d);

        res |= swspi_getMISO(d) ? 1 : 0;
        delay_def(d);
	}
	swspi_setSS(d);
	return res;
}

void swspi_readwrite(swspi_t *d, uint8_t *pWrite, uint8_t *pRead, uint32_t len) {
    uint8_t j;
    if(d->bitmask == 0) {
		if(d->bitmask > 128) {
			for (j=0; j<len; j++) { pRead[j] = swspi_rw2(d, pWrite[j]); }
		} else {
			uint16_t *t=(uint16_t*)pRead, *s=(uint16_t*)pWrite;
			for (j=0; j<len; j+=2, s++, t++) { *t = swspi_rw2(d, *s); }
		}
    } else {
    	hwspi_t h = { d->CLK.port, pRead, pWrite, len };
    	d->hal_io_ctl(HAL_IO_OPT_HWSPI_RW, &h);
    }
}

void swspi_read(swspi_t *d, uint8_t *pRead, uint32_t len) {
    uint8_t j;
    if(d->bitmask == 0) {
		if(d->bitmask > 128) {
			for (j=0; j<len; j++) { pRead[j] = __swspi_read(d); }
		} else {
			uint16_t *t=(uint16_t*)pRead;
			for (j=0; j<len; j+=2, t++) { *t = __swspi_read(d); }
		}
    } else {
    	hwspi_t h = { d->CLK.port, pRead, NULL, len };
    	d->hal_io_ctl(HAL_IO_OPT_HWSPI_READ, &h);
    }
}

void swspi_write(swspi_t *d, uint8_t *pWrite, uint32_t len) {
    uint8_t j;
    if(d->bitmask == 0) {
		if(d->bitmask > 128) {
			for (j=0; j<len; j++) { __swspi_write(d, pWrite[j]); }
		} else {
			uint16_t *s=(uint16_t*)pWrite;
			for (j=0; j<len; j+=2, s++) { __swspi_write(d, *s); }
		}
    } else {
    	hwspi_t h = { d->CLK.port, NULL, pWrite, len };
    	d->hal_io_ctl(HAL_IO_OPT_HWSPI_WRITE, &h);
    }
}
