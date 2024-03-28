#ifndef __LCD1602SW__
#define __LCD1602SW__

#define LCD_ADDRESS 0x27
#define LCD_ADDRESS_8 (LCD_ADDRESS << 1)

typedef union { //0x01
	struct {
	uint8_t data_l : 4;
	uint8_t data_h : 4;
	} __attribute__((packed));
	uint8_t i2cpkg;
} i2cpkg;

typedef struct tag_lcd1602 {
	struct tag_swi2c *d;
} lcd1602_t;

void lcd_send_string (lcd1602_t *p, char *str);
//uint8_t lcd_Init(struct tag_swi2c *d);
uint8_t lcd_Init(lcd1602_t *p, struct tag_swi2c *d);
void lcd_clear(lcd1602_t *p);
uint8_t lcd_send_data(lcd1602_t *p, char data);
uint8_t lcd_send_cmd(lcd1602_t *p, char cmd);
void lcd_put_cur(lcd1602_t *p, uint8_t row,uint8_t col);
#endif
