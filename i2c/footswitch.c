/**
 * @file footswitch.c
 * @author kubanec
 * @date 10.8.2012
 *
 */

/* Includes ------------------------------------------------------------------*/

#include "footswitch.h"
#include "logic_types.h"
#include "ssd1289/print.h"
#include "ssd1289/ssd1289_lld.h"

/**
 * @addtogroup FOOTSWITCH
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define EVENT_ID 1
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
Thread * foot_thd;
static volatile foot_t footswitch;
foot_t foot_switch;
EVENTSOURCE_DECL(event_i2c_buttons);
extern const logic_base_t * base;

/* Private function prototypes -----------------------------------------------*/
static WORKING_AREA(wa_i2c_receive_thread,256);
static void i2c_receive_thread(void * data) __attribute__ ((noreturn));
static void timeout_cb(void * data);
static uint8_t preprcat(uint8_t data);
/* Private functions ---------------------------------------------------------*/

/*
 * jenom napsat funkce na posilání ledek přes i2c, pak načitání footswitchů,
 * použit externi přerušeni
 * vystupem bude čislo tlačitka a kolikrát zamačknuty, ideálně nějaké event
 * načitání tlačitka z harmonizeru se bude řešit jako pořád vlákno
 */

/*
 * přerušeni spusti i2c vyčist PCA, zároveň se odpálí časovač a pokud nepřinde
 * dalši přerušeni tak se vyšle čislo tlačitka a jedno sešlápnuti
 * pokud přinde přerušeni tak se znova vyčte a zase se odpáli časovač
 */

/**
 * @brief initialize footswitch PCAs, configure external interrupt
 */
#ifdef I2C_FOOTSWITCH
void foot_init(void)
{
	uint8_t txbuf[3];
	uint8_t err;

	/*
	 * config footswitch PCAs direction
	 */
	txbuf[0] = PCA_DDR;
	txbuf[1] = 0; //output
	i2cAcquireBus(&I2CD1);
	err = i2cMasterTransmitTimeout(&I2CD1, PCA_LED_1_ADDRESS, txbuf, 2, NULL, 0,
			TIME_INFINITE );
	i2cMasterTransmitTimeout(&I2CD1, PCA_LED_2_ADDRESS, txbuf, 2, NULL, 0,
			TIME_INFINITE );
	txbuf[1] = 0xFF; //input
	i2cMasterTransmitTimeout(&I2CD1, PCA_BUTTONS_ADDRESS, txbuf, 2, NULL, 0,
			TIME_INFINITE );
	txbuf[0] = PCA_POL;
	txbuf[1] = 0xFF; //inverse input polarity
	i2cMasterTransmitTimeout(&I2CD1, PCA_BUTTONS_ADDRESS, txbuf, 2, NULL, 0,
			TIME_INFINITE );
	i2cReleaseBus(&I2CD1);

	/*
	 * external interrupt has been already configured in main.c
	 */

	/*
	 * make i2c_receive_thread
	 */
	foot_thd = chThdCreateStatic(&wa_i2c_receive_thread,
			sizeof(wa_i2c_receive_thread), NORMALPRIO,
			(tfunc_t) i2c_receive_thread, NULL );
}

/**
 * @brief last step timeout
 */
static void timeout_cb(void * data)
{
	(void) data;

	uint16_t temp = 0;

	temp = preprcat(footswitch.pin);

	foot_switch.count = footswitch.count;
	foot_switch.pin = temp;

	footswitch.count = 0;
	footswitch.pin = 0;

	if (foot_switch.count == 0 || foot_switch.pin == 0)
		return;

	//broadcast event
	chSysLockFromIsr()
	;
	chEvtBroadcastFlagsI(&event_i2c_buttons, BUTTON_EVENT_ID);
	chSysUnlockFromIsr()
	;
}

/**
 * @brief thread for reading button inputs
 */
static void i2c_receive_thread(void * data)
{
	(void) data;
	static uint8_t temp2 = 0;
	char retaz[10];

	chRegSetThreadName("i2c footswitch");
	uint8_t txbuf = PCA_IDR;
	uint8_t rxbuf[2];

	footswitch.count = 0;
	footswitch.pin = 0;

	while (TRUE)
	{
		//čeká na přerušení
		chEvtWaitAll(EVENT_ID);

		i2cAcquireBus(&I2CD1);
		i2cMasterTransmit(&I2CD1, PCA_BUTTONS_ADDRESS, &txbuf, 1, rxbuf, 2);
		i2cReleaseBus(&I2CD1);

		if (rxbuf[0] != 0)
		{
			if (footswitch.pin != 0)
			{
				//other reads
				if (footswitch.pin == rxbuf[0])
					//same button
					footswitch.count++;
				else
				{
					//different button
					footswitch.count = 1;
					footswitch.pin = rxbuf[0];
				}
			}
			else
			{
				//first read
				footswitch.pin = rxbuf[0];
				footswitch.count++;
			}
			uint8_t temp = preprcat(footswitch.pin);

			foot_switch.count = footswitch.count;
			foot_switch.pin = temp;
			chEvtBroadcastFlags(&event_i2c_buttons, BUTTON_NOW_EVENT_ID);
			static uint32_t brodcasty = 0;
			brodcasty++;
#if 0
			if (temp == 2)
			{
				chsprintf(retaz,"%d",temp2++);
				disp_PutsStringBackground(retaz,200,10,LCD_GREEN,0,16);

			}
#endif
		}
	}
}

static void glitch(void * data)
{

	chSysLockFromIsr()
	;
	extChannelEnableI(data, 15);

	chSysUnlockFromIsr()
	;

}

static uint8_t preprcat(uint8_t data)
{
	uint8_t temp = 0;
	//bit swap - wrong soldering ...
	temp |= (data & 0b00100000) << 1;
	temp |= (data & 0b01000000) >> 6;
	temp |= (data & 0b00000001) << 2;
	temp |= (data & 0b00000010) << 2;
	temp |= (data & 0b00000100) << 2;
	temp |= (data & 0b00001000) << 2;
	temp |= (data & 0b10000000) >> 6;
	temp |= (data & 0b00010000) << 3;

	return temp;
}

/**
 * @brief external interrupt from PCA_BUTTONS
 * @notapi
 */
void foot_buttons_interrupt(EXTDriver *extp, expchannel_t channel)
{
	static VirtualTimer vt;
	static VirtualTimer vt2;

	(void) channel;

	chSysLockFromIsr()
	;
	if (chVTIsArmedI(&vt))
	{
		chVTResetI(&vt);
	}
	extChannelDisableI(extp, channel);
	chVTSetI(&vt, MS2ST(base->time), timeout_cb, NULL ); //no another interrupt in 200ms (last step)
	chVTSetI(&vt2, MS2ST(10), (vtfunc_t) glitch, extp);

	chEvtSignalFlagsI(foot_thd, EVENT_ID);
	chSysUnlockFromIsr()
	;
}

/**
 * @brief PCA leds function
 * @note internal use only
 * @notapi
 */
void _foot_SetLeds(uint8_t address, uint8_t data)
{
	uint8_t txbuf[2];
	uint8_t temp;
	txbuf[0] = PCA_ODR;

	//bit swap because wrong soldering...
	temp = data;
	temp &= 0x0F;
	temp |= ((data & 0b10000000) >> 1);
	temp |= ((data & 0b01000000) << 1);
	temp |= ((data & 0b00100000) >> 1);
	temp |= ((data & 0b00010000) << 1);

	txbuf[1] = temp;
	i2cAcquireBus(&I2CD1);
	i2cMasterTransmit(&I2CD1, address, txbuf, 2, NULL, 0);
	i2cReleaseBus(&I2CD1);
}

/**
 * @brief PCA leds function
 * @note internal use only
 * @notapi
 */
uint8_t _foot_GetLeds(uint8_t address)
{
	uint8_t txbuf[2];
	uint8_t rxbuf[2];
	uint8_t temp;
	txbuf[0] = PCA_IDR;

	i2cAcquireBus(&I2CD1);
	i2cMasterTransmit(&I2CD1, address, txbuf, 1, rxbuf, 2);
	i2cReleaseBus(&I2CD1);

	temp = rxbuf[0];
	temp &= 0x0F;
	temp |= ((rxbuf[0] & 0b10000000) >> 1);
	temp |= ((rxbuf[0] & 0b01000000) << 1);
	temp |= ((rxbuf[0] & 0b00100000) >> 1);
	temp |= ((rxbuf[0] & 0b00010000) << 1);

	return temp;
}

/**
 * @}
 */
#endif
