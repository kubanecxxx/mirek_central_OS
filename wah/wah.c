/**
 * @file wah.c
 * @author kubanec
 * @date 13.10.2012
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "ch.h"
#include "hal.h"
#include "logic_use.h"
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
#define WAH_PIN 3
#define WAH_PORT GPIOE

extern logic_active_t active;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static WORKING_AREA(wa_wah, 256);
static uint8_t state;
static uint8_t prev_state;

/* Private function prototypes -----------------------------------------------*/
void wah_thread(void * arg);
/* Private functions ---------------------------------------------------------*/

void wah_init(void)
{
	chThdCreateStatic(&wa_wah, sizeof(wa_wah), NORMALPRIO - 3, (tfunc_t) wah_thread,
			NULL );
}

bool_t wah_isEnabled(void)
{
	if (state == 0)
		return TRUE;
	else
		return FALSE;
}

void wah_event(void)
{
	prev_state = ~state;
}

void wah_thread(void * arg)
{
	(void) arg;

	logic_marshall_t marsh;

	chRegSetThreadName("wah");

	palSetPadMode(WAH_PORT, WAH_PIN, PAL_MODE_INPUT_PULLUP);

	while (TRUE)
	{
		state = palReadPad(WAH_PORT,WAH_PIN);

		if (state != prev_state)
		{
			prev_state = state;
			if (state == 0)
			{
				//wah on
				memcpy(&marsh,
						&active.bank->channels[active.activeChannel-1].marshall,
						sizeof(logic_marshall_t));

				//marsh.gain = active.bank->wah.gain;
				/*
				if (marsh.volume != 4)
				*/
					marsh.volume = active.bank->wah.volume;


				logic_marshallSetup(&marsh);
			}
			else
			{
				//wah off
				logic_marshallSetup(
						&active.bank->channels[active.activeChannel-1].marshall);
			}
		}

		/// todo šlo by sem hodit event z přerušení... možná v budoucnu
		chThdSleep(MS2ST(20) );
	}
}
