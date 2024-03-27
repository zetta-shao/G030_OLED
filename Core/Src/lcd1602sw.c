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
	frame_data[1] = data_h | 8;
	frame_data[2] = data_l | 13;
	frame_data[3] = data_l | 8;
	return SW_I2C_Write_0addr(p->d, LCD_ADDRESS, frame_data, 4);
}

void lcd_clear(lcd1602_t *p) {
	if(p->d == NULL) return;
	lcd_send_cmd(p, 0x01);
	HAL_Delay(1);
}

uint8_t lcd_Init(lcd1602_t *p, struct sw_i2c_s *d) {
	uint8_t	res = 0;
	if(!p || !d) return 1;
	p->d = d;
	HAL_Delay(50);
	res |= lcd_send_cmd(p, 0x33);
	HAL_Delay(5);
	res |= lcd_send_cmd(p, 0x33);
	HAL_Delay(1);
	res |= lcd_send_cmd(p, 0x33);
	HAL_Delay(10);
	res |= lcd_send_cmd(p, 0x32);
	HAL_Delay(10);
	if(res != 0) { p->d = NULL; return 255; }

	res |= lcd_send_cmd(p, 0x28);		//function set
	HAL_Delay(1);
	//lcd_send_cmd(d, 0x08);		//Display on/off
	res |= lcd_send_cmd(p, 0x0c);		//Display on/off
	HAL_Delay(1);
	res |= lcd_send_cmd(p, 0x01);		//clear display
	HAL_Delay(1);
	res |= lcd_send_cmd(p, 0x06);		//Enter mode
	HAL_Delay(1);
	res |= lcd_send_cmd(p, 0x0C);		//Display on/off
	HAL_Delay(1);
	return res;
}

void lcd_send_string (lcd1602_t *p, char *str) {
	if(p->d == NULL) return;
	while(*str) { lcd_send_data(p, *str++); }
	HAL_Delay(1);
}

void lcd_put_cur(lcd1602_t *p, uint8_t row,uint8_t col) {
	if(p->d == NULL) return;
	lcd_send_cmd(p, 0x80 | (col + (0x40 * row)));
}

