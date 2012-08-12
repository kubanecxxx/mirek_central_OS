/**
 * @file touch.c
 * @author kubanec
 * @date 12.8.2012
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "ch.h"
#include "hal.h"
#include "touch.h"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
static const SPIConfig ls_spicfg =
{ NULL, GPIOB, 12, SPI_CR1_BR_2 | SPI_CR1_BR_1 };

/* Private variables ---------------------------------------------------------*/
volatile Thread * thd_touch;
volatile touch_t touch_coor;
EVENTSOURCE_DECL(event_touch);
volatile touch_state_t touch_state = FREE;

/* Private function prototypes -----------------------------------------------*/
static WORKING_AREA(wa_touch,256);
static void touch_thread(void * data) __attribute__ ((noreturn));

/* Private functions ---------------------------------------------------------*/
/**
 * @brief touch panel module init
 */
void touch_init(void)
{
	/*
	 * pins are already configured from board.h
	 */
	spiAcquireBus(&SPID2);
	spiStart(&SPID2, &ls_spicfg);
	spiUnselect(&SPID2);

	spiReleaseBus(&SPID2);

	thd_touch = chThdCreateStatic(&wa_touch, sizeof(wa_touch), NORMALPRIO - 2,
			(tfunc_t) touch_thread, NULL );
}

/**
 * @brief read touch data each after each external interrupt from touch
 */
static void touch_thread(void * data)
{
	(void) data;
	chRegSetThreadName("touch");

	while (TRUE)
	{
		chEvtWaitAll(1);

		touch_coor.x = touch_getX();
		touch_coor.y = touch_getY();

		if (touch_coor.y > 0 && touch_coor.x > 0)
		{
			touch_state = PUSHED;
			chEvtBroadcastFlags(&event_touch, TOUCH_PUSH);

			while (touch_state == PUSHED)
			{
				touch_coor.x = touch_getX();
				touch_coor.y = touch_getY();

				chThdSleepMilliseconds(5);
				if (touch_coor.x == 0 && touch_coor.y == 0)
					touch_state = FREE;
			}

		}
		chEvtBroadcastFlags(&event_touch, TOUCH_PULL);
		touch_state = FREE;
	}
}

/**
 * @brief read touch A/D value
 */
uint16_t _touch_get(uint8_t reg)
{
	uint8_t txbuf[3];
	uint8_t rxbuf[3];

	txbuf[0] = reg;
	txbuf[1] = 0;
	txbuf[2] = 0;
	spiAcquireBus(&SPID2);
	spiSelect(&SPID2);
	spiExchange(&SPID2, 3, txbuf, rxbuf);
	//wait for A/D conversion complete
	spiUnselect(&SPID2);
	spiReleaseBus(&SPID2);

	return ((rxbuf[1] << 4) | (rxbuf[2] >> 4));
}

touch_state_t touch_getState(void)
{
	return touch_state;
}

static void timeout_cb(void *data)
{
	chSysLockFromIsr()
	;
	extChannelEnableI(data, 11);
	chSysUnlockFromIsr()
	;
}

/**
 * @brief touch pannel external interrupt callback
 */
void touch_interrupt(EXTDriver *extp, expchannel_t channel)
{
	static VirtualTimer vt;

	chSysLockFromIsr()
	;
	extChannelDisableI(extp, channel);
	chVTSetI(&vt, MS2ST(50), timeout_cb, extp);
	chEvtSignalFlagsI((Thread*) thd_touch, 1);
	chSysUnlockFromIsr()
	;
}

