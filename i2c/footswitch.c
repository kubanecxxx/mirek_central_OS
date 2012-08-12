/**
 * @file footswitch.c
 * @author kubanec
 * @date 10.8.2012
 *
 */

/* Includes ------------------------------------------------------------------*/

#include "footswitch.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define EVENT_ID 1
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
Thread * foot_thd;
volatile foot_t footswitch;
EVENTSOURCE_DECL(event_i2c_buttons);

/* Private function prototypes -----------------------------------------------*/
static WORKING_AREA(wa_i2c_receive_thread,256);
static void i2c_receive_thread(void * data) __attribute__ ((noreturn));
static void timeout_cb(void * data);
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
void foot_init(void)
{
	uint8_t txbuf[3];

	/*
	 * config footswitch PCAs direction
	 */
	txbuf[0] = PCA_DDR;
	txbuf[1] = 0; //output
	i2cAcquireBus(&I2CD1);
	i2cMasterTransmitTimeout(&I2CD1, PCA_LED_1_ADDRESS, txbuf, 2, NULL, 0,
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
	//broadcast event
	chSysLockFromIsr()
	;
	chEvtBroadcastFlagsI(&event_i2c_buttons, BUTTON_EVENT_ID);
	chSysUnlockFromIsr()
	;

	footswitch.count = 0;
	footswitch.pin = 0;
}

/**
 * @brief thread for reading button inputs
 */
static void i2c_receive_thread(void * data)
{
	(void) data;

	uint8_t txbuf = PCA_IDR;
	uint8_t rxbuf[2];

	footswitch.count = 0;
	footswitch.pin = 0;

	while (TRUE)
	{
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
		}
	}
}

/**
 * @brief external interrupt from PCA_BUTTONS
 */
void foot_buttons_interrupt(EXTDriver *extp, expchannel_t channel)
{
	static VirtualTimer vt;

	(void) extp;
	(void) channel;

	chSysLockFromIsr()
	;
	if (chVTIsArmedI(&vt))
	{
		chVTResetI(&vt);
	}
	chVTSetI(&vt, MS2ST(STEP_TIMEOUT), timeout_cb, NULL ); //no another interrupt in 200ms (last step)

	chEvtSignalFlagsI(foot_thd, EVENT_ID);
	chSysUnlockFromIsr()
	;
}

/**
 * @brief PCA leds function
 * @note internal use only
 */
void _foot_SetLeds(uint8_t address, uint8_t data)
{
	uint8_t txbuf[2];
	txbuf[0] = PCA_ODR;
	txbuf[1] = data;
	i2cAcquireBus(&I2CD1);
	i2cMasterTransmit(&I2CD1, address, txbuf, 2, NULL, 0);
	i2cReleaseBus(&I2CD1);
}
