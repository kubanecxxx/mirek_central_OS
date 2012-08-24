/**
 * @file logic_use.c
 * @author kubanec
 * @date 23.8.2012
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "ch.h"
#include "hal.h"
#include "logic_types.h"
#include "logic_use.h"
#include "switch_master.h"
#include "i2c_user.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
	const logic_bank_t * bank;
	const logic_function_t * activeFunctions[10];
	uint8_t count;
	uint8_t activeChannel;
} logic_active_t;
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
WORKING_AREA(wa_blinking, 256);

logic_active_t active;
/* Private function prototypes -----------------------------------------------*/
static void logic_specific(const logic_specific_t * arg);
static void logic_blinkingThread(void * data);

/* Private functions ---------------------------------------------------------*/
void logic_init(void)
{
	//switch_masterGpioInit();

	active.bank = NULL;
	//inicializovat aktivni

	// todo zajistit aby věci byly v ramce co maji byt

	chThdCreateStatic(&wa_blinking, sizeof(wa_blinking), NORMALPRIO,
			(tfunc_t) logic_blinkingThread, NULL );
}

/**
 * @brief provede co je potřeba pro funkci
 * @note
 * posledních 8 bitů je pro I2C a opto
 * zbytek jsou efekty pro relé
 *
 * @param[in] function setup
 * @ingroup LOGIC_HL
 *
 * @todo todo dodat podmínku předchozího kanálu
 */
static void logic_function(const logic_function_t * arg)
{
	//I2C effects 28 - harmonist ,29 - delay
	uint64_t temp = arg->effects.w;
	logic_dibit_t temp_i2c = arg->effects;
	uint8_t temp_opto = (temp >> 60) & 0b1111;
	temp &= 0x00FFFFFFFFFFFFFF;

	logic_specific(arg->special);

	//relays
	uint8_t i;
	logic_effect_t tmp;
	for (i = 0; i < 28; i++)
	{
		tmp = temp & 0b11;
		temp >>= 2;

		if (tmp == EFF_ENABLE)
			switch_setRelay(i);
		else if (tmp == EFF_DISABLE)
			switch_clearRelay(i);
		else if (tmp == EFF_TOGGLE)
			switch_toggleRelay(i);
	}

	//opto
	for (i = 0; i < 2; i++)
	{
		tmp = temp_opto & 0b11;
		temp_opto >>= 2;

		if (tmp == EFF_ENABLE)
			opto_enableEffect(i);
		else if (tmp == EFF_DISABLE)
			opto_disableEffect(i);
		else if (tmp == EFF_TOGGLE)
			opto_toggleEffect(i);
	}

	//harmonist
	if (temp_i2c.s.eff28 == EFF_ENABLE)
		harm_enable();
	else if (temp_i2c.s.eff28 == EFF_DISABLE)
		harm_disable();
	else if (temp_i2c.s.eff28 == EFF_TOGGLE)
		harm_toggle();

	//delay
	if (temp_i2c.s.eff29 == EFF_ENABLE)
		delay_on();
	else if (temp_i2c.s.eff29 == EFF_DISABLE)
		delay_off();
	else if (temp_i2c.s.eff29 == EFF_TOGGLE)
		delay_toggle();

	active.activeFunctions[active.count++] = arg;
}

/**
 * @brief provede co je potřeba pro kanál
 * @note
 * poslední 4 bity jsou pro I2C a opto
 * zbytek jsou efekty pro relé
 *
 * @param[in] channel setup
 * @ingroup LOGIC_HL
 */
static void logic_channel(const logic_channel_t * arg)
{
	uint32_t temp = arg->effects.w;
	//I2C bits 28 - harmonist ,29 - delay
	logic_bit_t i2c_temp = arg->effects;
	//opto bits 30,31
	uint8_t temp_opto = (temp >> 30) & 0b11;
	temp &= 0x0FFFFFFF;

	logic_specific(arg->special);

	//relays
	switch_setRelays(temp);

	//optocouplers
	opto_setEffects(temp_opto);

	//harmonist
	if (i2c_temp.s.bit28)
		harm_enable();
	else
		harm_disable();

	//delay
	if (i2c_temp.s.bit29)
		delay_on();
	else
		delay_off();
}

/**
 * @brief přemapuje jedno volání
 * @todo todo musi se zajistit aby calls byly v ramce
 * @note musí se zajistit aby calls byly v ramce...
 */
static void logic_remap(const logic_remap_t * arg)
{
	uint8_t remapIndex = arg->remapIndex;

	arg->button->calls[remapIndex] = arg->newCall;
}

/**
 * @brief specielní nastavení efektů harmonist, delay + blikání ledek
 * @ingroup LOGIC_HL
 * @param[in] struktura specifického nastavení
 */
static void logic_specific(const logic_specific_t * arg)
{
	if (arg == NULL )
		return;

	delay_time(arg->delay.time);
	delay_volume(arg->delay.volume);

	harm_mode(arg->harmonist.mode);
	harm_key(arg->harmonist.key);
	harm_volume(arg->harmonist.volume);
	harm_harmony(arg->harmonist.harmony);
}

/**
 * @brief zavolá všechny volání od tlačitka
 * @todo todo dodělat podporu pro hold - bude lepši při volání podle typu eventu
 * @todo todo dodělat podporu pro okamžitě sepnout  a čekat - taky podle typu eventu
 *
 * @ingroup LOGIC_HL
 */
void logic_button(const logic_bank_t * bank, const foot_t * button)
{
	active.bank = bank;
	//tohle bude volat funkce above
	//volání bude probihat někde v samostatnym vlákně
	logic_button_t * but;
	uint8_t prevChannel = active.activeChannel;

	active.count = 0;

	uint8_t i;

	//prvni musi najit button podle čisla a počtu zmačknuti
	for (i = 0; i < bank->buttonCount; i++)
	{
		but = &bank->buttons[i];

		if (but->number == button->pin && but->pushCount == button->count)
		{
			break;
		}
		but = NULL;
	}

	//žádnej neni mapovanej
	if (but == NULL )
		return;

	logic_buttonCall_t * call;
	logic_channel_t * channel;
	logic_function_t * func;
	logic_remap_t * remap;

	for (i = 0; i < but->buttonCallCount; i++)
	{
		call = &(but->calls[i]);

		if (call->callType == callType_channel)
		{
			channel = (logic_channel_t *) call->call;
			logic_channel(channel);
			active.activeChannel = channel->index;
		}
		else if (call->callType == callType_function)
		{
			func = (logic_function_t *) call->call;
			if (func->channelCondition == prevChannel)
				logic_function(func);
		}
		else if (call->callType == callType_remap)
		{
			remap = (logic_remap_t *) call->call;
			if (remap->channelCondition == prevChannel)
				logic_remap(remap);
		}
	}
}

/**
 * @brief vykrade funkce ktery sou namapovany na tlačitka a rozbliká podle toho jejich ledky
 * @ingroup LOGIC_HL
 *
 */
static void logic_blinkingThread(void * data)
{
	(void) data;

	/*
	 * zjistit banku
	 * zjistit tlačítka v bance
	 * každy tlačitko projit
	 * v každym najit přemapování
	 * z každyho přemapování vytáhnout ledky
	 * blikat ledkama ktery nejsou COL_NONE
	 */

	uint8_t i, j;
///číslo ledky + barva
	logic_blinking pole[10];
	logic_remap_t * remap;
	uint8_t yellow, green;

	while (TRUE)
	{
		while (active.bank == NULL )
		{
			chThdSleepMilliseconds(1000);
		}

		uint8_t k = 0;
		uint8_t count = active.bank->buttonCount;
		uint8_t mapcount = active.bank->buttons->buttonCallCount;

		/*
		 * extract blinking diodes from mapped remaps
		 */
		for (i = 0; i < count; i++)
		{
			for (j = 0; j < mapcount; j++)
			{
				if (active.bank->buttons[i].calls[j].callType == callType_remap)
				{
					remap =
							(logic_remap_t *) active.bank->buttons[i].calls[j].call;
					if (remap->ledBlinkColor != COL_NONE)
					{
						//pokud je fce aktivní tak to musi zahodit
						//	if (active)
						{
							pole[k].ledColor = remap->ledBlinkColor;
							pole[k].ledNumber = active.bank->buttons[i].number;
							k++;
						}
					}
				}
			}
		}

		yellow = foot_getLedsYellow();
		green = foot_getLedsGreen();

		//toggle all user defined extracted leds
		while (k)
		{
			if (pole[k - 1].ledColor == COL_GREEN)
				green ^= (1 << pole[k - 1].ledNumber);
			else if (pole[k - 1].ledColor == COL_YELLOW)
				yellow ^= (1 << pole[k - 1].ledNumber);
			else //both colors
			{
				yellow ^= (1 << pole[k - 1].ledNumber);
				green ^= (1 << pole[k - 1].ledNumber);
			}

			k--;
		}

		foot_SetLedsBoth(yellow, green);

		chThdSleepMilliseconds(500);
	}
}
