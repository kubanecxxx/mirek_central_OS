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
/**
 * @brief pár GPIO + pin
 * @ingroup Optočleny
 * @notapi
 */
typedef struct
{
	GPIO_TypeDef * gpio;
	uint16_t pin;
} opto_gpio_t;

/**
 * @brief dvojce pinů pro optočleny
 * @ingroup Optočleny
 * @notapi
 */
typedef struct
{
	///Nastavení efektu optočlenem (emulace sešlápnutí tlačítka)
	opto_gpio_t led;
	///čtení stavu efektu podle jeho ledky
	opto_gpio_t drain;
} opto_both_t;

/**
 * @brief počet efektů z optočlenama
 * @ingroup Optočleny
 */
#define OPTO_COUNT 2

/**
 * @brief sada několika efektů
 * @ingroup Optočleny
 * @notapi
 */
typedef union
{
	opto_both_t a[OPTO_COUNT];
} opto_effects_t;

/********************************************************************
 * Exported global variables
 ********************************************************************/
opto_bitStruct_t _opto_states;
/********************************************************************
 * Private global variables
 ********************************************************************/
/**
 * @brief work area pro vlákno na hlidání stavu
 * @ingroup Optočleny
 * @notapi
 */
static WORKING_AREA(wa_opto,256);

/********************************************************************
 * Private constants
 ********************************************************************/
/**
 * @brief tabulka optočlenů
 * @ingroup Optočleny
 * @notapi
 */
static const opto_effects_t opto_effects =
{ GPIOC, 2, GPIOC, 4, GPIOC, 1, GPIOC, 5 };

/********************************************************************
 * Private function prototypes
 ********************************************************************/
static void opto_thread(void * data); //todo noreturn

/********************************************************************
 * Private functions
 ********************************************************************/
/**
 * @brief init GPIO pro ovládání optočlenů + vytvoření vlákna
 * @ingroup Optočleny
 */
void opto_init(void)
{
	uint8_t i;

	for (i = 0; i < OPTO_COUNT; i++)
	{
		palSetPadMode(opto_effects.a[i].led.gpio, opto_effects.a[i].led.pin,
				PAL_MODE_OUTPUT_PUSHPULL);
		palSetPadMode(opto_effects.a[i].drain.gpio, opto_effects.a[i].drain.pin,
				PAL_MODE_INPUT_PULLUP);

		palClearPad(opto_effects.a[i].led.gpio, opto_effects.a[i].led.pin);
	}

	chThdCreateStatic(&wa_opto, sizeof(wa_opto), NORMALPRIO,
			(tfunc_t) opto_thread, NULL );
}

/**
 * @brief vlákno ktery hlidá jesli je efekt zapnuté jestli má
 * @ingroup Optočleny
 * @notapi
 */
static void opto_thread(void * data)
{
	(void) data;
	uint8_t i;
	bool_t temp;
	chRegSetThreadName("opto");

	while (TRUE)
	{
		for (i = 0; i < OPTO_COUNT; i++)
		{
			temp =
					!palReadPad(opto_effects.a[i].drain.gpio,opto_effects.a[i].drain.pin);

			if (temp != opto_getEffect(i))
			{
				palSetPad(opto_effects.a[i].led.gpio,
						opto_effects.a[i].led.pin);
				chThdSleepMilliseconds(50);
				palClearPad(opto_effects.a[i].led.gpio,
						opto_effects.a[i].led.pin);
			}
		}
		chThdSleepMilliseconds(100);
		///@todo todo tady by šlo použit čekat  na event z externiho přerušeni...
	}
}
