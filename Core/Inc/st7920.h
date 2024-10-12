#ifndef _ST7290_h_          
#define _ST7290_h_
#include <stdint.h>
#include "gpiodef.h"
#include "swspi.h"
#include "lcd_fonts.h"

#define LCD_CLS         0x01
#define LCD_HOME        0x02
#define LCD_ADDRINC     0x06
#define LCD_DISPLAYON   0x0C
#define LCD_DISPLAYOFF  0x08
#define LCD_CURSORON    0x0E
#define LCD_CURSORBLINK 0x0F
#define LCD_BASIC       0x30
#define LCD_EXTEND      0x34
#define LCD_GFXMODE     0x36
#define LCD_TXTMODE     0x34
#define LCD_STANDBY     0x01
#define LCD_SCROLL      0x03
#define LCD_SCROLLADDR  0x40
#define LCD_ADDR        0x80
#define LCD_LINE0       0x80
#define LCD_LINE1       0x90
#define LCD_LINE2       0x88
#define LCD_LINE3       0x98

#define ST7920F_TXT        0x0
#define ST7920F_GFX        0x1

// Include only needed fonts
//#define LCDFONT_INCLUDE_FONT_5x7
#define LCDFONT_INCLUDE_FONT_5x8
//#define LCDFONT_INCLUDE_FONT16_6x8
//#define LCDFONT_INCLUDE_FONT_7x10
//#define LCDFONT_INCLUDE_FONT_11x18
//#define LCDFONT_INCLUDE_FONT_16x26
//#define LCDFONT_INCLUDE_FONT_16x24

#define ST7920_WIDTH           128
#define ST7920_HEIGHT          64
#define ST7920_GFXMODE            0x100

typedef struct st7920_t st7920_t;

struct st7920_t {
    lcddev_t    d;
    swspi_t     *pDev;
    swgpio_t    *rs; //en
    uint8_t        buf[1024]; //for 128x64 pixel
};

/*typedef struct {
    struct {
    uint32_t    CurrentX : 10;
    uint32_t    CurrentY : 10;
    uint32_t    mode : 1;
    uint32_t    flag : 11;
    } __attribute__((packed));
    swspi_t     *pDev;
    //swgpio_t    *psb;
    swgpio_t    *rs; //en
    //swgpio_t    *clk; //clk
    //swgpio_t    *sid; //mosi
    uint8_t        buf[1024]; //for 128x64 pixel
} st7920_t;*/

void st7920_init(st7920_t *d, swspi_t *spi, swgpio_t *rs, void *pvFontDef);
void st7920_cmd(st7920_t *d, uint8_t Cbyte);
void st7920_data(st7920_t *d, uint8_t Dbyte );
void st7920_cursor(st7920_t *d, uint8_t x, uint8_t y);
void st7920_string(st7920_t *d,uint8_t x,uint8_t y,char *s);
void st7920_fill(st7920_t *d, uint8_t color);
void st7920_gfxmode(st7920_t *d, uint8_t mode); //0=txt, 1=gfx
void st7920_update(lcddev_t *d);
char st7920_WriteChar2(st7920_t *d, uint8_t ch, FontDef *font, uint8_t color);
void st7920_DrawPixel(st7920_t *d, uint8_t x, uint8_t y, uint8_t color);
void st7920_strin2(st7920_t *d, char *s, FontDef *font, uint8_t color);
/*
 * swgpio_t gpios[4] { sid, sclk, rs, psb }
 */
void st7920_init_w_gpio(st7920_t *d, int gpios, swgpio_t *gpioary);

#endif


