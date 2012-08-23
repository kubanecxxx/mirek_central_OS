/*
 * optocouplers.c
 *
 *  Created on: 23.8.2012
 *      Author: kubanec
 */

/********************************************************************
 * Includes
 ********************************************************************/
#include "ch.h"
#include "hal.h"
#include "optocouplers.h"

/********************************************************************
 * Private typedefs
 ********************************************************************/
typedef struct
{
	GPIO_TypeDef * gpio;
	uint16_t pin;
} opto_gpio_t;

typedef struct
{
	opto_gpio_t led;
	opto_gpio_t drain;
} opto_both_t;

typedef union
{
	struct
	{
		opto_both_t overdrive;
		opto_both_t tuner;
	} s;

	opto_both_t a[2];
} opto_effects_t;

/********************************************************************
 * Exported global variables
 ********************************************************************/
opto_bitStruct_t _opto_states;
/********************************************************************
 * Private global variables
 ********************************************************************/
#define OPTO_THREAD
#ifdef OPTO_THREAD
static WORKING_AREA(wa_opto,256);
#endif

/********************************************************************
 * Private constants
 ********************************************************************/
//todo doplnit tabulku podle schematu
static const opto_effects_t opto_effects =
{ GPIOA, 3, GPIOB, 3, GPIOA, 2, GPIOB, 1 };

/********************************************************************
 * Private function prototypes
 ********************************************************************/
#ifdef OPTO_THREAD
static void opto_thread(void * data); //todo noreturn
#endif

/********************************************************************
 * Private functions
 ********************************************************************/
void opto_init(void)
{
	uint8_t i;

	for (i = 0; i < 2; i++)
	{
		palSetPadMode(opto_effects.a[i].led.gpio, opto_effects.a[i].led.pin,
				PAL_MODE_OUTPUT_PUSHPULL);
		palSetPadMode(opto_effects.a[i].drain.gpio, opto_effects.a[i].drain.pin,
				PAL_MODE_INPUT);
	}
#ifdef OPTO_THREAD
	chThdCreateStatic(&wa_opto, sizeof(wa_opto), NORMALPRIO,
			(tfunc_t) opto_thread, NULL );
#endif
}

#ifdef OPTO_THREAD
/**
 * @brief vlákno ktery hlidá jesli je efekt zapnuté jestli má
 */
static void opto_thread(void * data)
{
	(void) data;
	uint8_t i;
	bool_t temp;

	while (TRUE)
	{
//todo možná bych sem dal wait event na přerušeni od ledek nebo
//rovnou cely v externim přerušení + virtual timer

		for (i = 0; i < 2; i++)
		{
			temp =
					palReadPad(opto_effects.a[i].drain.gpio,opto_effects.a[i].drain.pin);

			if (temp != opto_getEffect(i))
			{
				palClearPad(opto_effects.a[i].led.gpio,
						opto_effects.a[i].led.pin);
				chThdSleepMilliseconds(20);
				palSetPad(opto_effects.a[i].led.gpio,
						opto_effects.a[i].led.pin);
			}
		}
	}
}
#endif
