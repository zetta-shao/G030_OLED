#include "gpiodef.h"
#include "st7920.h"

/****************************************
* clone from https://github.com/wumingyu12/STM32-ST7290-LCD12864-20150729/tree/master
****************************************/
void st7920_set_psb(st7920_t *d, uint8_t val); //always low
void st7920_set_rs(st7920_t *d, uint8_t val); //always high
void st7920_set_sclk(st7920_t *d, uint8_t val);
void st7920_set_sid(st7920_t *d, uint8_t val);
#define delay_us STM32_DELAY_US
#define delay_ms STM32_DELAY_MS

void st7920_init(st7920_t *d, spi_gpio_t *rs, spi_gpio_t *psb) {
	if(rs) d->rs = *rs;
	if(psb) d->psb = *psb;
	if(rs) st7920_set_rs(d, 1); //LCD_RS_1;  //CSһֱ���ߣ�ʹ��Һ������ֱ�ӽ�VCC��
	if(psb) st7920_set_psb(d, 0); //LCD_PSB_0; //һֱ���ͣ��ô��ڷ�ʽ���� ����ֱ�ӽӵأ�

	st7920_cmd(d, 0x30);
	delay_ms(10);
	st7920_cmd(d, 0x01);
	delay_ms(10);
	st7920_cmd(d, 0x02);
	delay_ms(10);
	st7920_cmd(d,0x06);
	delay_ms(10);
	st7920_cmd(d, 0x0c);
	delay_ms(10);
	st7920_cmd(d, 0x34); //ͼ�ο���������ʾ��
	delay_ms(10);
	st7920_fill(d, 0x00);    //�������RAM
	delay_ms(10);
	st7920_cmd(d, 0x36); //ͼ�ο���������ʾ��
	delay_ms(10);
}

void st7920_serialwrite(st7920_t *d, uint8_t Dbyte) {
#if 0
     uint8_t i;
     for(i=8;i>0;i--) {
    	 st7920_set_sid(d, (Dbyte&128)>>7);
    	 st7920_set_sclk(d, 1);	 //LCD_SCLK_1;  //�����ʾ�����⣬�����ڴ˺���ӳ�
		 delay_us(1); //72MHZ�ٶ�̫�죬M0��48MHZ�������
		 st7920_set_sclk(d, 0); //LCD_SCLK_0;
		 Dbyte<<=1;
     }
#endif
     swspi_write(d->pDev, &Dbyte, 1);
}
void st7920_cmd(st7920_t *d, uint8_t Cbyte) {
	st7920_set_rs(d, 1); //LCD_RS_1;
	st7920_serialwrite(d, 0xf8);              //11111,RW(0),RS(0),0
	st7920_serialwrite(d, 0xf0&Cbyte);
	st7920_serialwrite(d, 0xf0&Cbyte<<4);
	st7920_set_rs(d, 0); //LCD_RS_0;
}

void st7920_data(st7920_t *d, uint8_t Dbyte ) {
	st7920_set_rs(d, 1); //LCD_RS_1;
	st7920_serialwrite(d, 0xfa);              //11111,RW(0),RS(1),0
	st7920_serialwrite(d, 0xf0&Dbyte);
	st7920_serialwrite(d, 0xf0&Dbyte<<4);
	st7920_set_rs(d, 0); //LCD_RS_0;
}

void st7920_cursor(st7920_t *d, uint8_t x, uint8_t y) {
    uint8_t k = 0;
	switch(x) {
	 case 1: k = 0x80 + y; break;
     case 2: k = 0x90 + y; break;
	 case 3: k = 0x88 + y; break;
     case 4: k = 0x98 + y; break;
	 default: k = 0x80 + y;
	}
	st7920_cmd(d, k);
}

void st7920_string(st7920_t *d,uint8_t x,uint8_t y,uint8_t *s) {
	st7920_cmd(d, 0x30); //�����׼ģʽ
	st7920_cursor(d, x,y);
	while(*s) { st7920_data(d, *s); s++; }
	st7920_cmd(d, 0x36); //����ͼ��ģʽ
}

void st7920_fill(st7920_t *d, uint8_t color) { //�������RAM
	uint8_t x,y,i;
	st7920_cmd(d, 0x34);
	for(i=0;i<9;i=i+8) {
		for(y=0;y<0x20;y++) {
			for(x=0;x<8;x++) {
				st7920_cmd(d, y+0x80);
				st7920_cmd(d, x+0x80+i);
				st7920_data(d, color);
				st7920_data(d, color);
			}
		}
	}
	st7920_cmd(d, 0x36);
}

#ifdef USE_HAL_DRIVER //for STM32
void st7920_set_psb(st7920_t *d, uint8_t val) {
	if(! d->psb.port) return;
	//if(val != 0)	d->pDev->hal_io_ctl(IOCTL_SWSPI_SET_GPIO_LOW, &d->psb);
	//else			d->pDev->hal_io_ctl(IOCTL_SWSPI_SET_GPIO_HIGH, &d->psb);
	swspi_setgpo(&d->psb, val);
} //always low
void st7920_set_rs(st7920_t *d, uint8_t val) {
	if(! d->rs.port) return;
	//if(val != 0)	d->pDev->hal_io_ctl(IOCTL_SWSPI_SET_GPIO_LOW, &d->rs);
	//else			d->pDev->hal_io_ctl(IOCTL_SWSPI_SET_GPIO_HIGH, &d->rs);
	swspi_setgpo(&d->rs, val);
} //always high
void st7920_set_sclk(st7920_t *d, uint8_t val) {
	//if(val != 0)	d->pDev->hal_io_ctl(IOCTL_SWSPI_SET_GPIO_LOW, &d->clk);
	//else			d->pDev->hal_io_ctl(IOCTL_SWSPI_SET_GPIO_HIGH, &d->clk);
}
void st7920_set_sid(st7920_t *d, uint8_t val) {
	//if(val != 0)	d->pDev->hal_io_ctl(IOCTL_SWSPI_SET_GPIO_LOW, &d->sid);
	//else			d->pDev->hal_io_ctl(IOCTL_SWSPI_SET_GPIO_HIGH, &d->sid);
}
#else
__weak void st7920_set_psb(st7920_t *d, uint8_t val) { } //always low
__weak void st7920_set_rs(st7920_t *d, uint8_t val) { } //always high
__weak void st7920_set_sclk(st7920_t *d, uint8_t val) { }
__weak void st7920_set_sid(st7920_t *d, uint8_t val) { }
#endif
