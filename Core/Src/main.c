#include <stdio.h>
#include "main.h"
#include "gpiodef.h"
#include "swi2c.h"
#include "swspi.h"
#include "ssd1306.h"
#include "lcd1602sw.h"
#include "INA3221.h"
#include "IP2365.h"
#include "sw35xx_s.h"
#include "ath20.h"

#define LCD_SSD1306

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;
SPI_HandleTypeDef hspi1;
TIM_HandleTypeDef htim14;
TIM_HandleTypeDef htim3;
ADC_HandleTypeDef hadc1;
int32_t devices = 0;
int32_t vref;
int8_t env_key1 = 0;
int8_t env_key2 = 0;
int8_t env_key3 = 0;
int8_t _val_ = 0;
swi2c_t si2c1={0}, si2c2={0}, si2c3={0}, si2c4={0};
swspi_t sspi1={0};
//swspi_t sspi3={0};
lcddev_t *gp_lcd;
#ifdef LCD1602
lcd1602_t LCD1601={0}, LCD1602={0}, LCD1603={0}, LCD1604={0};
#endif
#ifdef LCD_SSD1306
ssd1306_t gt_lcd={0};
#endif
#ifdef LCD_ST7920
st7920_t gt_lcd={0};
#endif
//SSD1306_t SD13062 = { 0 };
ina3221_t ina3221 = { 0 };
IP2365_t IP23651={0}, IP23652={0};
sw35xx_t SW35181={0}, SW35182={0}, SW35183={0}, SW35184={0};
//ath20t ath20 = {0};
#define deffont Font_6x8

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) { }
#endif /* USE_FULL_ASSERT */
void update_ina3221(ina3221_t *ina, lcddev_t *lcd) {
	  char str[64];
	  int32_t wV,wI;

	if(!ina || !lcd) return;
	if(ina->pDev == NULL) {
		fontdraw_setpos(lcd, 1, 16);
		fontdraw_string(lcd, "no INA3221 delect");
		return;
	}

	  //wV = ina3221_getVol_mV(ina, 1); //mV
	  wV = ina3221_getAvgVol(ina, 1);
	  //wI = ina3221_getCur_mA(ina, 1);
	  wI = ina3221_getAvgCur(ina, 1);
	  sprintf(str, "1:%4ldmV %4ldmA    ", wV, wI);
	  fontdraw_setpos(lcd, 0, 16);
	  fontdraw_string(lcd, str);

	  wV = ina3221_getAvgVol(ina, 2);
	  wI = ina3221_getAvgCur(ina, 2);
	  sprintf(str, "2:%4ldmV %4ldmA    ", wV, wI);
	  fontdraw_setpos(lcd, 0, 24);
	  fontdraw_string(lcd, str);

	  wV = ina3221_getAvgVol(ina, 3);
	  wI = ina3221_getAvgCur(ina, 3);
	  sprintf(str, "3:%4ldmV %4ldmA    ", wV, wI);
	  fontdraw_setpos(lcd, 0, 32);
	  fontdraw_string(lcd, str);

	  lcd->update(lcd);
}
void update_sw3518(sw35xx_t *d, lcddev_t *lcd) {
	char str[64];
	if(!d || !lcd) return;
	str[63] = sw35xx_version(d);

	  if(d->pDev == NULL) {
			fontdraw_setpos(lcd, 1, 16);
			fontdraw_string(lcd, "no SW3518 delect");
		  return;
	  }

	sprintf(str, "SW3518 v:%x", str[63]);
	fontdraw_setpos(lcd, 0, 16);
	fontdraw_string(lcd, str);

	SW35xx_readStatus(d, 0);
	sprintf(str, "PD:%x ST:%x", d->PDVersion, d->fastChargeType);
	fontdraw_setpos(lcd, 64, 16);
	fontdraw_string(lcd, str);

	sprintf(str, "i:%4dmV o:%4dmV", d->vin_mV, d->vout_mV);
	fontdraw_setpos(lcd, 0, 24);
	fontdraw_string(lcd, str);
	sprintf(str, "A:%4dmA C:%4dmA", d->iout_usba_mA, d->iout_usbc_mA);
	fontdraw_setpos(lcd, 0, 32);
	fontdraw_string(lcd, str);

	lcd->update(lcd);
}
#ifdef ath20
void update_ath20(ath20t *d, SSD1306_t *led) {
	char str[64];
	int32_t vH, vT;
	uint8_t *res = (uint8_t*)(str+63);

	  if(d->pDev == NULL) {
		  ssd1306_SetCursor(led, 1, 16);
		  ssd1306_WriteString(led, "no ath20 delect", Font_6x8, 1);
		  return;
	  }


	*res = ath20_start(d);
	vH = d->humidity / 1049;
	vT = (d->temprature / 524) - 500;
	//if((*res) > 0) sprintf(str, "ath20 err:%d", *res);
	//else
	sprintf(str, "h:%2ld.%1ld t:%2ld.%1ld", vH/10, vH%10, vT/10, vT%10);
	ssd1306_SetCursor(led, 0, 16);
	ssd1306_WriteString(led, str, Font_6x8, 1);
	sprintf(str, "h:%5ld t:%5ld", d->humidity, d->temprature);
	ssd1306_SetCursor(led, 0, 24);
	ssd1306_WriteString(led, str, Font_6x8, 1);

	ssd1306_UpdateScreen(led);
}
#endif
#define ADV_VREF_PREAMP (4096 * 1200)

void init_adc(ADC_HandleTypeDef *d) {
	uint32_t idx, res = 0;
	ADC_ChannelConfTypeDef sConfig = {0};

	sConfig.Channel = ADC_CHANNEL_VREFINT;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) { Error_Handler(); }

	HAL_ADC_Start(d);
	for(idx=0; idx<4; idx++) {
		HAL_ADC_PollForConversion(d, 1); //wsit for 1mS
		res += HAL_ADC_GetValue(d);
	}
	HAL_ADC_Stop(d);
	res >>= 2;

	vref = ADV_VREF_PREAMP / res;

	sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) { Error_Handler(); }
}

void update_adc(ADC_HandleTypeDef *d, lcddev_t *lcd) {
	char str[64];
	uint32_t res, idx;
	//ADC_ChannelConfTypeDef sConfig = {0};
	init_adc(d);
	//sConfig.Channel = ADC_CHANNEL_VREFINT;
	//sConfig.Rank = ADC_REGULAR_RANK_1;
	//sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
	//HAL_ADC_ConfigChannel(&hadc1, &sConfig);

	//HAL_ADC_Start(d);
	//for(res=0,idx=0; idx<4; idx++) {
	//	HAL_ADC_PollForConversion(d, 1); //wsit for 1mS
	//	res += HAL_ADC_GetValue(d);
	//}
	//HAL_ADC_Stop(d);
	//res >>= 2;

	//sprintf(str, "chip_vref %ld %d    ", vref, _val_);
	//fontdraw_setpos(lcd, 0, 8);
	//fontdraw_string(lcd, str);
	//lcd->update(lcd);

	//sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
	//HAL_ADC_ConfigChannel(&hadc1, &sConfig);

	HAL_ADC_Start(d);
	HAL_ADC_PollForConversion(d, 1); //wsit for 1mS
	res = HAL_ADC_GetValue(d);
	HAL_ADC_Stop(d);
	idx = __LL_ADC_CALC_TEMPERATURE(vref, res, LL_ADC_RESOLUTION_12B);

	sprintf(str, "%ld", idx);
	fontdraw_setpos(lcd, 16, lcd->frameHeight - 8);
	fontdraw_string(lcd, str);
	lcd->update(lcd);
}

void update_devices(SSD1306_t *led) {
#if 0
	char str[64];
	sprintf(str, "lcd 1:%d 2:%d 3:%d 4:%d", (LCD1601.d)?1:0, (LCD1602.d)?1:0, (LCD1603.d)?1:0, (LCD1604.d)?1:0);
	ssd1306_SetCursor(led, 0, 16);
	ssd1306_WriteString(led, str, deffont, 1);
	ssd1306_UpdateScreen(led);
	sprintf(str, "ip2 1:%d 2:%d i:%d    ", (IP23651.pD)?1:0, (IP23652.pD)?1:0, (ina3221.pDev)?1:0);
	ssd1306_SetCursor(led, 0, 24);
	ssd1306_WriteString(led, str, deffont, 1);
	ssd1306_UpdateScreen(led);
	sprintf(str, "sw3 1:%d 2:%d 3:%d 4:%d", (SW35181.pDev)?1:0, (SW35182.pDev)?1:0, (SW35183.pDev)?1:0, (SW35184.pDev)?1:0);
	ssd1306_SetCursor(led, 0, 32);
	ssd1306_WriteString(led, str, deffont, 1);
	ssd1306_UpdateScreen(led);
#endif
}

int main(void) {
  HAL_Init();
  GPIOinit();
  swi2c_HWinit(&si2c1, &hi2c1);
  swi2c_HWinit(&si2c2, &hi2c2);
  {
	  swgpio_t SCL = { SI2C1P, SI2C1L };
	  swgpio_t SDA = { SI2C1P, SI2C1A };
	  swi2c_SWinit(&si2c3, &SCL, &SDA);
  }
  {
	  swgpio_t SCL = { SI2C2P, SI2C2L };
	  swgpio_t SDA = { SI2C2P, SI2C2A };
	  swi2c_SWinit(&si2c4, &SCL, &SDA);
  }
  swspi_HWinit(&sspi1, &hspi1);

  HAL_ADCEx_Calibration_Start(&hadc1);
  HAL_Delay(50);
  #ifdef LCD1602
  lcd_init(&LCD1601, &si2c1, 0);
  lcd_init(&LCD1602, &si2c2, 0);
  lcd_init(&LCD1603, &si2c3, 0);
  lcd_init(&LCD1604, &si2c4, 0);
  #endif
  #ifdef LCD_SSD1306
  {
	  swgpio_t CD = { LCD_CD_P, LCD_CD };
	  //stm32_gpio_t RS = { LCD_RSP, LCD_RS };
	  SSD1306_gpioinit4W2(&gt_lcd, NULL, &CD);
	  SSD1306_init(&gt_lcd, &sspi1, &deffont);
	  gp_lcd = &gt_lcd.d;
  }
  #endif
  //ina3221_begin(&ina3221, &si2c4);
  //IP2365_init(&IP23651, &si2c3);
  //IP2365_init(&IP23652, &si2c4);
  //sw35xx_init(&SW35181, &si2c1);
  //sw35xx_init(&SW35182, &si2c2);
  //sw35xx_init(&SW35182, &si2c3);
  //sw35xx_init(&SW35182, &si2c4);
  //ath20_init(&ath20, &si2c3);
#if LCD1602
  {
	  char str[64];

	  lcd_put_cur(&LCD1601, 0, 0);
	  lcd_send_string(&LCD1601, "hwi2c1 test");
	  sprintf(str, "log %d%d%d%d", LCD1601.log[0], LCD1602.log[0], LCD1603.log[0], LCD1604.log[0]);
	  lcd_put_cur(&LCD1601, 1, 0);
	  lcd_send_string(&LCD1601, str);

	  lcd_put_cur(&LCD1602, 0, 0);
	  lcd_send_string(&LCD1602, "hwi2c2 test");
	  sprintf(str, "log %d%d%d%d", LCD1601.log[1], LCD1602.log[1], LCD1603.log[1], LCD1604.log[1]);
	  lcd_put_cur(&LCD1602, 1, 0);
	  lcd_send_string(&LCD1602, str);

	  lcd_put_cur(&LCD1603, 0, 0);
	  lcd_send_string(&LCD1603, "swi2c1 test");
	  sprintf(str, "log %d%d%d%d", LCD1601.log[0], LCD1602.log[0], LCD1603.log[0], LCD1604.log[0]);
	  lcd_put_cur(&LCD1603, 1, 0);
	  lcd_send_string(&LCD1603, str);

	  lcd_put_cur(&LCD1604, 0, 0);
	  lcd_send_string(&LCD1604, "swi2c2 test");
	  sprintf(str, "log %d%d%d%d", LCD1601.log[1], LCD1602.log[1], LCD1603.log[1], LCD1604.log[1]);
	  lcd_put_cur(&LCD1604, 1, 0);
	  lcd_send_string(&LCD1604, str);
  }
#endif
#if 0
  ssd1306_SetCursor(&SD13062, 1, 0);
  ssd1306_WriteString(&SD13062, "font6x8", deffont, 1);
  ssd1306_SetCursor(&SD13062, 65, 0);
  ssd1306_WriteString(&SD13062, "font6x8", deffont, 0);
  ssd1306_UpdateScreen(&SD13062);
#endif
#if 0
  {
	  char str[64]; //, vtg[8], cit[8];
	  //float fvalv, fvali;
	  uint16_t wV = ina3221_getDieID(&ina3221);
	  uint16_t wI = ina3221_getManufID(&ina3221);
	  sprintf(str, "INA:%04x:%04x", wV, wI);
	  lcd_put_cur(&si2c3, 0, 0);
	  lcd_send_string(&si2c3, str);
  }
#endif
  init_adc(&hadc1);
  //update_devices(&SD13061);
  if(gp_lcd) gp_lcd->update(gp_lcd);
  _val_ = 0;
  while (1) {
	  //update_ina3221(&ina3221, &SD13061);
	  //update_sw3518(&SW35184, &SD13061);
	  if(env_key1 > 0) { _val_++; env_key1--; }
	  if(env_key2 > 0) { _val_--; env_key2--; }
	  if(env_key3 > 0) { _val_=0; env_key3--; }
	  //update_ath20(&ath20, &SD13061);
	  update_adc(&hadc1, gp_lcd);
	  #ifdef LCD1602
	  lcd_set_backlight_on(&LCD1601, 1); lcd_set_backlight_on(&LCD1602, 1);
	  lcd_set_backlight_on(&LCD1603, 1); lcd_set_backlight_on(&LCD1604, 1);
	  #endif
	  //HAL_GPIO_WritePin(EVB_LED_P, EVB_LED, GPIO_PIN_SET);
	  HAL_Delay(500);
	  //update_ina3221(&ina3221, &SD13061);
	  //update_sw3518(&SW35184, &SD13061);
	  if(env_key1 > 0) { _val_++; env_key1--; }
	  if(env_key2 > 0) { _val_--; env_key2--; }
	  if(env_key3 > 0) { _val_=0; env_key3--; }
	  //update_ath20(&ath20, &SD13061);
	  update_adc(&hadc1, gp_lcd);
	  #ifdef LCD1602
	  lcd_set_backlight_on(&LCD1601, 0); lcd_set_backlight_on(&LCD1602, 0);
	  lcd_set_backlight_on(&LCD1603, 0); lcd_set_backlight_on(&LCD1604, 0);
	  #endif
	  //HAL_GPIO_WritePin(EVB_LED_P, EVB_LED, GPIO_PIN_RESET);
	  HAL_Delay(500);
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
        if(htim == &htim14) {
                HAL_GPIO_TogglePin(EVB_LED_P, EVB_LED);
                //HAL_GPIO_TogglePin(LCD_RSP, LCD_RS);
        }
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {
	switch(GPIO_Pin) {
	case GPIO_PIN_13: env_key1=1; break;
	case GPIO_PIN_14: env_key2=1; break;
	case GPIO_PIN_15: env_key3=1; break;
	}
}
