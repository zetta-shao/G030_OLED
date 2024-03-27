#ifndef _INA3221_H_
#define _INA3221_H_
//#include "Arduino.h"
//#include "Wire.h"
#include "sw_i2c.h"

/*typedef enum {
    INA3221_ADDR40_GND = 0b1000000,  // A0 pin -> GND
    INA3221_ADDR41_VCC = 0b1000001,  // A0 pin -> VCC
    INA3221_ADDR42_SDA = 0b1000010,  // A0 pin -> SDA
    INA3221_ADDR43_SCL = 0b1000011   // A0 pin -> SCL
} ina3221_addr_t;*/

#define INA3221_ADDR40_GND	0x40  // A0 pin -> GND
#define INA3221_ADDR41_VCC	0x41  // A0 pin -> VCC
#define INA3221_ADDR42_SDA	0x42  // A0 pin -> SDA
#define INA3221_ADDR43_SCL	0x43  // A0 pin -> SCL
#define default_i2caddr INA3221_ADDR40_GND

// Channels
/*typedef enum {
    INA3221_CH1 = 0,
    INA3221_CH2,
    INA3221_CH3,
    INA3221_CH_NUM
} ina3221_ch_t;*/

#define INA3221_CH1		1
#define INA3221_CH2		2
#define INA3221_CH3		3
#define INA3221_CH_NUM	3

// Registers
typedef enum {
    INA3221_REG_CONF = 0,
    INA3221_REG_CH1_SHUNTV,
    INA3221_REG_CH1_BUSV,
    INA3221_REG_CH2_SHUNTV,
    INA3221_REG_CH2_BUSV,
    INA3221_REG_CH3_SHUNTV,
    INA3221_REG_CH3_BUSV,
    INA3221_REG_CH1_CRIT_ALERT_LIM,
    INA3221_REG_CH1_WARNING_ALERT_LIM,
    INA3221_REG_CH2_CRIT_ALERT_LIM,
    INA3221_REG_CH2_WARNING_ALERT_LIM,
    INA3221_REG_CH3_CRIT_ALERT_LIM,
    INA3221_REG_CH3_WARNING_ALERT_LIM,
    INA3221_REG_SHUNTV_SUM,
    INA3221_REG_SHUNTV_SUM_LIM,
    INA3221_REG_MASK_ENABLE,
    INA3221_REG_PWR_VALID_HI_LIM,
    INA3221_REG_PWR_VALID_LO_LIM,
    INA3221_REG_MANUF_ID = 0xFE,
    INA3221_REG_DIE_ID   = 0xFF
} ina3221_reg_t;

// Conversion times
typedef enum {
    INA3221_REG_CONF_CT_140US = 0,
    INA3221_REG_CONF_CT_204US,
    INA3221_REG_CONF_CT_332US,
    INA3221_REG_CONF_CT_588US,
    INA3221_REG_CONF_CT_1100US,
    INA3221_REG_CONF_CT_2116US,
    INA3221_REG_CONF_CT_4156US,
    INA3221_REG_CONF_CT_8244US
} ina3221_conv_time_t;

// Averaging modes
typedef enum {
    INA3221_REG_CONF_AVG_1 = 0,
    INA3221_REG_CONF_AVG_4,
    INA3221_REG_CONF_AVG_16,
    INA3221_REG_CONF_AVG_64,
    INA3221_REG_CONF_AVG_128,
    INA3221_REG_CONF_AVG_256,
    INA3221_REG_CONF_AVG_512,
    INA3221_REG_CONF_AVG_1024
} ina3221_avg_mode_t;

// Configuration register
/*typedef struct {
	uint16_t mode_shunt_en : 1;
	uint16_t mode_bus_en : 1;
	uint16_t mode_continious_en : 1;
	uint16_t shunt_conv_time : 3;
	uint16_t bus_conv_time : 3;
	uint16_t avg_mode : 3;
	uint16_t ch3_en : 1;
	uint16_t ch2_en : 1;
	uint16_t ch1_en : 1;
	uint16_t reset : 1;
} __attribute__((packed)) conf_reg_t;*/
typedef union {
	struct {
	uint16_t mode_shunt_en : 1;
	uint16_t mode_bus_en : 1;
	uint16_t mode_continious_en : 1;
	uint16_t shunt_conv_time : 3;
	uint16_t bus_conv_time : 3;
	uint16_t avg_mode : 3;
	uint16_t ch3_en : 1;
	uint16_t ch2_en : 1;
	uint16_t ch1_en : 1;
	uint16_t reset : 1;
	} __attribute__((packed));
	uint16_t conf_reg;
} conf_reg_t;

// Mask/Enable register
/*typedef struct {
	uint16_t conv_ready : 1;
	uint16_t timing_ctrl_alert : 1;
	uint16_t pwr_valid_alert : 1;
	uint16_t warn_alert_ch3 : 1;
	uint16_t warn_alert_ch2 : 1;
	uint16_t warn_alert_ch1 : 1;
	uint16_t shunt_sum_alert : 1;
	uint16_t crit_alert_ch3 : 1;
	uint16_t crit_alert_ch2 : 1;
	uint16_t crit_alert_ch1 : 1;
	uint16_t crit_alert_latch_en : 1;
	uint16_t warn_alert_latch_en : 1;
	uint16_t shunt_sum_en_ch3 : 1;
	uint16_t shunt_sum_en_ch2 : 1;
	uint16_t shunt_sum_en_ch1 : 1;
	uint16_t reserved : 1;
} __attribute__((packed)) masken_reg_t;*/
typedef union {
	struct {
	uint16_t conv_ready : 1;
	uint16_t timing_ctrl_alert : 1;
	uint16_t pwr_valid_alert : 1;
	uint16_t warn_alert_ch3 : 1;
	uint16_t warn_alert_ch2 : 1;
	uint16_t warn_alert_ch1 : 1;
	uint16_t shunt_sum_alert : 1;
	uint16_t crit_alert_ch3 : 1;
	uint16_t crit_alert_ch2 : 1;
	uint16_t crit_alert_ch1 : 1;
	uint16_t crit_alert_latch_en : 1;
	uint16_t warn_alert_latch_en : 1;
	uint16_t shunt_sum_en_ch3 : 1;
	uint16_t shunt_sum_en_ch2 : 1;
	uint16_t shunt_sum_en_ch1 : 1;
	uint16_t reserved : 1;
	} __attribute__((packed));
	uint16_t masken_reg;
} masken_reg_t;

// voltage sense is 10mV by 1 LSB in INA3221's 12bit ADC, range is -32768mV - 32767mV, low-3bit
// (0,1,2) always 0, so real minimal resolution is 10*8=80mV. so i added it 16 times, and divided by 2,
// got minimal resolution = 80 / (16/2) = 10mV.

// shunt voltage sense is 5uV by 1 LSB in INA3221's 12bit ADC, range is -163.84mV - 163.8mV, low-3bit
// (0,1,2) always 0, so real minimal resolution is 5*8=40uV. so i added it 8 times, and divided by 2,
// got minimal resolution = 40 / (8/2) = 10uV. minimal current resolution is 10uV / 100mOhm = 0.0001(A)=0.1mA

#define AvgVal
#define VolTabNum 16
#define CurTabNum 2
#define ScaleShift 1 // div by 2

typedef struct tAvgVal {
	int32_t		VolAvg;
	int32_t		CurAvg;
	int16_t		VolTab[VolTabNum];
	int16_t		CurTab[CurTabNum];
	int16_t		vidx, cidx;
} AvgVal_t;

typedef struct t_INA3221 {
	sw_i2c_t	*_i2c;
	uint16_t	_i2c_addr;
    // Value of Mask/Enable register.
	masken_reg_t 	_masken_reg; //uint16_t
    // Shunt resistance in mOhm
	uint32_t	_shuntRes[INA3221_CH_NUM];
    // Series filter resistance in Ohm
	uint32_t	_filterRes[INA3221_CH_NUM];
	int32_t		voltage[3];
	int32_t		curcuit[3];
	int32_t		power[3];
#if 0
	AvgVal_t		avg[3];
#endif
} INA3221_t;

	// Reads 16 bytes from a register.
	void ina3221_read(struct t_INA3221 *d, uint16_t reg, uint16_t *val);

	// Writes 16 bytes to a register.
	void ina3221_write(struct t_INA3221 *d, uint16_t reg, uint16_t *val);

	// Initializes INA3221
	uint8_t ina3221_begin(struct t_INA3221 *d, struct sw_i2c_s *pvDev);

	// Sets shunt resistor value in mOhm
	void ina3221_setShuntRes(struct t_INA3221 *d, uint32_t res_ch1, uint32_t res_ch2, uint32_t res_ch3);

	// Sets filter resistors value in Ohm
	void ina3221_setFilterRes(struct t_INA3221 *d, uint32_t res_ch1, uint32_t res_ch2, uint32_t res_ch3);

	// Gets a register value.
	uint16_t ina3221_getReg(struct t_INA3221 *d, uint16_t reg);

	// Resets INA3221
	void ina3221_reset(struct t_INA3221 *d);

	// Sets operating mode to power-down
	void ina3221_setModePowerDown(struct t_INA3221 *d);

	// Sets operating mode to continious
	void ina3221_setModeContinious(struct t_INA3221 *d);

	// Sets operating mode to triggered (single-shot)
	void ina3221_setModeTriggered(struct t_INA3221 *d);

	// Enables shunt-voltage measurement
	void ina3221_setShuntMeasEnable(struct t_INA3221 *d);

	// Disables shunt-voltage mesurement
	void ina3221_setShuntMeasDisable(struct t_INA3221 *d);

	// Enables bus-voltage measurement
	void ina3221_setBusMeasEnable(struct t_INA3221 *d);

	// Disables bus-voltage measureement
	void ina3221_setBusMeasDisable(struct t_INA3221 *d);

	// Sets averaging mode. Sets number of samples that are collected
	// and averaged togehter.
	void ina3221_setAveragingMode(struct t_INA3221 *d, ina3221_avg_mode_t mode);

	// Sets bus-voltage conversion time.
	void ina3221_setBusConversionTime(struct t_INA3221 *d, ina3221_conv_time_t convTime);

	// Sets shunt-voltage conversion time.
	void ina3221_setShuntConversionTime(struct t_INA3221 *d, ina3221_conv_time_t convTime);

	// Sets power-valid upper-limit voltage. The power-valid condition
	// is reached when all bus-voltage channels exceed the value set.
	// When the powervalid condition is met, the PV alert pin asserts high.
	void ina3221_setPwrValidUpLimit(struct t_INA3221 *d, int16_t voltagemV);

	// Sets power-valid lower-limit voltage. If any bus-voltage channel drops
	// below the power-valid lower-limit, the PV alert pin pulls low.
	void ina3221_setPwrValidLowLimit(struct t_INA3221 *d, int16_t voltagemV);

	// Sets the value that is compared to the Shunt-Voltage Sum register value
	// following each completed cycle of all selected channels to detect
	// for system overcurrent events.
	void ina3221_setShuntSumAlertLimit(struct t_INA3221 *d, int32_t voltagemV);

	// Sets the current value that is compared to the sum all currents.
	// This function is a helper for setShuntSumAlertLim(). It onverts current
	// value to shunt voltage value.
	void ina3221_setCurrentSumAlertLimit(struct t_INA3221 *d, int32_t currentmA);

	// Enables warning alert latch.
	void ina3221_setWarnAlertLatchEnable(struct t_INA3221 *d);

	// Disables warning alert latch.
	void ina3221_setWarnAlertLatchDisable(struct t_INA3221 *d);

	// Enables critical alert latch.
	void ina3221_setCritAlertLatchEnable(struct t_INA3221 *d);

	// Disables critical alert latch.
	void ina3221_setCritAlertLatchDisable(struct t_INA3221 *d);

	// Reads flags from Mask/Enable register.
	// When Mask/Enable register is read, flags are cleared.
	// Use getTimingCtrlAlertFlag(), getPwrValidAlertFlag(),
	// getCurrentSumAlertFlag() and getConvReadyFlag() to get flags after
	// readFlags() is called.
	void ina3221_readFlags(struct t_INA3221 *d);

	// Gets timing-control-alert flag indicator.
	uint8_t ina3221_getTimingCtrlAlertFlag(struct t_INA3221 *d);

	// Gets power-valid-alert flag indicator.
	uint8_t ina3221_getPwrValidAlertFlag(struct t_INA3221 *d);

	// Gets summation-alert flag indicator.
	uint8_t ina3221_getCurrentSumAlertFlag(struct t_INA3221 *d);

	// Gets Conversion-ready flag.
	uint8_t ina3221_getConversionReadyFlag(struct t_INA3221 *d);

	// Gets manufacturer ID.
	// Should read 0x5449.
	uint16_t ina3221_getManufID(struct t_INA3221 *d);

	// Gets die ID.
	// Should read 0x3220.
	uint16_t ina3221_getDieID(struct t_INA3221 *d);

	// Enables channel measurements
	void ina3221_setChannelEnable(struct t_INA3221 *d, uint8_t channel);

	// Disables channel measurements
	void ina3221_setChannelDisable(struct t_INA3221 *d, uint8_t channel);

	// Sets warning alert shunt voltage limit
	void ina3221_setWarnAlertShuntLimit(struct t_INA3221 *d, uint8_t channel, int32_t voltageuV);

	// Sets critical alert shunt voltage limit
	void ina3221_setCritAlertShuntLimit(struct t_INA3221 *d, uint8_t channel, int32_t voltageuV);

	// Sets warning alert current limit
	void ina3221_setWarnAlertCurrentLimit(struct t_INA3221 *d, uint8_t channel, int32_t currentmA);

	// Sets critical alert current limit
	void ina3221_setCritAlertCurrentLimit(struct t_INA3221 *d, uint8_t channel, int32_t currentmA);

	// Includes channel to fill Shunt-Voltage Sum register.
	void ina3221_setCurrentSumEnable(struct t_INA3221 *d, uint8_t channel);

	// Excludes channel from filling Shunt-Voltage Sum register.
	void ina3221_setCurrentSumDisable(struct t_INA3221 *d, uint8_t channel);

	// Gets shunt voltage in uV.
	int32_t ina3221_getShuntVoltage(struct t_INA3221 *d, uint8_t channel);

	// Gets warning alert flag.
	uint8_t ina3221_getWarnAlertFlag(struct t_INA3221 *d, uint8_t channel);

	// Gets critical alert flag.
	uint8_t ina3221_getCritAlertFlag(struct t_INA3221 *d, uint8_t channel);

	// Estimates offset voltage added by the series filter resitors
	int32_t ina3221_estimateOffsetVoltage(struct t_INA3221 *d, uint8_t channel, uint32_t busVoltage);

	// Gets current in A.
	float ina3221_getCurrent(struct t_INA3221 *d, uint8_t channel);

	int16_t ina3221_getVol_Raw(struct t_INA3221 *d, uint8_t channel);

	// Gets current compensated with calculated offset voltage.
	float ina3221_getCurrentCompensated(struct t_INA3221 *d, uint8_t channel);

	// Gets bus voltage in V.
	float ina3221_getVoltage(struct t_INA3221 *d, uint8_t channel);

	int16_t ina3221_getShuntVolRaw(struct t_INA3221 *d, uint8_t channel);
#define ina3221_getVol_mV ina3221_getVol_Raw

	int16_t ina3221_getCur_mA(struct t_INA3221 *d, uint8_t channel);
#if 0
	int32_t ina3221_getAvgVol(struct t_INA3221 *d, uint8_t channel); //mV
	int32_t ina3221_getAvgCur(struct t_INA3221 *d, uint8_t channel);
#endif
	int32_t ina3221_getCurPower(struct t_INA3221 *d, uint8_t channel);

#endif
