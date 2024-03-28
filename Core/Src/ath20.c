// code base on https://github.com/Seeed-Studio/Seeed_Arduino_AHT20.git
// AHT20 ARDUINO LIBRARY
#include "ath20.h"

#if 0
void AHT20::begin() {
    Wire.begin();
    Wire.beginTransmission(0x38); // transmit to device #8
    Wire.write(0xBE);
    Wire.endTransmission();    // stop transmitting
}
#endif
#if 0
bool AHT20::startSensor() {
    Wire.beginTransmission(0x38); // transmit to device #8
    Wire.write(0xac);
    Wire.write(0x33);
    Wire.write(0x00);
    Wire.endTransmission();    // stop transmitting
    unsigned long timer_s = millis();
    while(1) {
        if(millis()-timer_s > 200) return 0;        // time out
        Wire.requestFrom(0x38, 1);
        while(Wire.available()) {
            unsigned char c = Wire.read();
            if(c&0x80 != 0)return 1;      // busy
        }
        delay(20);
    }
}
#endif
#if 0
bool AHT20::getSensor(float *h, float *t) {
    startSensor();
    Wire.requestFrom(0x38, 6);
    unsigned char str[6] ={0,};
    int index = 0;
    while (Wire.available()) {
        str[index++] = Wire.read(); // receive a byte as character
    }
    if(index == 0 )return 0; 
    if(str[0] & 0x80)return 0;
    unsigned long __humi = 0;
    unsigned long __temp = 0;
    __humi = str[1];
    __humi <<= 8;
    __humi += str[2];
    __humi <<= 4;
    __humi += str[3] >> 4;
    *h = (float)__humi/1048576.0;
    __temp = str[3]&0x0f;
    __temp <<=8;
    __temp += str[4];
    __temp <<=8;
    __temp += str[5];
    *t = (float)__temp/1048576.0*200.0-50.0;
    return 1;
}
#endif

uint8_t ath20_wait_ready(struct ath20_t *d) {
	uint8_t val, idx = 0;
	if(d->pDev == NULL) return 255;
	for(idx=0; idx<ath20_retry; idx++) {
		HAL_Delay(ath20_wait_delay);
		if(SW_I2C_Read_0addr(d->pDev, ath20_addr, &val, 1) != 0) return 3;
		if((val & 128) == 0) return 0;
	}
	return (idx==ath20_retry)? 2 : 0;
}

uint8_t ath20_reset(struct ath20_t *d) {
	uint8_t res, val = ath20_cmd_rst;
	if(d->pDev == NULL) return 255;
	res = SW_I2C_Write_0addr(d->pDev, ath20_addr, &val, 1);
	res |= ath20_wait_ready(d);
	return res;
}

uint8_t ath20_init(struct ath20_t *d, struct tag_swi2c *s) {
	uint8_t res;
	int32_t val = ath20_cmd_init;
	//int32_t val = ath20_cmd_pwron;
	if(!s||!d) return 255;
	d->pDev = s;
	//ath20_reset(d);
	res = SW_I2C_Write_0addr(d->pDev, ath20_addr, (uint8_t*)&val, 1);
	res |= ath20_wait_ready(d);
	if(res != 0) d->pDev = NULL;
	return res;
}

uint8_t ath20_start(struct ath20_t *d) { //wait for ready?
	uint8_t cmd[8];
	int32_t *dwT = (int32_t*)cmd;
	if(d->pDev == NULL) return 255;
	dwT[0] = ath20_cmd_read;
	dwT[1] = 0;
	cmd[7] = SW_I2C_Write_0addr(d->pDev, ath20_addr, cmd, 3); //start sampling
	cmd[7] = ath20_wait_ready(d); //wait ready
	if(cmd[7] != 0) return cmd[7];
	cmd[7] = SW_I2C_Read_0addr(d->pDev, ath20_addr, cmd, 7);
	d->humidity = (cmd[1] << 12) | (cmd[2] << 4) | (cmd[3] >> 4);
	d->temprature = ((cmd[3] & 15) << 16) | (cmd[4] << 8) | cmd[5];
	return 0;
}
