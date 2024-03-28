#include "IP2365.h"
#include <stddef.h>

void writeReg(IP2365_t *d, uint8_t reg, uint8_t val) {
	if(!d || d->pD == NULL) return;
	SW_I2C_Write_8addr(d->pD, i2caddr, reg, &val, 1);
}

void writeRegP(IP2365_t *d, uint8_t reg, uint8_t *val) {
	if(!d || d->pD == NULL) return;
	SW_I2C_Write_8addr(d->pD, i2caddr, reg, val, 1);
}

uint8_t readReg(IP2365_t *d, uint8_t reg) {
	uint8_t val;
	if(!d || d->pD == NULL) return 255;
	SW_I2C_Read_8addr(d->pD, i2caddr, reg, &val, 1);
	return val;
}

void readRegP(IP2365_t *d, uint8_t reg, uint8_t *val) {
	if(!d || d->pD == NULL) return;
	SW_I2C_Read_8addr(d->pD, i2caddr, reg, val, 1);
}

void IP2365_init(IP2365_t *d, swi2c_t *pDev) {
	uint8_t val;
	d->pD = pDev;
	if(SW_I2C_Read_8addr(d->pD, i2caddr, IP2365_REG_SYSCTL1, &val, 1) != 0) //getChargeEnable
		d->pD = NULL;
}

void set_ChargeEnable(IP2365_t *d, uint8_t enable) {
	reg_sysctl1_t r;
	if(d->pD == NULL) return;
	r = (reg_sysctl1_t)readReg(d, IP2365_REG_SYSCTL1);
	r.charge_enable = (enable) ? 1 : 0;
	writeRegP(d, IP2365_REG_SYSCTL1, (uint8_t*)&r);
}

uint8_t getChargeEnable(IP2365_t *d) {
	reg_sysctl1_t r;
	if(d->pD == NULL) return 255;
	r = (reg_sysctl1_t)readReg(d, IP2365_REG_SYSCTL1);
	return r.charge_enable;
}

uint16_t get_VinOVmode(IP2365_t *d) {
	reg_vinovset_t r;
	if(d->pD == NULL) return 65535;
	r = (reg_vinovset_t)readReg(d, IP2365_REG_VINOVSET);
	return r.unuse_vinsen;
}

void set_VinOVmode(IP2365_t *d, uint8_t enable) {
	reg_vinovset_t r;
	if(d->pD == NULL) return;
	r = (reg_vinovset_t)readReg(d, IP2365_REG_VINOVSET);
	r.unuse_vinsen = (enable==0)?0:1;
	writeRegP(d, IP2365_REG_VINOVSET, (uint8_t*)&r);
}

uint16_t get_VinOVmV(IP2365_t *d) {
	reg_vinovset_t r;
	if(d->pD == NULL) return 65535;
	r = (reg_vinovset_t)readReg(d, IP2365_REG_VINOVSET);
	//uint8_t val = r.winsen;
	if(r.unuse_vinsen) { return 2700; }
	else {
		switch(r.winsen) {
			case 0: return 300; break;
			case 1: return 280; break;
			case 2: return 260; break;
			case 3: return 220; break;
		}
	}
	return 0;
}

void set_VinOVmV(IP2365_t *d, uint16_t val) {
	reg_vinovset_t r;
	r = (reg_vinovset_t)readReg(d, IP2365_REG_VINOVSET);
	if(d->pD == NULL) return;
	r.unuse_vinsen = 0;
	if(val < 260) r.winsen = 3;
	else if(val >= 260 && val < 280) r.winsen = 2;
	else if(val >= 280 && val < 300) r.winsen = 1;
	else r.winsen = 0;
	if(val >= 2700) r.unuse_vinsen = 1;
	writeRegP(d, IP2365_REG_VINOVSET, (uint8_t*)&r);
}

uint16_t get_NTCthres_mV(IP2365_t *d, uint8_t level) {
	reg_ntc_ctl_t r;
	if(d->pD == NULL) return 65535;
	r = (reg_ntc_ctl_t)readReg(d, IP2365_REG_NTC_CTL);
	//uint8_t res = readReg(d, IP2365_REG_NTC_CTL);
	//if((res & 1)==0) return 0;
	if(! r.ntc_enable) return 0;
	if(level==0) {
		switch(r.ntc_vol_low) {
			case 0: return 152; break;
			case 1: return 149; break;
			case 2: return 144; break;
			case 3: return 132; break;
		}
	} else if(level==1) {
		switch(r.ntc_vol_mid) {
			case 0: return 60; break;
			case 1: return 58; break;
			case 2: return 56; break;
			case 3: return 54; break;
		}
	} else if(level==2) {
		switch(r.ntc_vol_high) {
			case 0: return 56; break;
			case 1: return 49; break;
			case 2: return 43; break;
			case 3: return 38; break;
		}
	}
	return 0;
}

void set_NTCthres_mV(IP2365_t *d, uint8_t level, uint16_t val) {
	reg_ntc_ctl_t r;
	if(d->pD == NULL) return;
	r = (reg_ntc_ctl_t)readReg(d, IP2365_REG_NTC_CTL);
	//uint8_t res = readReg(d, IP2365_REG_NTC_CTL);
	//if(val==0) { res &= ~1; writeReg(d, IP2365_REG_NTC_CTL, res); }
	if(val==0) { r.ntc_enable=0; level=3; } else r.ntc_enable = 1;
	if(level==0) {		
		if(val <= 132) r.ntc_vol_low = 3;
		else if(val > 132 && val <= 144) r.ntc_vol_low = 2;
		else if(val > 144 && val <= 149) r.ntc_vol_low = 1;
		else r.ntc_vol_low = 0;
	} else if(level==1) {
		if(val <= 54) r.ntc_vol_mid = 3;
		else if(val > 54 && val <= 56) r.ntc_vol_mid = 2;
		else if(val > 56 && val <= 58) r.ntc_vol_mid = 1;
		else r.ntc_vol_mid = 0;
	} else if(level==2) {
		if(val <= 38) r.ntc_vol_high = 3;
		else if(val > 38 && val <= 43) r.ntc_vol_high = 2;
		else if(val > 43 && val <= 49) r.ntc_vol_high = 1;
		else r.ntc_vol_high = 0;
	}
	writeRegP(d, IP2365_REG_NTC_CTL, (uint8_t*)&r);
}

uint8_t get_Isetvin(IP2365_t *d) {
	reg_r_iset_t e;
	reg_vin_iset_t r;
	if(d->pD == NULL) return 255;
	e = (reg_r_iset_t)readReg(d, IP2365_REG_R_ISET);
	r = (reg_vin_iset_t)readReg(d, IP2365_REG_VIN_ISET);
	if(e.use_r_iadp == 0) return 255;
	return r.iset_vin;
}

uint8_t get_Isetbat(IP2365_t *d) {
	reg_r_iset_t e;
	reg_bat_iset_t r;
	if(d->pD == NULL) return 255;
	e = (reg_r_iset_t)readReg(d, IP2365_REG_R_ISET);
	r = (reg_bat_iset_t)readReg(d, IP2365_REG_BAT_ISET);
	if(e.use_r_ichg == 0) return 255;
	return r.iset_bat;
}

void set_Isetvin(IP2365_t *d, uint8_t val) {
	reg_r_iset_t e;
	reg_vin_iset_t r;
	if(d->pD == NULL) return;
	e = (reg_r_iset_t)readReg(d, IP2365_REG_R_ISET);
	r = (reg_vin_iset_t)readReg(d, IP2365_REG_VIN_ISET);
	e.use_r_iadp = (val > 63) ? 0 : 1;
	if(val < 64) {
	r.iset_vin = val & 63;
	writeRegP(d, IP2365_REG_VIN_ISET, (uint8_t*)&r); }
	writeRegP(d, IP2365_REG_R_ISET, (uint8_t*)&e);
}

void set_Isetbat(IP2365_t *d, uint8_t val) {
	reg_r_iset_t e;
	reg_bat_iset_t r;
	if(d->pD == NULL) return;
	e = (reg_r_iset_t)readReg(d, IP2365_REG_R_ISET);
	r = (reg_bat_iset_t)readReg(d, IP2365_REG_BAT_ISET);
	e.use_r_ichg = (val > 63) ? 0 : 1;
	if(val < 64) {
	r.iset_bat = val & 63;
	writeRegP(d, IP2365_REG_BAT_ISET, (uint8_t*)&r); }
	writeRegP(d, IP2365_REG_R_ISET, (uint8_t*)&e);
}

uint8_t get_LED_stat_chgfull(IP2365_t *d) {
	reg_led_t r;
	if(d->pD == NULL) return 255;
	r = (reg_led_t)readReg(d, IP2365_REG_LED);
	return r.stat_chgful;
}

uint8_t get_LED_stat_chging(IP2365_t *d) {
	reg_led_t r;
	if(d->pD == NULL) return 255;
	r = (reg_led_t)readReg(d, IP2365_REG_LED);
	return r.stat_chging;
}

void set_LED_stat_chging(IP2365_t *d, uint8_t stat) {
	reg_led_t r;
	if(d->pD == NULL) return;
	r = (reg_led_t)readReg(d, IP2365_REG_LED);
	r.stat_chging = stat & 3;
	writeRegP(d, IP2365_REG_LED, (uint8_t*)&r);
}

uint8_t get_chg_stat_full(IP2365_t *d) {
	reg_chg_full_t r;
	if(d->pD == NULL) return 255;
	r = (reg_chg_full_t)readReg(d, IP2365_REG_CHG_FULL);
	return r.stat_charge_full;
}

uint8_t get_chg_stat_cicv(IP2365_t *d) {
	reg_chg_full_t r;
	if(d->pD == NULL) return 255;
	r = (reg_chg_full_t)readReg(d, IP2365_REG_CHG_FULL);
	return r.stat_charge;
}

uint8_t get_vin_isov(IP2365_t *d) {
	reg_ntc_stat_t r;
	if(d->pD == NULL) return 255;
	r = (reg_ntc_stat_t)readReg(d, IP2365_REG_NTC_STAT);
	return r.vin_ov;
}

uint8_t get_ntc_stat(IP2365_t *d) {
	reg_ntc_stat_t r;
	if(d->pD == NULL) return 255;
	r = (reg_ntc_stat_t)readReg(d, IP2365_REG_NTC_STAT);
	return r.stat_ntc;
}

