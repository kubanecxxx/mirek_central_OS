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
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
Thread * foot_thd;
foot_t footswitch;
//EVENTSOURCE_DECL(i2c_interrupt_event);

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
	//send event k vyššímu zpracování

	footswitch.count = 0;
	footswitch.pin = 0;
}

/**
 * @brief re-enable external interrupt after some miliseconds
 */
static void glitch_timeout(void * data)
{
	extChannelEnableI(data, PCA_INTERRUPT);
}

/**
 * @brief thread for reading button inputs
 */
static void i2c_receive_thread(void * data)
{
	(void) data;
/*
	EventListener el;
	chEvtInit(&i2c_interrupt_event);
	chEvtRegisterMask(&i2c_interrupt_event, &el, 1);
*/

	uint8_t txbuf = PCA_IDR;
	uint8_t rxbuf[2];

	footswitch.count = 0;
	footswitch.pin = 0;

	chThdSleep(TIME_INFINITE );

	while (TRUE)
	{
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

		chThdSleep(TIME_INFINITE );
	}
}

/**
 * @brief external interrupt from PCA_BUTTONS
 */
void foot_buttons_interrupt(EXTDriver *extp, expchannel_t channel)
{
	static VirtualTimer vt;
	static VirtualTimer vt2;

	(void) extp;
	(void) channel;

	chSysLockFromIsr()
	;
	if (chVTIsArmedI(&vt))
	{
		chVTResetI(&vt);
	}

	extChannelDisableI(extp, PCA_INTERRUPT);
	//disable external interrupt to supress glitches
	chVTSetI(&vt2, MS2ST(10), glitch_timeout, extp); //re-enable after 10ms
	chVTSetI(&vt, MS2ST(STEP_TIMEOUT), timeout_cb, NULL ); //no another interrupt in 200ms (last step)
	if (foot_thd->p_state == THD_STATE_SLEEPING)
		chThdResumeI(foot_thd);
	chSysUnlockFromIsr()
	;

}
