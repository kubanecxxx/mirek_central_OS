/**
 * @file logic_use.c
 * @author kubanec
 * @date 23.8.2012
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "logic_types.h"
#include "switch_master.h"
#include "i2c_user.h"
#include "rs232.h"
#include "logic_use.h"
#include "logic_flash.h"
#include "string.h"

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
WORKING_AREA(wa_scan, 512);
static logic_base_t * base = (logic_base_t *) FLASH_BASE_ADDRESS;
static logic_active_t active;

/* Private function prototypes -----------------------------------------------*/
static void logic_specific(const logic_specific_t * arg);
static void logic_blinkingThread(void * data);
static void logic_scanThread(void * data);
static void logic_marshallSetup(const logic_marshall_t * marsh);
static void logic_button(const logic_bank_t * bank, const foot_t * button);
static void logic_channel(const logic_channel_t * arg,
		const logic_button_t * but);

/* Private functions ---------------------------------------------------------*/
void logic_init(void)
{
	/*
	 * start shell for RS232 - communication with marshall
	 */
	serial_init();

	/**
	 * @brief init gpio for relay switching and optocouplers watching
	 */
	switch_masterGpioInit();

	/**
	 * @brief init I2C1, make i2c1 thread
	 */
	i2c1_init();

	if ((base->bankCount == 0)
			&& (base->banks != (logic_bank_t *) FLASH_BANK_ADDRESS))
	{
		return;

	}

	active.bank = base->banks;

	//nastavit první kanál
	if (active.bank->channels != NULL )
		logic_channel(active.bank->channels, NULL );
	//inicializovat aktivni
	//počáteční init efektů

	chThdCreateStatic(&wa_scan, sizeof(wa_scan), NORMALPRIO,
			(tfunc_t) (logic_scanThread), NULL );

//	chThdCreateStatic(&wa_blinking, sizeof(wa_blinking), NORMALPRIO,
//			(tfunc_t) logic_blinkingThread, NULL );
}

static void logic_scanThread(void * data)
{
	(void) data;

	EventListener el;
	chEvtRegister(&event_i2c_buttons, &el, BUTTON_EVENT_ID);

	while (TRUE)
	{
		chEvtWaitAny(BUTTON_EVENT_ID);
		logic_button(active.bank, &foot_switch);
	}
}

static void logic_functionLeds(const logic_function_t * arg, uint16_t pin)
{
	logic_ledColor_t coun = arg->led;
	uint16_t green = 0;
	uint16_t yellow = 0;
	if (coun != COL_NONE)
	{
		uint8_t yellow_i = foot_getLedsYellow();
		uint8_t green_i = foot_getLedsGreen();
		if (coun == COL_GREEN)
			green = pin;
		else if (coun == COL_YELLOW)
			yellow = pin;
		else if (coun == COL_BOTH)
		{
			yellow = pin;
			green = pin;
		}

		bool_t watcha = FALSE;
		if (arg->watchEffect < RELAY_COUNT)
		{
			watcha = switch_getRelay(arg->watchEffect);
		}
		else if (arg->watchEffect == 28)
		{
			uint8_t temp = harm_getInputs();
			watcha = harm_getInput_LED(temp);
		}
		else if (arg->watchEffect == 29)
		{
			watcha = delay_get();
		}
		else if (arg->watchEffect == 30 || arg->watchEffect == 31)
		{
			watcha = opto_getEffect(arg->watchEffect);
		}

		if (watcha == TRUE)
		{
			foot_SetLedsBoth(yellow_i | yellow, green_i | green);
		}
		else
		{
			foot_SetLedsBoth(yellow_i & (~yellow), green_i & (~green));
		}
	}

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
 */
static void logic_function(const logic_function_t * arg,
		const logic_button_t * but)
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

	//active.activeFunctions[active.count++] = arg;

	logic_marshallSetup(&arg->marshall);

	//leds
	logic_functionLeds(arg, but->button.pin);
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
static void logic_channel(const logic_channel_t * arg,
		const logic_button_t * but)
{
	uint32_t temp = arg->effects.w;
	//I2C bits 28 - harmonist ,29 - delay
	logic_bit_t i2c_temp = arg->effects;
	//opto bits 30,31
	uint8_t temp_opto = (temp >> 30) & 0b11;
	temp &= 0x0FFFFFFF;

	uint16_t pin = but->button.pin;
	uint16_t coun = but->button.count;
	uint16_t green = 0;
	uint16_t yellow = 0;

	//leds
	if (coun == COL_YELLOW)
		yellow = pin;
	else if (coun == COL_GREEN)
		green = pin;
	else if (coun == COL_BOTH)
	{
		yellow = pin;
		green = pin;
	}

	foot_SetLedsBoth(yellow, green);

	//setup delay and harmonist
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

	logic_marshallSetup(&arg->marshall);
}

/**
 * @brief nastavení maršála po complotě
 */
static void logic_marshallSetup(const logic_marshall_t * marsh)
{
	//od 1-4 jinak se nic nestane
	serial_gain(marsh->gain);
	serial_volume(marsh->volume);

	if (marsh->effLoop != EFF_NOTHING)
	{
		if (marsh->effLoop == EFF_ENABLE)
			serial_loopOn();
		else
			serial_loopBypass();
	}

	if (marsh->high != EFF_NOTHING)
	{
		if (marsh->high == EFF_ENABLE)
			serial_channelHigh();
		else
			serial_channelLow();
	}

	if (marsh->mute != EFF_NOTHING)
	{
		if (marsh->mute == EFF_ENABLE)
			serial_mute();
		else
			serial_unmute();
	}
}

/**
 * @brief přemapuje jedno volání
 */
static void logic_remap(const logic_bank_t * bank, const logic_remap_t * remap,
		bool_t zpatky)
{
	logic_button_t * but = NULL;
	logic_buttonCall_t * temp;

	if (zpatky == TRUE && remap->save == TRUE)
		return;

	//najít *but podle jména
	uint8_t j;
	for (j = 0; j < bank->buttonCount; j++)
	{
		if (!strcmp(bank->buttons[j].name, remap->ButtonName))
		{
			but = &bank->buttons[j];
			break;
		}
	}

	//tlačítko není
	if (but == NULL )
		return;

	if (*(but->ramCalls) == NULL )
	{
		temp = (logic_buttonCall_t *) chCoreAlloc(
				but->buttonCallCount * sizeof(logic_buttonCall_t));
		*(but->ramCalls) = temp;

		//naládovat to tam z flašky
		uint8_t i;
		uint8_t cant = but->buttonCallCount;
		for (i = 0; i < cant; i++)
		{
			temp[i].CallName = but->calls[i].CallName;
			temp[i].call = but->calls[i].call;
			temp[i].callType = but->calls[i].callType;
		}
	}
	else
	{
		temp = *(but->ramCalls);
	}

	uint8_t idx;
	uint8_t i;
	bool_t back;

	for (i = 0; i < but->buttonCallCount; i++)
	{
		if (!strcmp(temp[i].CallName, remap->newCall.CallName))
		{
			idx = i;
			back = TRUE;
			break;
		}
		else if (!strcmp(temp[i].CallName, remap->oldCall.CallName))
		{
			idx = i;
			back = FALSE;
			break;
		}
	}

	if (zpatky == TRUE)
		back = TRUE;

	const logic_buttonCall_t * call;
	if (back == FALSE)
		call = &remap->newCall;
	else
		call = &remap->oldCall;

	temp[idx].call = call->call;
	temp[idx].CallName = call->CallName;
	temp[idx].callType = call->callType;
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
 * @todo todo domyslet přemapování zpátky při změně kanálu
 *
 * @ingroup LOGIC_HL
 */
static void logic_button(const logic_bank_t * bank, const foot_t * button)
{
//tohle bude volat funkce above
//volání bude probihat někde v samostatnym vlákně
	static logic_remap_t * lastRemap;
	logic_button_t * but;
	uint8_t prevChannel = active.activeChannel;

	active.count = 0;

	uint8_t i;

//prvni musi najit button podle čisla a počtu zmačknuti
	for (i = 0; i < bank->buttonCount; i++)
	{
		but = &bank->buttons[i];

		if (but->button.pin == button->pin
				&& but->button.count == button->count)
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
		/*vyhrabat to prvni z ramky kvuli přemapování*/
		logic_buttonCall_t * tmp = *(but->ramCalls);
		if (tmp == NULL )
			call = &(but->calls[i]);
		else
			call = tmp;

		if (call->callType == callType_channel)
		{
			logic_remap(bank, lastRemap, TRUE);
			channel = (logic_channel_t *) call->call;
			logic_channel(channel, but);
			active.activeChannel = channel->index;
			//prohledat všecky funkce + tlačitka a nastavit podle toho ledky z funkcí
			uint8_t j, k, l;
			logic_function_t * _func;
			logic_button_t * _but;
			logic_buttonCall_t * _call;
			for (j = 0; j < bank->functionCount; j++)
			{
				_func = &bank->functions[j];

				if (_func->led != COL_NONE
						&& (_func->channelCondition == channel->index
								|| _func->channelCondition == 0))
				{
					for (k = 0; k < bank->buttonCount; k++)
					{
						_but = &bank->buttons[k];
						for (l = 0; l < _but->buttonCallCount; l++)
						{
							_call = &_but->calls[l];
							if (!strcmp(_call->CallName, _func->name))
							{
								logic_functionLeds(_func, _but->button.pin);
							}
						}
					}
				}
			}
		}
		else if (call->callType == callType_function)
		{
			func = (logic_function_t *) call->call;
			if (func->channelCondition == prevChannel
					|| func->channelCondition == 0)
				logic_function(func, but);
		}
		else if (call->callType == callType_remap)
		{
			remap = (logic_remap_t *) call->call;
			if (remap->channelCondition == prevChannel
					|| remap->channelCondition == 0)
			{
				logic_remap(bank, remap, FALSE);
				lastRemap = remap;
			}

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
						//prohledat active jesli náhodou se ointery nerovnaji
						//	if (active)
						{
							pole[k].ledColor = remap->ledBlinkColor;
							//	pole[k].ledNumber = active.bank->buttons[i].number;
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
