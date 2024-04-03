#ifndef _ST7290_h_          
#define _ST7290_h_
#include <gpiodef.h>
#include <swspi.h>

typedef struct tag_st7920 {
	struct tag_swspi *pDev;
	spi_gpio_t	psb;
	spi_gpio_t	rs;
	//spi_gpio_t	clk;
	//spi_gpio_t	sid;
} st7920_t;

void st7920_init(st7920_t *d, spi_gpio_t *rs, spi_gpio_t *psb);
void st7920_serialwrite(st7920_t *d, uint8_t Dbyte);
void st7920_cmd(st7920_t *d, uint8_t Cbyte);
void st7920_data(st7920_t *d, uint8_t Dbyte );
void st7920_cursor(st7920_t *d, uint8_t x, uint8_t y);
void st7920_string(st7920_t *d,uint8_t x,uint8_t y,uint8_t *s);
void st7920_fill(st7920_t *d, uint8_t color);

/*
 * spi_gpio_t gpios[4] { sid, sclk, rs, psb }
 */
void st7920_init_w_gpio(st7920_t *d, int gpios, spi_gpio_t *gpioary);

#endif

