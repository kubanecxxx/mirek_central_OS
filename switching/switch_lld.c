/**
 * @file switch_lld.c
 * @author kubanec
 * @date 22.8.2012
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "ch.h"
#include "hal.h"
#include "switch_lld.h"
/* Private typedef -----------------------------------------------------------*/

/**
 * @defgroup Relays
 * @brief API pro prácu s efektama s relé
 * @ingroup Switching
 */

/**
 * @notapi
 * @brief Pár gpio + pin
 * @ingroup Relays
 */
typedef struct
{
	GPIO_TypeDef * gpio;
	uint16_t pin;
} switch_relay;

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/**
 * @brief tabulka GPIO+pinů všech relé
 * @ingroup Relays
 */
static const switch_relay relays[RELAY_COUNT] =
{
{ GPIOB, 1 }, //REL1
		{ GPIOB, 0 }, //REL2
		{ GPIOB, 2 }, //REL3
		/*
		 * doesn't work
		 * { GPIOD, 3 }, //REL4
		 */
		{ GPIOD, 6 }, //REL4
		{ GPIOB, 3 }, //REL5
		{ GPIOB, 4 }, //REL6
		{ GPIOE, 2 }, //REL7
		{ GPIOB, 5 }, //REL8
		{ GPIOE, 4 }, //REL9
		{ GPIOE, 5 }, //REL10
		{ GPIOE, 6 }, //REL11
		{ GPIOC, 13 }, //REL12
		{ GPIOC, 14 }, //REL13
		{ GPIOA, 1 }, //REL14
		{ GPIOC, 3 }, //REL15
		};

/**
 * @notapi
 * @ingroup Relays
 */
#define _switch_setRelay(rel)  palSetPad(relays[rel].gpio, relays[rel].pin)
/**
 * @notapi
 * @ingroup Relays
 */
#define _switch_toggleRelay(rel)  palTogglePad(relays[rel].gpio, relays[rel].pin)
/**
 * @notapi
 * @ingroup Relays
 */
#define _switch_clearRelay(relay) palClearPad(relays[relay].gpio, relays[relay].pin)
/**
 * @notapi
 * @ingroup Relays
 */
#define _switch_getRelay(relay) palReadPad(relays[relay].gpio, relays[relay].pin)
/* Private function prototypes -----------------------------------------------*/
#ifdef SWITCHING_TEST
static void switch_demo(void);
#endif
/* Private functions ---------------------------------------------------------*/

/**
 * @brief inicializuje GPIO pro ovládání relé
 * @ingroup Relays
 */
void switch_init(void)
{
	uint8_t i = 0;
	for (i = 0; i < RELAY_COUNT; i++)
	{
		palSetPadMode(relays[i].gpio, relays[i].pin, PAL_MODE_OUTPUT_PUSHPULL);
	}

#ifdef SWITCHING_TEST
	switch_demo();
#endif
}

/**
 * @brief přečte stav všech relátek z pinů
 * @ingroup Relays
 * @return bitmask
 */
uint32_t switch_getRelays(void)
{
	uint8_t i;
	uint32_t temp = 0;

	for (i = 0; i < RELAY_COUNT; i++)
	{
		temp |= _switch_getRelay(i);
		temp <<= 1;
	}

	return temp;
}

/**
 * @brief nastaví stav všech relátek
 * @ingroup Relays
 * @param[in] bitmask
 */
void switch_setRelays(uint32_t _relays)
{
	uint8_t i;

	for (i = 0; i < RELAY_COUNT; i++)
	{
		if (_relays & 1)
			_switch_setRelay(i);
		else
			_switch_clearRelay(i);

		_relays >>= 1;
	}
}

/**
 * @brief zapne jedno relé
 * @ingroup Relays
 * @param[in] číslo relé
 */
void switch_setRelay(uint8_t relay)
{
	if (relay < RELAY_COUNT)
	{
		_switch_setRelay(relay);
	}
}

/**
 * @brief vypne jedno relé
 * @ingroup Relays
 * @param[in] číslo relé
 */
void switch_clearRelay(uint8_t relay)
{
	if (relay < RELAY_COUNT)
	{
		_switch_clearRelay(relay);
	}
}

/**
 * @brief přepne jedno relé
 * @ingroup Relays
 * @param[in] číslo relé
 */
void switch_toggleRelay(uint8_t relay)
{
	if (relay < RELAY_COUNT)
	{
		_switch_toggleRelay(relay);
	}
}

/**
 * @brief přečte stav jednoho relé
 * @ingroup Relays
 * @return boolean
 */
bool_t switch_getRelay(uint8_t relay)
{
	if (relay < RELAY_COUNT)
	{
		return _switch_getRelay(relay);
	}

	return FALSE;
}

#ifdef SWITCHING_TEST
/**
 * @brief demo ktery postupně problinke jedno réle po druhym
 * @ingroup Relays
 * @notapi
 */
static void switch_demo(void)
{
	uint8_t i;
	switch_setRelays(0);
	for (i = 0; i < RELAY_COUNT; i++)
	{
		switch_setRelay(i);
		chThdSleepMilliseconds(1000);
		switch_clearRelay(i);
	}
}
#endif

