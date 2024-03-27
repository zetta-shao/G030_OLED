
#include "sw_i2c.h"

// #include "stm32f1xx_hal.h"
#include "gpiodef.h"

//#define SW_I2C1_SCL_PORT    GPIOB
//#define SW_I2C1_SDA_PORT    GPIOB
//#define SW_I2C1_SCL_PIN     GPIO_PIN_6
//#define SW_I2C1_SDA_PIN     GPIO_PIN_7

void SW_I2C_initial(sw_i2c_t *d);

//引脚置位
static void GPIO_SetBits(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
}

//引脚复位
static void GPIO_ResetBits(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);
}

//读引脚状态
static uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
	return (uint8_t)HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
}

//SDA引脚切换输入模式
static void sda_in_mode(struct sw_i2c_g *p)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Pin = p->SDApin;
    HAL_GPIO_Init(p->SDAport, &GPIO_InitStruct);
}

//SDA引脚切换输出模式
static void sda_out_mode(struct sw_i2c_g *p)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;

    GPIO_InitStruct.Pin = p->SDApin;
    HAL_GPIO_Init(p->SDAport, &GPIO_InitStruct);
}

//SCL引脚切换输入模式
static void scl_in_mode(struct sw_i2c_g *p) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;

    GPIO_InitStruct.Pin = p->SCLpin;
    HAL_GPIO_Init(p->SCLport, &GPIO_InitStruct);
}

//SCL引脚切换输出模式
static void scl_out_mode(struct sw_i2c_g *p) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;

    GPIO_InitStruct.Pin = p->SCLpin;
    HAL_GPIO_Init(p->SCLport, &GPIO_InitStruct);
}
#if 0
static void enable_clock(void* GPIOport) {
	switch((uint32_t)GPIOport) {
	case (uint32_t)GPIOA: __HAL_RCC_GPIOA_CLK_ENABLE(); break;
	case (uint32_t)GPIOB: __HAL_RCC_GPIOB_CLK_ENABLE(); break;
	case (uint32_t)GPIOC: __HAL_RCC_GPIOC_CLK_ENABLE(); break;
	}
}
#endif
static int sw_i2c_port_initial(struct sw_i2c_g *pIIC) {
	if(!pIIC) return 1;
	//enable_clock(pIIC->SDAport);
	//enable_clock(pIIC->SCLport);
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    // i2c_sw SCL
    GPIO_InitStruct.Pin = pIIC->SCLpin;
    HAL_GPIO_Init(pIIC->SCLport, &GPIO_InitStruct);
    // i2c_sw SDA
    GPIO_InitStruct.Pin = pIIC->SDApin;
    HAL_GPIO_Init(pIIC->SDAport, &GPIO_InitStruct);
    return 0;
}

static void sw_i2c_port_delay_us(uint32_t us) {
	//uint32_t nCount = us/10*25;
	//nCount = (nCount>0) ? nCount : 1;
	//for (; nCount > 0; nCount--);
	STM32_DELAY_US(us);
}

static void sw_i2c_port_delay_ms(uint32_t ms) { STM32_DELAY_MS(ms); }

//static int i2c_transmit(void *pv, uint16_t addr, uint8_t *pData, uint16_t wDataSize) {
//	return (int)HAL_I2C_Master_Transmit((I2C_HandleTypeDef *)pv, (addr<<1), pData, wDataSize, 0x100);
//}

static int i2c_transmit_mem(void *pv, uint16_t addr, uint16_t memadr, uint8_t adrsize, uint8_t *pData, uint16_t wDataSize) {
	if(adrsize == 0)
		return (HAL_I2C_Master_Transmit((I2C_HandleTypeDef *)pv, (addr<<1), pData, wDataSize, 0x100) == HAL_OK) ? 0 : 1;
	else
		return (HAL_I2C_Mem_Write((I2C_HandleTypeDef *)pv, (addr<<1), memadr, adrsize, pData, wDataSize, 0x100) == HAL_OK) ? 0 : 1;
}

//static int i2c_receive(void *pv, uint16_t addr, uint8_t *pData, uint16_t wDataSize) {
//	return (int)HAL_I2C_Master_Receive((I2C_HandleTypeDef *)pv, (addr<<1), pData, wDataSize, 0x100);
//}

static int i2c_receive_mem(void *pv, uint16_t addr, uint16_t memadr, uint8_t adrsize, uint8_t *pData, uint16_t wDataSize) {
	if(adrsize == 0)
		return (HAL_I2C_Master_Transmit((I2C_HandleTypeDef *)pv, (addr<<1), pData, wDataSize, 0x100) == HAL_OK) ? 0 : 1;
	else
		return (HAL_I2C_Mem_Read((I2C_HandleTypeDef *)pv, (addr<<1), memadr, adrsize, pData, wDataSize, 0x100) == HAL_OK) ? 0 : 1;
}

static int sw_i2c_port_io_ctl(uint8_t opt, void *param) {
    int ret = -1;
    struct sw_i2c_s *d=(struct sw_i2c_s*)param;
    struct sw_i2c_g *p=(struct sw_i2c_g*)&d->gpio;
    switch (opt)
    {
    case HAL_IO_OPT_SET_SDA_HIGH:
        GPIO_SetBits(p->SDAport, p->SDApin);
        break;
    case HAL_IO_OPT_SET_SDA_LOW:
        GPIO_ResetBits(p->SDAport, p->SDApin);
        break;
    case HAL_IO_OPT_GET_SDA_LEVEL:
        ret = GPIO_ReadInputDataBit(p->SDAport, p->SDApin);
        break;
    case HAL_IO_OPT_SET_SDA_INPUT:
        sda_in_mode(p);
        break;
    case HAL_IO_OPT_SET_SDA_OUTPUT:
        sda_out_mode(p);
        break;
    case HAL_IO_OPT_SET_SCL_HIGH:
        GPIO_SetBits(p->SCLport, p->SCLpin);
        break;
    case HAL_IO_OPT_SET_SCL_LOW:
        GPIO_ResetBits(p->SCLport, p->SCLpin);
        break;
    case HAL_IO_OPT_GET_SCL_LEVEL:
        ret = GPIO_ReadInputDataBit(p->SCLport, p->SCLpin);
        break;
    case HAL_IO_OPT_SET_SCL_INPUT:
        scl_in_mode(p);
        break;
    case HAL_IO_OPT_SET_SCL_OUTPUT:
        scl_out_mode(p);
        break;
    default:
        break;
    }
    return ret;
}

/*sw_i2c_t swi2c1 = {
    .hal_init = sw_i2c_port_initial,
    .hal_io_ctl = sw_i2c_port_io_ctl,
    .hal_delay_us = sw_i2c_port_delay_us,
	{ SI2C1P, SI2C1P, SI2C1A, SI2C1L, 0, 0 }
    };*/

void __HAL_init__(void *pvswI2Ct, void* pSDAport, void* pSCLport, uint16_t bSDApin, uint16_t bSCLpin) {
	struct sw_i2c_s *d=(struct sw_i2c_s*)pvswI2Ct;
	//struct sw_i2c_g *p=(struct sw_i2c_g*)&d->gpio;
	d->hal_init = sw_i2c_port_initial;
    d->hal_io_ctl = sw_i2c_port_io_ctl;
    d->hal_delay_us = sw_i2c_port_delay_us;
    d->hal_delay_ms = sw_i2c_port_delay_ms;
    d->gpio.hal_transmit = i2c_transmit_mem;
    d->gpio.hal_receive = i2c_receive_mem;
    d->gpio.SDAport = pSDAport;
    d->gpio.SCLport = pSCLport;
    d->gpio.SDApin = bSDApin;
    d->gpio.SCLpin = bSCLpin;
    //if(pSCLport != NULL)	SW_I2C_initial(d);
}
