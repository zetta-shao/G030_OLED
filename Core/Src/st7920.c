#include "gpiodef.h"
#include "st7920.h"
#include "lcd_fonts.h"
#include <stdio.h>
#include <string.h>

/****************************************
* clone from https://github.com/wumingyu12/STM32-ST7290-LCD12864-20150729/tree/master
****************************************/
#define delay_us swspi_hal_delay_us
#define delay_ms swspi_hal_delay_ms

//void st7920_set_psb(st7920_t *d, uint8_t val) { (void)d; (void)val; } //always low
void st7920_set_rs(st7920_t *d, uint8_t val) { (void)d; (void)val; } //enable, always high
//void st7920_set_sclk(st7920_t *d, uint8_t val) { (void)d; (void)val; }
//void st7920_set_sid(st7920_t *d, uint8_t val) { (void)d; (void)val; }

void st7920_init(st7920_t *d, swspi_t *spi, swgpio_t *rs, void *pvFontDef) {
    if(spi) d->pDev = spi; else d->pDev = NULL;
    if(rs) d->rs = rs; else d->rs = NULL;
    //if(psb) d->psb = psb; else d->psb = NULL;
    if(rs) { swspi_setgpmode(rs, 1); st7920_set_rs(d, 0); } //LCD_RS_1;  //CSһֱ���ߣ�ʹ��Һ������ֱ�ӽ�VCC��
    //if(psb) { swspi_setgpmode(psb, 1); st7920_set_psb(d, 0); } //LCD_PSB_0; //һֱ���ͣ��ô��ڷ�ʽ���� ����ֱ�ӽӵأ�
    d->d.curX=0; d->d.curY=0;
    d->d.flags = (FONTDRAW_HEIGHTMUL | FONTDRAW_WIDTHDIV );
    d->d.frameWidth = 128;
    d->d.frameHeight = 64;
    d->d.oneLineOffsetSize = 16;
    d->d.heightScale = 4;
    d->d.widthScale = 3;
    d->d.posmask = 7;
    d->d.invposmask = 7; //position from 7-0 to 0-7
    d->d.pFrameBuf = d->buf;
    if(pvFontDef) d->d.pFont = (struct FontDef*)pvFontDef;
    d->d.color = 1;
	d->d.update = st7920_update;
	d->d.parent = d;
    delay_ms(40);
    st7920_cmd(d, LCD_BASIC); //0x30
    delay_ms(10);
    st7920_cmd(d, LCD_CLS); //0x01
    delay_ms(10);
    st7920_cmd(d, LCD_HOME); //0x02
    delay_ms(10);
    st7920_cmd(d, LCD_ADDRINC); //0x06
    delay_ms(10);
    st7920_cmd(d, LCD_DISPLAYON); //0x0c
    delay_ms(10);
    st7920_cmd(d, LCD_EXTEND); //0x34 ͼ�ο���������ʾ��
    delay_ms(10);
    st7920_fill(d, 0x00);    //�������RAM
    delay_ms(10);
    st7920_cmd(d, LCD_GFXMODE); //0x36 ͼ�ο���������ʾ��
    delay_ms(10);
    memset(d->buf, 0, 1024);
}

void st7920_cmd(st7920_t *d, uint8_t Cbyte) {
    uint8_t cmd[4] = { 0xf8, 0xf0, 0xf0, 0};
    cmd[1] &= Cbyte;
    cmd[2] &= Cbyte << 4;
    //st7920_set_rs(d, 1); //LCD_RS_1;
    swspi_write(d->pDev, cmd, 3);
    //st7920_set_rs(d, 0); //LCD_RS_0;
}

void st7920_data(st7920_t *d, uint8_t Dbyte ) {
    uint8_t cmd[4] = { 0xfa, 0xf0, 0xf0, 0};
    cmd[1] &= Dbyte;
    cmd[2] &= Dbyte << 4;
    //st7920_set_rs(d, 1); //LCD_RS_1;
    swspi_write(d->pDev, cmd, 3);
    //st7920_set_rs(d, 0); //LCD_RS_0;
}

void st7920_pdata(st7920_t *d, void *pvBUF, int32_t bytesize) {
    uint8_t cmd = 0xfa;
    //st7920_set_rs(d, 1); //LCD_RS_1;
    swspi_write(d->pDev, &cmd, 1);
    swspi_write(d->pDev, pvBUF, bytesize);
    //st7920_set_rs(d, 0); //LCD_RS_0;
}


void st7920_cursor(st7920_t *d, uint8_t x, uint8_t y) {
    uint8_t k = 0x80; //0x80, 0x90, 0x88, 0x98
    k |= (y & 1) << 4;
    k |= (y & 2) << 2;
    k += x;
    //d->CurrentX = x; d->CurrentY = y;
    d->d.curX=x; d->d.curY=y;
    st7920_cmd(d, k);
}

void st7920_string(st7920_t *d,uint8_t x,uint8_t y,char *s) {
    st7920_cmd(d, 0x30); //�����׼ģʽ
    st7920_cursor(d,x,y);
    while(*s) { st7920_data(d, (uint8_t)*s); s++; }
    st7920_cmd(d, 0x36); //����ͼ��ģʽ
}

void st7920_fill(st7920_t *d, uint8_t color) { //�������RAM
    uint8_t y, z=ST7920_WIDTH/4;
    st7920_cmd(d, 0x34);
    memset(d->buf, color, z);    
    for(y=128; y<160; y++) {
        st7920_cmd(d, y); //row from, 0-63
        st7920_cmd(d, 128); //col from, 0-7, w=128
        st7920_pdata(d, d->buf, z);
    }
    st7920_cmd(d, 0x36);
}

void st7920_gfxmode(st7920_t *d, uint8_t mode) {
    if(mode) { 
        st7920_cmd(d, LCD_EXTEND); 
        st7920_cmd(d, LCD_GFXMODE);
        d->d.flags |= ST7920_GFXMODE;
    } else { 
        st7920_cmd(d, LCD_EXTEND); 
        st7920_cmd(d, LCD_TXTMODE); 
        d->d.flags &= ~ST7920_GFXMODE;
    }
}

/*    ST7920 had 256(w)x64(h)bit, total 2KiB GDRAM, in 128x64pixel product, 
    GDRAM operation mode is 256x32pixel. Imagination 128x64 is 256x32 in real,
    also row address only accept 0-31, but col address accept 0-15 
    (16bit data per one address, 16x16=256bit), and auto accumulation 
    able from 0 to 15 in the st7920.
    col address bit 4 determined display at y position 0-31 or 32-63.
    for example, row=1,col=1 (cmd 0x81,0x81), display position is 2,2,
    row=1,col=9 (cmd 0x81,0x89),display position is 2,34    
*/ 
void st7920_update(fontdraw_t *d) {
    int x, y;
	st7920_t *e = (st7920_t*)d->parent;
    uint32_t *_ps = (uint32_t*)d->pFrameBuf;
    uint32_t _t[8]; uint8_t *_pt=(uint8_t*)_t;    
    //st7920_cmd(e, 0x34);
    for(y=0x80; y<0xa0; y++, _ps+=4) {
        st7920_cmd(e, y); //row from, 0-63
        st7920_cmd(e, 0x80); //col from, 0-7, w=128
        for(x=0; x<4; x++) { //for one row, total 32byte
            _t[2] = (_ps[x] & 0xf0f0f0f0);
            _t[3] = (_ps[x] & 0x0f0f0f0f) << 4;
            _pt[0] = _pt[ 8]; _pt[1] = _pt[12];
            _pt[2] = _pt[ 9]; _pt[3] = _pt[13];
            _pt[4] = _pt[10]; _pt[5] = _pt[14];
            _pt[6] = _pt[11]; _pt[7] = _pt[15];
            st7920_pdata(e, _t, 8);
        }

        for(x=d->frameWidth; x<d->frameWidth+4; x++) { //for one row, total 8byte
            _t[2] = (_ps[x] & 0xf0f0f0f0);
            _t[3] = (_ps[x] & 0x0f0f0f0f) << 4;
            _pt[0] = _pt[ 8]; _pt[1] = _pt[12];
            _pt[2] = _pt[ 9]; _pt[3] = _pt[13];
            _pt[4] = _pt[10]; _pt[5] = _pt[14];
            _pt[6] = _pt[11]; _pt[7] = _pt[15];
            st7920_pdata(e, _t, 8);
        }            
    }
    //st7920_cmd(e, 0x36);
}

void st7920_DrawPixel(st7920_t *d, uint8_t x, uint8_t y, uint8_t color) {
    if(x >= ST7920_WIDTH || y >= ST7920_HEIGHT) { return; }
    // Draw in the right color //optimize for 128pixel width.
    if(color) {
        d->buf[(x >> 3) + (y << 4)] |= 1 << ((x&7)^7); 
    } else { 
        d->buf[(x >> 3) + (y << 4)] &= ~(1 << ((x&7)^7));
    }    
}
// 1-31 => 95-125
// 32-126 => 0-94
// 127-255 => 126-254
char st7920_WriteChar2(st7920_t *d, uint8_t ch, FontDef *font, uint8_t color) {
    uint32_t i, j;
    uint32_t posx = d->d.curX, posy = d->d.curY;
    if(ch == 0) return 0;
    //if (ST7920_WIDTH < (d->CurrentX + font->FontWidth) ||
    //    ST7920_HEIGHT < (d->CurrentY + font->FontHeight)) { return 0; }
    if (ST7920_WIDTH < (d->d.curX + font->FontWidth) ||
        ST7920_HEIGHT < (d->d.curY + font->FontHeight)) { return 0; }
    //printf("--2--\n");
    if(font->flags & FONT_FLAG_WPTR) {
        uint16_t *px, m;
        //if (ch < 32 || ch > 126) return 0;
        if(d->d.flags & FONTDRAW_VERTICALDRAW) {
            px = (uint16_t*)font->data + ((ch-32)*font->FontWidth);
            for(i = 0; i < font->FontWidth; i++) {
                for(m=0x8000,j=0; j<font->FontHeight; j++, m>>=1) {
                    st7920_DrawPixel(d, posx + j, (posy + i), (px[i] & m) ? color : !color);
                }
            }
        } else {
            px = (uint16_t*)font->data + ((ch-32)*font->FontHeight);
            for(i = 0; i < font->FontHeight; i++) {
                for(m=0x8000,j=0; j<font->FontWidth; j++, m>>=1) {
                    st7920_DrawPixel(d, posx + j, (posy + i), (px[i] & m) ? color : !color);
                }
            }
        }
    } else {
        uint8_t *px, m;
        //if (ch < 32 || ch > 126) return 0;
        if(d->d.flags & FONTDRAW_VERTICALDRAW) {
            px = ((uint8_t*)font->data);
            if(font->flags & FONT_FLAG_BTAB) px += (ch - 1) * font->FontWidth;
            else px += (ch - 32) * font->FontWidth;
            for(i = 0; i < font->FontWidth; i++) {
                for(m=0x80,j=0; j<font->FontHeight; j++, m>>=1) {
                    st7920_DrawPixel(d, posx + i, (posy + j), (px[i] & m) ? color : !color);
                }
            }
        } else {
            px = ((uint8_t*)font->data);
            if(font->flags & FONT_FLAG_BTAB) px += (ch - 1) * font->FontHeight;
            else px += (ch - 32) * font->FontHeight;
            for(i = 0; i < font->FontHeight; i++) {
                for(m=0x80,j=0; j<font->FontWidth; j++, m>>=1) {
                    st7920_DrawPixel(d, posx + j, (posy + i), (px[i] & m) ? color : !color);
                }
            }
        }
    }

    d->d.curX += font->FontWidth;
    return ch;
}

void st7920_strin2(st7920_t *d, char *s, FontDef *font, uint8_t color) {
    while(*s) { st7920_WriteChar2(d, *s, font, color); s++; }
}
