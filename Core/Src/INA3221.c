/*

    Arduino library for INA3221 current and voltage sensor.

    MIT License

    Copyright (c) 2020 Beast Devices, Andrejs Bondarevs

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.

*/

#include "INA3221.h"
#include <stddef.h>

#define swapwordp(a) ({ uint16_t __t=(*a)>>8; (*a)<<=8; (*a)|=__t; })

void ina3221_read(struct t_INA3221 *d, uint16_t reg, uint16_t *val) {
    //_i2c->beginTransmission(_i2c_addr);
    //_i2c->write(reg);  // Register
    //_i2c->endTransmission(false);
    //_i2c->requestFrom((uint8_t)_i2c_addr, (uint8_t)2);
    //if (_i2c->available()) { *val = ((_i2c->read() << 8) | _i2c->read()); }
	if(d->_i2c == NULL) return;
    SW_I2C_Read_8addr(d->_i2c, d->_i2c_addr, reg, (uint8_t*)val, 2);
    swapwordp(val);
}

uint16_t ina3221_read16(struct t_INA3221 *d, uint16_t reg) {
	uint16_t val;
	if(d->_i2c == NULL) return 65535;
	SW_I2C_Read_8addr(d->_i2c, d->_i2c_addr, reg, (uint8_t*)&val, 2);
	swapwordp(&val);
	return val;
}

void ina3221_write(struct t_INA3221 *d, uint16_t reg, uint16_t *val) {
    //_i2c->beginTransmission(_i2c_addr);
    //_i2c->write(reg);                 // Register
    //_i2c->write((*val >> 8) & 0xFF);  // Upper 8-bits
    //_i2c->write(*val & 0xFF);         // Lower 8-bits
    //_i2c->endTransmission();
	if(d->_i2c == NULL) return;
	swapwordp(val);
    SW_I2C_Write_8addr(d->_i2c, d->_i2c_addr, reg, (uint8_t*)val, 2);
}

void ina3221_write16(struct t_INA3221 *d, uint16_t reg, uint16_t val) {
	if(d->_i2c == NULL) return;
	swapwordp(&val);
    SW_I2C_Write_8addr(d->_i2c, d->_i2c_addr, reg, (uint8_t*)&val, 2);
}

//void HAL_Delay(uint32_t Delay); //stm32 api
uint8_t ina3221_begin(struct t_INA3221 *d, struct sw_i2c_s *pvDev) {
	if(d==NULL || pvDev==NULL) return 255;
	d->_i2c = pvDev;
	if(d->_i2c_addr == 0) d->_i2c_addr = default_i2caddr;
	d->_shuntRes[0] = 100; //100 mean 100mOhm.
	d->_shuntRes[1] = 100;
	d->_shuntRes[2] = 100;
	d->_filterRes[0] = 0;
	d->_filterRes[1] = 0;
	d->_filterRes[2] = 0;
	ina3221_write16(d, INA3221_REG_CONF, 0x8000); //reset
	//HAL_Delay(10);
	d->_i2c->hal_delay_ms(10);
	ina3221_write16(d, INA3221_REG_CONF, 0x7127); //to default
	//HAL_Delay(2);
	if(ina3221_getDieID(d) == 0x3220) { d->_i2c = NULL; return 0; }
	else return 2;
}

void ina3221_setShuntRes(struct t_INA3221 *d, uint32_t res_ch1, uint32_t res_ch2,
                          uint32_t res_ch3) {
	d->_shuntRes[0] = res_ch1;
	d->_shuntRes[1] = res_ch2;
	d->_shuntRes[2] = res_ch3;
}

void ina3221_setFilterRes(struct t_INA3221 *d, uint32_t res_ch1, uint32_t res_ch2,
                           uint32_t res_ch3) {
	d->_filterRes[0] = res_ch1;
	d->_filterRes[1] = res_ch2;
	d->_filterRes[2] = res_ch3;
}

uint16_t ina3221_getReg(struct t_INA3221 *d, uint16_t reg) {
    uint16_t val = 0;
    ina3221_read(d, reg, &val);
    return val;
}

void ina3221_reset(struct t_INA3221 *d) {
	conf_reg_t CR;
	ina3221_read(d, INA3221_REG_CONF, (uint16_t *)&CR);
	CR.reset = 1;
	ina3221_write(d, INA3221_REG_CONF, (uint16_t *)&CR);
}

void ina3221_setModePowerDown(struct t_INA3221 *d) {
	conf_reg_t CR;
	ina3221_read(d, INA3221_REG_CONF, (uint16_t *)&CR);
	CR.mode_bus_en = 0;
	CR.mode_continious_en = 0;
	ina3221_write(d, INA3221_REG_CONF, (uint16_t *)&CR);
}

void ina3221_setModeContinious(struct t_INA3221 *d) {
	conf_reg_t CR;
	ina3221_read(d, INA3221_REG_CONF, (uint16_t *)&CR);
	CR.mode_continious_en = 1;
	ina3221_write(d, INA3221_REG_CONF, (uint16_t *)&CR);
}

void ina3221_setModeTriggered(struct t_INA3221 *d) {    
	conf_reg_t CR;
	ina3221_read(d, INA3221_REG_CONF, (uint16_t *)&CR);
	CR.mode_continious_en = 0;
	ina3221_write(d, INA3221_REG_CONF, (uint16_t *)&CR);
}

void ina3221_setShuntMeasEnable(struct t_INA3221 *d) {
	conf_reg_t CR;
	ina3221_read(d, INA3221_REG_CONF, (uint16_t *)&CR);
	CR.mode_shunt_en = 1;
	ina3221_write(d, INA3221_REG_CONF, (uint16_t *)&CR);
}

void ina3221_setShuntMeasDisable(struct t_INA3221 *d) {
	conf_reg_t CR;
	ina3221_read(d, INA3221_REG_CONF, (uint16_t *)&CR);
	CR.mode_shunt_en = 0;
	ina3221_write(d, INA3221_REG_CONF, (uint16_t *)&CR);
}

void ina3221_setBusMeasEnable(struct t_INA3221 *d) {
	conf_reg_t CR;
	ina3221_read(d, INA3221_REG_CONF, (uint16_t *)&CR);
	CR.mode_bus_en = 1;
	ina3221_write(d, INA3221_REG_CONF, (uint16_t *)&CR);
}

void ina3221_setBusMeasDisable(struct t_INA3221 *d) {
	conf_reg_t CR;	
	ina3221_read(d, INA3221_REG_CONF, (uint16_t *)&CR);
	CR.mode_bus_en = 0;
	ina3221_write(d, INA3221_REG_CONF, (uint16_t *)&CR);
}

void ina3221_setAveragingMode(struct t_INA3221 *d, ina3221_avg_mode_t mode) {
	conf_reg_t CR;	
	ina3221_read(d, INA3221_REG_CONF, (uint16_t *)&CR);
	CR.avg_mode = mode;
	ina3221_write(d, INA3221_REG_CONF, (uint16_t *)&CR);
}

void ina3221_setBusConversionTime(struct t_INA3221 *d, ina3221_conv_time_t convTime) {
	conf_reg_t CR;
	ina3221_read(d, INA3221_REG_CONF, (uint16_t *)&CR);
	CR.bus_conv_time = convTime;
	ina3221_write(d, INA3221_REG_CONF, (uint16_t *)&CR);
}

void ina3221_setShuntConversionTime(struct t_INA3221 *d, ina3221_conv_time_t convTime) {
	conf_reg_t CR;	
	ina3221_read(d, INA3221_REG_CONF, (uint16_t *)&CR);
	CR.shunt_conv_time = convTime;
	ina3221_write(d, INA3221_REG_CONF, (uint16_t *)&CR);
}

void ina3221_setPwrValidUpLimit(struct t_INA3221 *d, int16_t voltagemV) {
	ina3221_write(d, INA3221_REG_PWR_VALID_HI_LIM, (uint16_t *)&voltagemV);
}

void ina3221_setPwrValidLowLimit(struct t_INA3221 *d, int16_t voltagemV) {
    ina3221_write(d, INA3221_REG_PWR_VALID_LO_LIM, (uint16_t *)&voltagemV);
}

void ina3221_setShuntSumAlertLimit(struct t_INA3221 *d, int32_t voltageuV) {
    int16_t val = 0;
    val         = voltageuV / 20;
    ina3221_write(d, INA3221_REG_SHUNTV_SUM_LIM, (uint16_t *)&val);
}

void ina3221_setCurrentSumAlertLimit(struct t_INA3221 *d, int32_t currentmA) {
    int16_t shuntuV = 0;
    shuntuV         = currentmA * (int32_t)d->_shuntRes[INA3221_CH1];
    ina3221_setShuntSumAlertLimit(d, shuntuV);
}

void ina3221_setWarnAlertLatchEnable(struct t_INA3221 *d) {
	masken_reg_t MR;	
	ina3221_read(d, INA3221_REG_MASK_ENABLE, (uint16_t*)&MR);
	MR.warn_alert_latch_en = 1;
	ina3221_write(d, INA3221_REG_MASK_ENABLE, (uint16_t*)&MR);
	d->_masken_reg = MR;
}

void ina3221_setWarnAlertLatchDisable(struct t_INA3221 *d) {
    masken_reg_t MR;
    ina3221_read(d, INA3221_REG_MASK_ENABLE, (uint16_t*)&MR);
    MR.warn_alert_latch_en = 1;
    ina3221_write(d, INA3221_REG_MASK_ENABLE, (uint16_t*)&MR);
    d->_masken_reg = MR;
}

void ina3221_setCritAlertLatchEnable(struct t_INA3221 *d) {
    masken_reg_t MR;
    ina3221_read(d, INA3221_REG_MASK_ENABLE, (uint16_t*)&MR);
    MR.crit_alert_latch_en = 1;
    ina3221_write(d, INA3221_REG_MASK_ENABLE, (uint16_t*)&MR);
    d->_masken_reg = MR;
}

void ina3221_setCritAlertLatchDisable(struct t_INA3221 *d) {
    masken_reg_t MR;
    ina3221_read(d, INA3221_REG_MASK_ENABLE, (uint16_t*)&MR);
    MR.crit_alert_latch_en = 1;
    ina3221_write(d, INA3221_REG_MASK_ENABLE, (uint16_t*)&MR);
    d->_masken_reg = MR;
}

void ina3221_readFlags(struct t_INA3221 *d) {
	masken_reg_t MR;
    ina3221_read(d, INA3221_REG_MASK_ENABLE, (uint16_t*)&MR);
    d->_masken_reg = MR;
}

uint8_t ina3221_getTimingCtrlAlertFlag(struct t_INA3221 *d) {
    return (uint8_t)d->_masken_reg.timing_ctrl_alert;
}

uint8_t ina3221_getPwrValidAlertFlag(struct t_INA3221 *d) {
    return (uint8_t)d->_masken_reg.pwr_valid_alert;
}

uint8_t ina3221_getCurrentSumAlertFlag(struct t_INA3221 *d) {
    return (uint8_t)d->_masken_reg.shunt_sum_alert;
}

uint8_t ina3221_getConversionReadyFlag(struct t_INA3221 *d) {
    return (uint8_t)d->_masken_reg.conv_ready;
}

uint16_t ina3221_getManufID(struct t_INA3221 *d) {
    //uint16_t id = 0;
    //ina3221_read(d, INA3221_REG_MANUF_ID, &id);
    //return id;
    return ina3221_read16(d, INA3221_REG_MANUF_ID);
}

uint16_t ina3221_getDieID(struct t_INA3221 *d) {
    //uint16_t id = 0;
    //ina3221_read(d, INA3221_REG_DIE_ID, &id);
    //return id;
    return ina3221_read16(d, INA3221_REG_DIE_ID);
}

void ina3221_setChannelEnable(struct t_INA3221 *d, uint8_t channel) {
	conf_reg_t CR;
	ina3221_read(d, INA3221_REG_CONF, (uint16_t*)&CR);
	switch (channel) {
	case INA3221_CH1: CR.ch1_en = 1; break;
	case INA3221_CH2: CR.ch2_en = 1; break;
	case INA3221_CH3: CR.ch3_en = 1; break;
	}
	ina3221_write(d, INA3221_REG_CONF, (uint16_t*)&CR);
}

void ina3221_setChannelDisable(struct t_INA3221 *d, uint8_t channel) {
	conf_reg_t CR;
	ina3221_read(d, INA3221_REG_CONF, (uint16_t*)&CR);
	switch (channel) {
	case INA3221_CH1: CR.ch1_en = 0; break;
	case INA3221_CH2: CR.ch2_en = 0; break;
	case INA3221_CH3: CR.ch3_en = 0; break;
	}
	
	ina3221_write(d, INA3221_REG_CONF, (uint16_t*)&CR);
}

void ina3221_setWarnAlertShuntLimit(struct t_INA3221 *d, uint8_t channel, int32_t voltageuV) {
    int16_t val = voltageuV / 5; uint16_t *pT = (uint16_t*)&val;
    switch (channel) {
        case INA3221_CH1: ina3221_write16(d, INA3221_REG_CH1_WARNING_ALERT_LIM, *pT); break;
        case INA3221_CH2: ina3221_write16(d, INA3221_REG_CH2_WARNING_ALERT_LIM, *pT); break;
        case INA3221_CH3: ina3221_write16(d, INA3221_REG_CH3_WARNING_ALERT_LIM, *pT); break;
    }
}

void ina3221_setCritAlertShuntLimit(struct t_INA3221 *d, uint8_t channel, int32_t voltageuV) {
    int16_t val = voltageuV / 5; uint16_t *pT = (uint16_t*)&val;
    switch (channel) {
        case INA3221_CH1: ina3221_write16(d, INA3221_REG_CH1_CRIT_ALERT_LIM, *pT); break;
        case INA3221_CH2: ina3221_write16(d, INA3221_REG_CH2_CRIT_ALERT_LIM, *pT); break;
        case INA3221_CH3: ina3221_write16(d, INA3221_REG_CH3_CRIT_ALERT_LIM, *pT); break;
    }
}

void ina3221_setWarnAlertCurrentLimit(struct t_INA3221 *d, uint8_t channel,
                                       int32_t currentmA) {
    int32_t shuntuV = 0;
    shuntuV         = currentmA * (int32_t)d->_shuntRes[channel];
    ina3221_setWarnAlertShuntLimit(d, channel, shuntuV);
}

void ina3221_setCritAlertCurrentLimit(struct t_INA3221 *d, uint8_t channel,
                                       int32_t currentmA) {
    int32_t shuntuV = 0;
    shuntuV         = currentmA * (int32_t)d->_shuntRes[channel];
    ina3221_setCritAlertShuntLimit(d, channel, shuntuV);
}

void ina3221_setCurrentSumEnable(struct t_INA3221 *d, uint8_t channel) {
    masken_reg_t MR;
    ina3221_read(d, INA3221_REG_MASK_ENABLE, (uint16_t*)&MR);
    switch (channel) {
        case INA3221_CH1: MR.shunt_sum_en_ch1 = 1; break;
        case INA3221_CH2: MR.shunt_sum_en_ch2 = 1; break;
        case INA3221_CH3: MR.shunt_sum_en_ch3 = 1; break;
    }
    ina3221_write(d, INA3221_REG_MASK_ENABLE, (uint16_t*)&MR);
    d->_masken_reg = MR;
}

void ina3221_setCurrentSumDisable(struct t_INA3221 *d, uint8_t channel) {
    masken_reg_t MR;
    ina3221_read(d, INA3221_REG_MASK_ENABLE, (uint16_t*)&MR);
    switch (channel) {
        case INA3221_CH1: MR.shunt_sum_en_ch1 = 0; break;
        case INA3221_CH2: MR.shunt_sum_en_ch2 = 0; break;
        case INA3221_CH3: MR.shunt_sum_en_ch3 = 0; break;
    }
    ina3221_write(d, INA3221_REG_MASK_ENABLE, (uint16_t*)&MR);
    d->_masken_reg = MR;
}

int16_t ina3221_getShuntVolRaw(struct t_INA3221 *d, uint8_t channel) { //40uV per bit, -32768 ~ +32760
	int16_t v = -1;
    switch (channel) {	//40uV per bit
        case INA3221_CH1: ina3221_read(d, INA3221_REG_CH1_SHUNTV, (uint16_t*)&v); break;
        case INA3221_CH2: ina3221_read(d, INA3221_REG_CH2_SHUNTV, (uint16_t*)&v); break;
        case INA3221_CH3: ina3221_read(d, INA3221_REG_CH3_SHUNTV, (uint16_t*)&v); break;
    }
    return v;
}

int32_t ina3221_getShuntVoltage(struct t_INA3221 *d, uint8_t channel) { //by mV, -163840 ~ +163800
    int32_t res;
    /*int16_t val_raw = 0;
    switch (channel) {
        case INA3221_CH1: ina3221_read(d, INA3221_REG_CH1_SHUNTV, (uint16_t*)&val_raw); break;
        case INA3221_CH2: ina3221_read(d, INA3221_REG_CH2_SHUNTV, (uint16_t*)&val_raw); break;
        case INA3221_CH3: ina3221_read(d, INA3221_REG_CH3_SHUNTV, (uint16_t*)&val_raw); break;
    }

    res = (int32_t)(val_raw >> 3) * 40;*/
    res = (int32_t)ina3221_getShuntVolRaw(d, channel);
    res += res << 2;
    return res;
}


uint8_t ina3221_getWarnAlertFlag(struct t_INA3221 *d, uint8_t channel) {
    switch (channel) {
        case INA3221_CH1: return (uint8_t)d->_masken_reg.warn_alert_ch1; break;
        case INA3221_CH2: return (uint8_t)d->_masken_reg.warn_alert_ch2; break;
        case INA3221_CH3: return (uint8_t)d->_masken_reg.warn_alert_ch3; break;
        default: return 0;
    }
}

uint8_t ina3221_getCritAlertFlag(struct t_INA3221 *d, uint8_t channel) {
    switch (channel) {
        case INA3221_CH1: return (uint8_t)d->_masken_reg.crit_alert_ch1; break;
        case INA3221_CH2: return (uint8_t)d->_masken_reg.crit_alert_ch2; break;
        case INA3221_CH3: return (uint8_t)d->_masken_reg.crit_alert_ch3; break;
        default: return 0;
    }
}

int32_t ina3221_estimateOffsetVoltage(struct t_INA3221 *d, uint8_t channel, uint32_t busV) {
    float bias_in     = 10.0;   // Input bias current at IN– in uA
    float r_in        = 0.670;  // Input resistance at IN– in MOhm
    uint32_t adc_step = 40;     // smallest shunt ADC step in uV
    float shunt_res   = d->_shuntRes[channel] / 1000.0;  // convert to Ohm
    float filter_res  = d->_filterRes[channel];
    int32_t offset    = 0.0;
    float reminder;

    offset = (shunt_res + filter_res) * (busV / r_in + bias_in) -
             bias_in * filter_res;

    // Round the offset to the closest shunt ADC value
    reminder = offset % adc_step;
    if (reminder < adc_step / 2) {
        offset -= reminder;
    } else {
        offset += adc_step - reminder;
    }

    return offset;
}

float ina3221_getCurrent(struct t_INA3221 *d, uint8_t channel) { //by A
    //int32_t shunt_uV = 0;
    float current_A  = 0;

    //shunt_uV  = ina3221_getShuntVoltage(d, channel);
    //current_A = shunt_uV / 1000.0 / (int32_t)d->_shuntRes[channel];
    current_A = (float)ina3221_getShuntVoltage(d, channel);
    current_A /= (1000.0f * (float)d->_shuntRes[channel]);
    return current_A;
}

//mV, low-3bit always 0
int16_t ina3221_getVol_Raw(struct t_INA3221 *d, uint8_t channel) { //mV, low-3bit always 0
	int16_t v = 32767;
	if(d->_i2c == NULL) return -1;
    switch (channel) { //just get, no shift.
	case INA3221_CH1: ina3221_read(d, INA3221_REG_CH1_BUSV, (uint16_t*)&v); break;
	case INA3221_CH2: ina3221_read(d, INA3221_REG_CH2_BUSV, (uint16_t*)&v); break;
	case INA3221_CH3: ina3221_read(d, INA3221_REG_CH3_BUSV, (uint16_t*)&v); break;
    }
    return v;
}

float ina3221_getVoltage(struct t_INA3221 *d, uint8_t channel) { //by V
    float voltage_V = 0.0;
    /*int16_t val_raw = 0;
    switch (channel) {
        case INA3221_CH1: ina3221_read(d, INA3221_REG_CH1_BUSV, (uint16_t*)&val_raw); break;
        case INA3221_CH2: ina3221_read(d, INA3221_REG_CH2_BUSV, (uint16_t*)&val_raw); break;
        case INA3221_CH3: ina3221_read(d, INA3221_REG_CH3_BUSV, (uint16_t*)&val_raw); break;
    }
    voltage_V = (float)(val_raw);*/
    voltage_V = (float)ina3221_getVol_Raw(d, channel);
    voltage_V /= 1000.0f;
    return voltage_V;
}

float ina3221_getCurrentCompensated(struct t_INA3221 *d, uint8_t channel) {
    int32_t shunt_uV  = 0;
    int32_t bus_V     = 0;
    float current_A   = 0.0;
    int32_t offset_uV = 0;

    shunt_uV  = ina3221_getShuntVoltage(d, channel);
    bus_V     = ina3221_getVoltage(d, channel);
    offset_uV = ina3221_estimateOffsetVoltage(d, channel, bus_V);

    current_A = (shunt_uV - offset_uV) / (int32_t)d->_shuntRes[channel] / 1000.0;

    return current_A;
}

int16_t ina3221_getCur_mA(struct t_INA3221 *d, uint8_t channel) {
	int16_t val;
	if(d->_i2c == NULL) return -1;
	//switch (channel) {
	//	case INA3221_CH1: ina3221_read(d, INA3221_REG_CH1_SHUNTV, (uint16_t*)&val); break;
	//	case INA3221_CH2: ina3221_read(d, INA3221_REG_CH2_SHUNTV, (uint16_t*)&val); break;
	//	case INA3221_CH3: ina3221_read(d, INA3221_REG_CH3_SHUNTV, (uint16_t*)&val); break;
	//}
	val = ina3221_getShuntVolRaw(d, channel);
	val >>= 3;
	val += val << 2;
	val /= d->_shuntRes[channel];
	return val;
}
#if 0
int32_t ina3221_getAvgVol(struct t_INA3221 *d, uint8_t channel) {
	struct tAvgVal *t;
	int16_t wVol;
	if(!d || channel > 3 || channel < 1) return 0;
	wVol = ina3221_getVol_Raw(d, channel) >> 3;
	t = &(d->avg[channel  -1]);
	t->VolAvg -= (int32_t)t->VolTab[t->vidx];
	t->VolTab[t->vidx] = wVol;
	t->VolAvg += (int32_t)wVol;
	(t->vidx)++;
	if(t->vidx >= VolTabNum) t->vidx = 0;
	return (t->VolAvg >> ScaleShift);
}

int32_t ina3221_getAvgCur(struct t_INA3221 *d, uint8_t channel) {
	struct tAvgVal *t;
	int16_t wCur;
	if(!d || channel > 3 || channel < 1) return 0;
	//when shunt-res=0.04ohm, wCur=ina3221_getShuntVolRaw*5/40=ina3221_getShuntVolRaw/8
	wCur = ina3221_getShuntVolRaw(d, channel) >> 3;
	t = &(d->avg[channel  -1]);
	t->CurAvg -= (int32_t)t->CurTab[t->cidx];
	t->CurTab[t->cidx] = wCur;
	t->CurAvg += (int32_t)wCur;
	(t->cidx)++;
	if(t->cidx >= CurTabNum) t->cidx = 0;
	return (t->CurAvg >> ScaleShift);
}
#endif
int32_t ina3221_getCurPower(struct t_INA3221 *d, uint8_t channel) { //mW
	if(d->_i2c == NULL) return -1;
	d->voltage[channel] = ina3221_getVol_Raw(d, channel);
	d->curcuit[channel] = ina3221_getCur_mA(d, channel);
	d->power[channel] = (d->voltage[channel] * d->curcuit[channel]) / 1000;
	return d->power[channel];
}
