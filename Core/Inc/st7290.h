#ifndef _ST7290_h_          
#define _ST7290_h_
#include <gpiodef.h>
#include <sw_i2c.h>

typedef struct tag_st7920 {
	struct tag_swi2c *pDev;
	stm32_gpio_t	psb;
	stm32_gpio_t	rs;
	stm32_gpio_t	clk;
	stm32_gpio_t	sid;
} st7920_t;

void st7920_init(st7920_t *d);
void st7920_serialwrite(st7920_t *d, uint8_t Dbyte);
void st7920_cmd(st7920_t *d, uint8_t Cbyte);
void st7920_data(st7920_t *d, uint8_t Dbyte );
void st7920_cursor(st7920_t *d, uint8_t x, uint8_t y);
void st7920_string(st7920_t *d,uint8_t x,uint8_t y,uint8_t *s);
void st7920_fill(st7920_t *d, uint8_t color);

/*
 * stm32_gpio_t gpios[4] { sid, sclk, rs, psb }
 */
void st7920_init_w_gpio(st7920_t *d, int gpios, stm32_gpio_t *gpioary);

#endif

