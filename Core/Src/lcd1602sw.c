#include "stdio.h"
#include "sw_i2c.h"
#include "gpiodef.h"
#include "lcd1602sw.h"

//bit	7	6	5	4	3	2	1	0
//def  d7  d6  d5  d4  un  en  rw  rs
uint8_t lcd_send_cmd(lcd1602_t *p, char cmd) {
	char data_h,data_l;
	uint8_t frame_data[4];
	if(p->d == NULL) return 255;
	data_h = (cmd&0xf0);
	data_l = ((cmd <<4)&0xf0);
	frame_data[0] = data_h | 12;
	frame_data[1] = data_h | 8;
	frame_data[2] = data_l | 12;
	frame_data[3] = data_l | 8;
	return SW_I2C_Write_0addr(p->d, LCD_ADDRESS, frame_data, 4);
}

uint8_t lcd_send_data(lcd1602_t *p, char data) {
	char data_h,data_l;
	uint8_t frame_data[4];
	if(p->d == NULL) return 255;
	data_h = (data&0xf0);
	data_l = ((data <<4)&0xf0);
	frame_data[0] = data_h | 13;
	frame_data[1] = data_h | 9;
	frame_data[2] = data_l | 13;
	frame_data[3] = data_l | 9;
	return SW_I2C_Write_0addr(p->d, LCD_ADDRESS, frame_data, 4);
}

uint8_t lcd_read_data(lcd1602_t *p) {
	uint8_t buf[4];
	SW_I2C_Read_0addr(p->d, LCD_ADDRESS, buf, 1);
	return buf[0];
}

void lcd_wait_ready(lcd1602_t *p) {
	uint8_t timeout=200, buf = 0x80;
	for(; timeout>=1; timeout--) {
		SW_I2C_Read_0addr(p->d, LCD_ADDRESS, &buf, 1);
		if((buf & 128) != 0) break;
		swi2c_delay_us(100);
	}
}


void lcd_clear(lcd1602_t *p) {
	if(p->d == NULL) return;
	lcd_send_cmd(p, 0x01);
	HAL_Delay(1);
}

uint8_t lcd_Init(lcd1602_t *p, struct tag_swi2c *d) {
	uint8_t	res = 0;
	if(!p || !d) return 1;
	p->d = d;

	//if(SW_I2C_Read_0addr(p->d, LCD_ADDRESS, &res, 1) != 0) { p->d = NULL; return 255; }

	//swi2c_delay_ms(50); //HAL_Delay(100);
	res = lcd_send_cmd(p, 0x33);
	HAL_Delay(5);
	//if(res != 0) { p->d = NULL; return 255; }
	//swi2c_delay_ms(5);
	//lcd_wait_ready(p);
	lcd_send_cmd(p, 0x33);
	HAL_Delay(1);
	////lcd_wait_ready(p);
	//swi2c_delay_ms(1);
	lcd_send_cmd(p, 0x33);
	HAL_Delay(10);
	//swi2c_delay_ms(10);
	////lcd_wait_ready(p);
	lcd_send_cmd(p, 0x32);
	HAL_Delay(10);
	//swi2c_delay_ms(10);
	//lcd_wait_ready(p);

	lcd_send_cmd(p, 0x28);		//function set
	HAL_Delay(1);
	//swi2c_delay_ms(1);
	//lcd_wait_ready(p);
	//lcd_send_cmd(d, 0x08);		//Display on/off
	lcd_send_cmd(p, 0x0c);		//Display on/off
	HAL_Delay(1);
	//swi2c_delay_ms(1);
	//lcd_wait_ready(p);
	lcd_send_cmd(p, 0x01);		//clear display
	HAL_Delay(5);
	//lcd_wait_ready(p);
	//swi2c_delay_ms(5);
	lcd_send_cmd(p, 0x06);		//Enter mode
	HAL_Delay(1);
	////lcd_wait_ready(p);
	//swi2c_delay_ms(1);
	lcd_send_cmd(p, 0x0C);		//Display on/off
	HAL_Delay(1);
	////lcd_wait_ready(p);
	//swi2c_delay_ms(1);
	return res;
}

void lcd_send_string (lcd1602_t *p, char *str) {
	if(p->d == NULL) return;
	while(*str) { lcd_send_data(p, *str++); }
	//HAL_Delay(1);
	lcd_wait_ready(p);
}

void lcd_put_cur(lcd1602_t *p, uint8_t row,uint8_t col) {
	if(p->d == NULL) return;
	lcd_send_cmd(p, 0x80 | (col + (0x40 * row)));
	lcd_wait_ready(p);
}

