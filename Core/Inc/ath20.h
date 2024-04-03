#ifndef __AHT20_H__
#define __AHT20_H__

#include "swi2c.h"
#include "gpiodef.h"
#define ath20_addr	0x38
#define ath20_cmd_pwron		0x71
#define ath20_cmd_init		0xbe
//#define ath20_cmd_trig	0xac
#define ath20_cmd_rst		0xba
#define ath20_cmd_read		0x33ac
#define ath20_wait_delay	100 //mS
#define ath20_retry			10
//class AHT20{    
//private:
//    bool startSensor();
//public:
//    void begin();
//    bool getSensor(float *h, float *t);
//    bool getTemperature(float *t);
//    bool getHumidity(float *h);
//};

typedef struct ath20_t {
	struct tag_swi2c *pDev;
	uint32_t		humidity;
	uint32_t		temprature;
} ath20t;

uint8_t ath20_reset(struct ath20_t *d);
uint8_t ath20_init(struct ath20_t *p, struct tag_swi2c *d);
uint8_t ath20_wait_ready(struct ath20_t *d);
uint8_t ath20_start(struct ath20_t *d);
uint8_t ath20_readraw(struct ath20_t *d);

#endif
