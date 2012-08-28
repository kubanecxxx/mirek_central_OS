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
static void logic_button(const logic_bank_t * bank, const foot_t * button,
		eventmask_t mask);
static void logic_channel(const logic_channel_t * arg,
		const logic_button_t * but);

Thread * thd_logic_scan = NULL;
Thread * thd_logic_blinking = NULL;

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

	if (base->banks != (logic_bank_t *) FLASH_BANK_ADDRESS)
	{
		return;
	}

	active.bank = base->banks;

	//nastavit první kanál
	if (active.bank->channels != NULL )
		logic_channel(active.bank->channels, NULL );
	//inicializovat aktivni
	//počáteční init efektů

	thd_logic_scan = chThdCreateStatic(&wa_scan, sizeof(wa_scan), NORMALPRIO,
			(tfunc_t) (logic_scanThread), NULL );

	thd_logic_blinking = chThdCreateStatic(&wa_blinking, sizeof(wa_blinking),
			NORMALPRIO - 1, (tfunc_t) logic_blinkingThread, NULL );
}

static void logic_scanThread(void * data)
{
	(void) data;

	EventListener el;
	chEvtRegister(&event_i2c_buttons, &el,
			BUTTON_EVENT_ID | BUTTON_NOW_EVENT_ID);

	eventmask_t mask;

	while (TRUE)
	{
		mask = chEvtWaitAny(BUTTON_NOW_EVENT_ID | BUTTON_EVENT_ID);
		logic_button(active.bank, &foot_switch, mask);
	}
}

static bool_t logic_functionLeds(const logic_function_t * arg, uint16_t pin)
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
			return TRUE;
		}
	}

	return FALSE;
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

	logic_marshallSetup(&arg->marshall);
}

static void logic_channelLeds(uint8_t pin, logic_ledColor_t coun)
{
	/*
	 * pokud COL_NONE tak tam hodi minuly nastaveni ledek
	 */
	static uint16_t green = 0;
	static uint16_t yellow = 0;

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

	logic_channelLeds(but->button.pin, but->button.count);

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

	logic_channelLeds(but->button.pin, but->button.count);
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
 *
 * @ingroup LOGIC_HL
 */
static void logic_button(const logic_bank_t * bank, const foot_t * button,
		eventmask_t mask)
{
	static logic_remap_t * lastRemap[10];
	static uint8_t remap_count = 0;
	logic_button_t * but;
	uint8_t prevChannel = active.activeChannel;
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

	if (((but->bit.now == FALSE) && (mask & BUTTON_NOW_EVENT_ID))
			|| ((but->bit.now == TRUE)&& (mask &BUTTON_EVENT_ID)))
			return;

	logic_buttonCall_t * call;
	logic_channel_t * channel;
	logic_function_t * func;
	logic_remap_t * remap;
	bool_t jednou = FALSE;

	for (i = 0; i < but->buttonCallCount; i++)
	{
		/*vyhrabat to prvni z ramky kvuli přemapování*/
		logic_buttonCall_t * tmp = *(but->ramCalls);
		if (tmp == NULL )
			call = &but->calls[i];
		else
			call = &tmp[i];

		if (call->callType == callType_channel)
		{
			while (remap_count)
			{
				remap_count--;
				logic_remap(bank, lastRemap[remap_count], TRUE);
			}
			channel = (logic_channel_t *) call->call;
			logic_channel(channel, but);
			active.activeChannel = channel->index;

		}
		else if (call->callType == callType_function)
		{
			func = (logic_function_t *) call->call;
			if (func->channelCondition == prevChannel
					|| func->channelCondition == 0)
			{
				logic_function(func, but);
			}
		}
		else if (call->callType == callType_remap)
		{
			remap = (logic_remap_t *) call->call;
			if (remap->channelCondition == prevChannel
					|| remap->channelCondition == 0)
			{
				if (jednou == FALSE)
				{
					jednou = TRUE;
					remap_count = 0;
				}
				logic_remap(bank, remap, FALSE);
				lastRemap[remap_count++] = remap;
				logic_channelLeds(0, COL_NONE);
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

	/**
	 * @detials
	 * vytahnout funkce z aktivního kanálu
	 * podivat se jak vypadaji jejich sledovany efekty
	 * pokud jsou sledovany efekty nahozeny a funkce sou mapovany tak rožne ledky
	 * zároveň sledovat jesli je funkce mapovaná nebo ne jesli je mapovaná a a efekt
	 * je zapnuté tak svitit, jesli  neni zapnuté tak blikat
	 * zhášení efektů se provede přímo v remapu -
	 * pokud se přemapovává tak zhasne uplně všechno kromě
	 * aktivniho kanálu
	 */

	while (TRUE)
	{
		while (active.bank == NULL )
		{
			chThdSleepMilliseconds(1000);
		}

		uint8_t i, j;
		uint8_t k = 0;

		uint8_t yellow = 0, green = 0;

		for (i = 0; i < active.bank->functionCount; i++)
		{
			//všechny funkce se splněnym kanálem
			if (active.bank->functions[i].channelCondition
					== active.activeChannel
					|| active.bank->functions[i].channelCondition == 0)
			{
				logic_function_t * _func = &active.bank->functions[i];
				/*
				 * zjistit na jaky sou namapovany tlačitka a podle tlačitek pohlidat ledky
				 * ty ktery nejsou nikde mapovany nedělat nic, ty ktery sou mapovany a
				 * sledovany efekty sou zapnuty tak rožnout a ktery sou mapovany a sledovany
				 * efekty nejsou zapnuty tak rozblikat
				 */
				/*
				 * -? (projet všechny tlačitka a rovnou vybirat funkce)
				 * - nebo projiždět funkce a k nim projet tlačitka se všema volánima
				 */
				for (j = 0; j < active.bank->buttonCount; j++)
				{
					logic_button_t * but = &active.bank->buttons[j];

					/*
					 * rozhodnout jesli je to z ramky nebo flašky
					 */
					logic_buttonCall_t * calls = *but->ramCalls;
					if (calls == NULL )
					{
						/*
						 * flaška
						 */
						calls = but->calls;
					}
					for (k = 0; k < but->buttonCallCount; k++)
					{
						/*
						 * všechny mapovany funkce v kanálu
						 * potřebuju i nemapovany abych je mohl zhasnout - kde vzít
						 * asi zhasnout eště před přemapem
						 * tohle co tady zatim je jenom ledku rožne nebo rozbliká ale
						 * neumi ju to jenom zhasnout protože jak neni mapovaná tak nikdo
						 * nevi kde je takže to nějak řešit přimo ve funkci remap posilat
						 * data sem
						 * kdepak zháši se to rovnou v remapu
						 */
						if (_func == calls->call)
						{
							/*
							 * podivat se jesli sou sledovany efekty aktivní a rožnout ledky nebo je rozblikat
							 */
							uint8_t num = but->button.pin;
							if (logic_functionLeds(_func, num))
							{
								/*
								 * ledkou blikat
								 */
								if (_func->led == COL_GREEN)
								{
									green |= num;
								}
								else if (_func->led == COL_YELLOW)
								{
									yellow |= num;
								}
								else //both
								{
									green |= num;
									yellow |= num;
								}
								/*
								 * vygenerovat bitovou masku kterou pak blikat...
								 */
							}
						}
						calls++;
					}
				}
			}
		}

		uint8_t yellow_i = foot_getLedsYellow();
		uint8_t green_i = foot_getLedsGreen();

		static bool_t oj;
		if (oj)
		{
			yellow_i &= ~yellow;
			green_i &= ~green;
		}
		else
		{
			yellow_i |= yellow;
			green_i |= green;
		}
		oj = !oj;

		foot_SetLedsBoth(yellow_i, green_i);

		if (chThdShouldTerminate())
			break;
		chThdSleepMilliseconds(200);
	}
}
