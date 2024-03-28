#include <stdio.h>
#include "main.h"
#include "gpiodef.h"
#include "sw_i2c.h"
#include "ssd1306.h"
#include "lcd1602sw.h"
#include "INA3221.h"
#include "IP2365.h"
#include "sw35xx_s.h"
#include "ath20.h"

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;
SPI_HandleTypeDef hspi1;
TIM_HandleTypeDef htim14;
TIM_HandleTypeDef htim3;
ADC_HandleTypeDef hadc1;
int32_t devices = 0;
int32_t vref;
swi2c_t si2c1={0}, si2c2={0}, si2c3={0}, si2c4={0};
lcd1602_t LCD1601 = { 0 };
lcd1602_t LCD1602 = { 0 };
lcd1602_t LCD1603 = { 0 };
lcd1602_t LCD1604 = { 0 };
SSD1306_t SH11061 = { 0 };
INA3221_t ina3221 = { 0 };
IP2365_t IP23651 = { 0 }; IP2365_t IP23652 = { 0 };
sw35xx_t SW35181 = { 0 }; sw35xx_t SW35182 = { 0 };
sw35xx_t SW35183 = { 0 }; sw35xx_t SW35184 = { 0 };
ath20t ath20 = { 0 };

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) { }
#endif /* USE_FULL_ASSERT */
void update_ina3221(INA3221_t *ina, SSD1306_t *led) {
	  char str[64];
	  int32_t wV,wI;

	  if(ina->pDev == NULL) {
		  ssd1306_SetCursor(led, 1, 16);
		  ssd1306_WriteString(led, "no INA3221 delect", Font_6x8, 1);
		  return;
	  }

	  //wV = ina3221_getVol_mV(ina, 1); //mV
	  wV = ina3221_getAvgVol(ina, 1);
	  //wI = ina3221_getCur_mA(ina, 1);
	  wI = ina3221_getAvgCur(ina, 1);
	  sprintf(str, "1:%4ldmV %4ldmA    ", wV, wI);
	  ssd1306_SetCursor(led, 1, 16);
	  ssd1306_WriteString(led, str, Font_6x8, 1);

	  wV = ina3221_getAvgVol(ina, 2);
	  wI = ina3221_getAvgCur(ina, 2);
	  sprintf(str, "2:%4ldmV %4ldmA    ", wV, wI);
	  ssd1306_SetCursor(led, 1, 24);
	  ssd1306_WriteString(led, str, Font_6x8, 1);

	  wV = ina3221_getAvgVol(ina, 3);
	  wI = ina3221_getAvgCur(ina, 3);
	  sprintf(str, "3:%4ldmV %4ldmA    ", wV, wI);
	  ssd1306_SetCursor(led, 1, 32);
	  ssd1306_WriteString(led, str, Font_6x8, 1);

	  ssd1306_UpdateScreen(led);
}
void update_sw3518(sw35xx_t *d, SSD1306_t *led) {
	char str[64];
	str[63] = sw35xx_version(d);

	  if(d->pDev == NULL) {
		  ssd1306_SetCursor(led, 1, 16);
		  ssd1306_WriteString(led, "no SW3518 delect", Font_6x8, 1);
		  return;
	  }

	sprintf(str, "SW3518 v:%x", str[63]);
	ssd1306_SetCursor(led, 0, 16);
	ssd1306_WriteString(led, str, Font_6x8, 1);

	SW35xx_readStatus(d, 0);
	sprintf(str, "PD:%x ST:%x", d->PDVersion, d->fastChargeType);
	ssd1306_SetCursor(led, 64, 16);
	ssd1306_WriteString(led, str, Font_6x8, 1);

	sprintf(str, "i:%4dmV o:%4dmV", d->vin_mV, d->vout_mV);
	ssd1306_SetCursor(led, 0, 24);
	ssd1306_WriteString(led, str, Font_6x8, 1);
	sprintf(str, "A:%4dmA C:%4dmA", d->iout_usba_mA, d->iout_usbc_mA);
	ssd1306_SetCursor(led, 0, 32);
	ssd1306_WriteString(led, str, Font_6x8, 1);
	ssd1306_UpdateScreen(led);
}

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

#define ADV_VREF_PREAMP (4096 * 1210)

void init_adc(ADC_HandleTypeDef *d, SSD1306_t *led) {
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

void update_adc(ADC_HandleTypeDef *d, SSD1306_t *led) {
	char str[64];
	uint32_t res, idx;
	//ADC_ChannelConfTypeDef sConfig = {0};
	init_adc(d, led);
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

	sprintf(str, "chip_vref %ld", vref);
	ssd1306_SetCursor(led, 0, 8);
	ssd1306_WriteString(led, str, Font_6x8, 1);
	ssd1306_UpdateScreen(led);

	//sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
	//HAL_ADC_ConfigChannel(&hadc1, &sConfig);

	HAL_ADC_Start(d);
	HAL_ADC_PollForConversion(d, 1); //wsit for 1mS
	res = HAL_ADC_GetValue(d);
	HAL_ADC_Stop(d);
	idx = __LL_ADC_CALC_TEMPERATURE(vref, res, LL_ADC_RESOLUTION_12B);

	sprintf(str, "chip_temp %ld %ld", idx, res);
	ssd1306_SetCursor(led, 0, 0);
	ssd1306_WriteString(led, str, Font_6x8, 1);
	ssd1306_UpdateScreen(led);
}

int main(void) {
  HAL_Init();
  GPIOinit();

  SW_I2C_HWinit(&si2c1, &hi2c1);
  SW_I2C_HWinit(&si2c2, &hi2c2);
  SW_I2C_SWinit(&si2c3, SI2C1P, SI2C1L, SI2C1P, SI2C1A);
  SW_I2C_SWinit(&si2c4, SI2C2P, SI2C2L, SI2C2P, SI2C2A);
  HAL_ADCEx_Calibration_Start(&hadc1);
  lcd_Init(&LCD1601, &si2c1);
  lcd_Init(&LCD1602, &si2c2);
  lcd_Init(&LCD1603, &si2c3);
  lcd_Init(&LCD1604, &si2c4);
  //SSD1306_gpioinit4W(&SH11061, LCD_CS_P, LCD_CS, LCD_CD_P, LCD_CD);
  SSD1306_gpioinit3W(&SH11061, LCD_CS_P, LCD_CS);
  ssd1306_Init(&SH11061, &hspi1);
  devices |= ina3221_begin(&ina3221, &si2c4) ? 0: DEV_INA3221;
  IP2365_init(&IP23651, &si2c3);
  IP2365_init(&IP23652, &si2c4);
  devices |= sw35xx_init(&SW35181, &si2c1) ? 0: DEV_SW35181;
  devices |= sw35xx_init(&SW35182, &si2c2) ? 0: DEV_SW35182;
  devices |= sw35xx_init(&SW35182, &si2c3) ? 0: DEV_SW35183;
  devices |= sw35xx_init(&SW35182, &si2c4) ? 0: DEV_SW35184;
  ath20_init(&ath20, &si2c3);

  lcd_put_cur(&LCD1601, 0, 0);
  lcd_put_cur(&LCD1602, 0, 0);
  lcd_put_cur(&LCD1603, 0, 0);
  lcd_put_cur(&LCD1604, 0, 0);
  lcd_send_string(&LCD1601, "hwi2c1 test");
  lcd_send_string(&LCD1602, "hwi2c2 test");
  lcd_send_string(&LCD1603, "swi2c1 test");
  lcd_send_string(&LCD1603, "swi2c2 test");

#if 0
  ssd1306_SetCursor(&SH11061, 1, 0);
  ssd1306_WriteString(&SH11061, "font6x8", Font_6x8, 1);
  ssd1306_SetCursor(&SH11061, 65, 0);
  ssd1306_WriteString(&SH11061, "font6x8", Font_6x8, 0);
  ssd1306_UpdateScreen(&SH11061);
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
  HAL_TIM_Base_Start_IT(&htim14);
  init_adc(&hadc1, &SH11061);
  while (1) {
	  //update_ina3221(&ina3221, &SH11061);
	  //update_sw3518(&SW35184, &SH11061);
	  update_ath20(&ath20, &SH11061);
	  update_adc(&hadc1, &SH11061);
	  //HAL_GPIO_WritePin(EVB_LED_P, EVB_LED, GPIO_PIN_SET);
	  HAL_Delay(3000);
	  //update_ina3221(&ina3221, &SH11061);
	  //update_sw3518(&SW35184, &SH11061);
	  update_ath20(&ath20, &SH11061);
	  update_adc(&hadc1, &SH11061);
	  //HAL_GPIO_WritePin(EVB_LED_P, EVB_LED, GPIO_PIN_RESET);
	  HAL_Delay(3000);
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
        if(htim == &htim14) {
                HAL_GPIO_TogglePin(EVB_LED_P, EVB_LED);
        }
}

