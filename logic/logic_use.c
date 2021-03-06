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
#include "gui_MainScreen.h"
#include "wah.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static WORKING_AREA(wa_blinking, 256);
static WORKING_AREA(wa_scan, 512);
static WORKING_AREA(wa_specific,256);
const logic_base_t * base = (logic_base_t *) FLASH_BASE_ADDRESS;
logic_active_t active;
static logic_specific_t _specific;

/* Private function prototypes -----------------------------------------------*/
static void logic_blinkingThread(void * data);
static void logic_scanThread(void * data);
static void logic_specificThread(void *data);
static void logic_button(const logic_bank_t * bank, const foot_t * button,
		eventmask_t mask);
static void logic_channel(const logic_channel_t * arg,
		const logic_button_t * but);
static void logic_marshallMute(void);
static void logic_marshallUnmute(void);
static int mutePico = 0;

static struct
{
	const logic_function_t * arg[20];
	uint8_t count;
} active_functions;

Thread * thd_logic_scan = NULL;
Thread * thd_logic_blinking = NULL;
Thread * thd_logic_specific = NULL;

//#define MUTEEEE

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

	thd_logic_specific = chThdCreateStatic(&wa_specific, sizeof(wa_specific),
			NORMALPRIO, (tfunc_t) logic_specificThread, NULL);

	active.bank = base->banks;

	//nastavit první kanál
	if (active.bank->channels != NULL)
		logic_channel(active.bank->channels, NULL);
	//inicializovat aktivni
	//počáteční init efektů

	thd_logic_scan = chThdCreateStatic(&wa_scan, sizeof(wa_scan), NORMALPRIO,
			(tfunc_t) (logic_scanThread), NULL);

	thd_logic_blinking = chThdCreateStatic(&wa_blinking, sizeof(wa_blinking),
			NORMALPRIO - 1, (tfunc_t) logic_blinkingThread, NULL);
}

static void logic_scanThread(void * data)
{
	(void) data;

	chRegSetThreadName("logic scan");
	EventListener el;
	chEvtRegister(&event_i2c_buttons, &el,
			BUTTON_EVENT_ID | BUTTON_NOW_EVENT_ID);

	eventmask_t mask;
	static uint32_t recevie = 0;

	while (TRUE)
	{
		mask = chEvtWaitOne(BUTTON_NOW_EVENT_ID | BUTTON_EVENT_ID);
		recevie++;
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
		else if (arg->watchEffect == 12)
		{
			uint8_t temp = harm_getInputs();
			watcha = harm_getInput_LED(temp);
		}
		else if (arg->watchEffect == 13)
		{
			watcha = delay_get();
		}
		else if (arg->watchEffect == 14 || arg->watchEffect == 15)
		{
			watcha = opto_getEffect(arg->watchEffect);
		}

		if (watcha == TRUE)
		{
			foot_SetLedsBoth(yellow_i | yellow, green_i | green);
			/*
			 * projit pole a pokud už tam je tak ho nepřidávat
			 */

			if (arg->overloadEff || arg->overloadVolume)
			{
				int i;
				bool_t uz = FALSE;
				for (i = 0; i < active_functions.count; i++)
				{
					if (active_functions.arg[i] == arg)
					{
						uz = TRUE;
						break;
					}
				}

				if (uz == FALSE)
					active_functions.arg[active_functions.count++] = arg;
			}
		}
		else
		{
			foot_SetLedsBoth(yellow_i & (~yellow), green_i & (~green));
			/*
			 * projit pole a pokud tam je tak ho vyhodit a pole posunout
			 * pokud tam neni tak nedělat nic
			 */
			int i, j;
			for (i = 0; i < active_functions.count; i++)
			{
				if (active_functions.arg[i] == arg)
				{
					//vyhodit ho
					for (j = i; j < active_functions.count; j++)
					{
						active_functions.arg[j] = active_functions.arg[j + 1];
					}
					active_functions.count--;
					break;
				}
			}

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
	(void) but;
	//I2C effects 28 - harmonist ,29 - delay
	uint64_t temp = arg->effects.w;
	logic_dibit_t temp_i2c = arg->effects;

	logic_specific(arg->special);

	uint32_t old_state_eff = switch_getRelays();
	old_state_eff |= opto_getEffects() << 20;
	old_state_eff |= delay_get() << 30;
	uint8_t in = harm_getInputs();
	old_state_eff |= harm_getInput_LED(in) << 31;

	//relays
	uint8_t i;
	logic_effect_t tmp;
	for (i = 0; i < 12; i++)
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

	//overdrive
	if (temp_i2c.s.eff14 == EFF_ENABLE)
		opto_enableEffect(0);
	else if (temp_i2c.s.eff14 == EFF_DISABLE)
		opto_disableEffect(0);
	else if (temp_i2c.s.eff14 == EFF_TOGGLE)
		opto_toggleEffect(0);

	//tuner
	if (temp_i2c.s.eff15 == EFF_ENABLE)
		opto_enableEffect(1);
	else if (temp_i2c.s.eff15 == EFF_DISABLE)
		opto_disableEffect(1);
	else if (temp_i2c.s.eff15 == EFF_TOGGLE)
		opto_toggleEffect(1);

	//harmonist
	if (temp_i2c.s.eff12 == EFF_ENABLE)
		harm_enable();
	else if (temp_i2c.s.eff12 == EFF_DISABLE)
		harm_disable();
	else if (temp_i2c.s.eff12 == EFF_TOGGLE)
		harm_toggle();

	//delay
	if (temp_i2c.s.eff13 == EFF_ENABLE)
	{
		delay_on();
	}
	else if (temp_i2c.s.eff13 == EFF_DISABLE)
	{
		delay_off();
	}
	else if (temp_i2c.s.eff13 == EFF_TOGGLE)
	{
		delay_toggle();
	}

	uint32_t new_state_eff = switch_getRelays();
	new_state_eff |= opto_getEffects() << 20;
	new_state_eff |= delay_get() << 30;
	in = harm_getInputs();
	new_state_eff |= harm_getInput_LED(in) << 31;

	uint32_t diff = new_state_eff ^ old_state_eff;

	const logic_marshall_t * marsh;
	if (active.bank->channels[active.activeChannel - 1].VolumeOverloadEnabled
			&& new_state_eff & diff)
		marsh = &arg->marshall;
	else
		marsh = &active.bank->channels[active.activeChannel - 1].marshall;

	logic_marshallSetup(marsh);
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
	{
		green = 0;
		yellow = pin;
	}
	else if (coun == COL_GREEN)
	{
		yellow = 0;
		green = pin;
	}
	else if (coun == COL_BOTH)
	{
		yellow = pin;
		green = pin;
	}

	foot_SetLedsBoth(yellow, green)
	;
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
	temp &= 0x0FFF;
	const logic_marshall_t * marsh;
	logic_marshall_t marsh_temp;

	/*
	 * projit všechny funkce ješli některá nemá overload
	 * pokud má overloadvolume tak volat změněné marshall o to co je overloadnuty
	 *
	 * pokud je overload na effekt tak změnit nastavení efektů - kanál muže jenom zapinat
	 * ne vypinat
	 *
	 */

	marsh = &arg->marshall;

	int j;
	for (j = 0; j < active_functions.count; j++)
	{
		if (active_functions.arg[j]->overloadVolume
				&& arg->VolumeOverloadEnabled)
		{
			//memcpy (&marsh_temp,&active_functions.arg[j]->marshall, sizeof(logic_marshall_t));

			if (marsh_temp.gain == 0)
				marsh_temp.gain = marsh->gain;

			if (marsh_temp.volume == 0)
				marsh_temp.volume = marsh->volume;

			marsh_temp.high = arg->marshall.high;

			marsh = &marsh_temp;
		}

		if (active_functions.arg[j]->overloadEff)
		{
			logic_dibit_t eff = active_functions.arg[j]->effects;

			int i;
			for (i = 0; i < 12; i++)
			{
				if (((eff.w & 0b11) == EFF_ENABLE) || ((eff.w & 0b11)
						== EFF_TOGGLE))
				{
					temp |= (1 << i);
				}
				eff.w >>= 2;
			}

			//overdrive
			if (eff.s.eff14 == EFF_ENABLE || eff.s.eff14 == EFF_TOGGLE)
				i2c_temp.s.bit14 = TRUE;

			//skuner
			if (eff.s.eff15 == EFF_ENABLE || eff.s.eff15 == EFF_TOGGLE)
				i2c_temp.s.bit15 = TRUE;

			//harmonist
			if (eff.s.eff12 == EFF_ENABLE || eff.s.eff12 == EFF_TOGGLE)
				i2c_temp.s.bit12 = TRUE;

			//delay
			if (eff.s.eff13 == EFF_ENABLE || eff.s.eff13 == EFF_TOGGLE)
				i2c_temp.s.bit13 = TRUE;
		}
	}

	//optocouplers
	if (i2c_temp.s.bit14)
		opto_enableEffect(0);
	else
		opto_disableEffect(0);

	if (i2c_temp.s.bit15)
		opto_enableEffect(1);
	else
		opto_disableEffect(1);

	if (!wah_isEnabled())
	{
		logic_marshallSetup(marsh);
	}
	else
	{
		wah_event();
	}

	switch_setRelays(temp);
	//harmonist
	if (i2c_temp.s.bit12)
		harm_enable();
	else
		harm_disable();

	//delay
	if (i2c_temp.s.bit13)
		delay_on();
	else
		delay_off();

	//setup delay and harmonist
	logic_specific(arg->special);

	if (but != NULL)
		logic_channelLeds(but->button.pin, but->button.count);

	if (but != NULL)
	{
		if (arg->led == COL_NONE)
			logic_channelLeds(but->button.pin, but->button.count);
		else
			logic_channelLeds(but->button.pin, arg->led);
	}

	gui_putChannel(arg);
}
MUTEX_DECL(marshall_mutex);

static void logic_marshallMute(void)
{
	chMtxLock(&marshall_mutex);
	serial_mute();
	chMtxUnlock();
}

static void logic_marshallUnmute(void)
{
	chMtxLock(&marshall_mutex);
	serial_unmute();
	chMtxUnlock();
}

/**
 * @brief nastavení maršála po complotě
 */
void logic_marshallSetup(const logic_marshall_t * marsh)
{

	chMtxLock(&marshall_mutex);

	//if (marsh->high == EFF_DISABLE)
	//	serial_channelLowSrat();

	if (marsh->high != EFF_NOTHING)
	{
		if (marsh->high == EFF_ENABLE)
		{
			serial_channelHigh();
		}
		else
		{
			serial_channelLow();
		}
	}

	if (marsh->effLoop != EFF_NOTHING)
	{
		if (marsh->effLoop == EFF_ENABLE)
		{
			serial_loopOn();

		}
		else
		{
			serial_loopBypass();
		}
	}

	if (marsh->mute != EFF_NOTHING)
	{
		if (marsh->mute == EFF_ENABLE)
		{
			serial_mute();
		}
		else
		{
			serial_unmute();
		}
	}

	gui_putMarshall(marsh);

	//od 1-4 jinak se nic nestane
	serial_gain(marsh->gain);
	serial_volume(marsh->volume);
	chMtxUnlock();
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
	if (but == NULL)
		return;

	if (*(but->ramCalls) == NULL)
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
void logic_specific(const logic_specific_t * arg)
{
	if (arg == NULL || thd_logic_specific == NULL)
		return;

	if (!(arg->delay.time == 65535 || arg->delay.time == 65535))
		_specific.delay = arg->delay;

	if (!(arg->harmonist.harmony == 255 || arg->harmonist.key == 255
			|| arg->harmonist.mode == 255 || arg->harmonist.volume == 65535))
		_specific.harmonist = arg->harmonist;

	//_specific = *arg;
	chEvtSignalFlags(thd_logic_specific, 1 << 24);
	gui_putSpecial(arg);
}

/**
 * @brief zavolá všechny volání od tlačitka
 *
 * @ingroup LOGIC_HL
 */
static void logic_button(const logic_bank_t * bank, const foot_t * button,
		eventmask_t mask)
{
	static logic_remap_t * lastRemap[10];
	static uint8_t remap_count = 0;
	static bool_t retreat = FALSE;
	static logic_function_t * last_func = 0;
	static logic_button_t * lastBut = NULL;
	logic_button_t * but;
	uint8_t prevChannel = active.activeChannel;
	uint8_t i;

	//prvni musi najit button podle čisla a počtu zmačknuti
	for (i = 0; i < bank->buttonCount; i++)
	{
		but = &bank->buttons[i];

		if (but->button.pin == button->pin && but->button.count
				== button->count)
		{
			break;
		}
		but = NULL;
	}

	//žádnej neni mapovanej
	if (but == NULL)
		return;

	if (((but->bit.now == FALSE) && (mask & BUTTON_NOW_EVENT_ID))
			|| ((but->bit.now == TRUE) && (mask & BUTTON_EVENT_ID)))
		return;

	logic_buttonCall_t * call;
	logic_channel_t * channel;
	logic_function_t * func;
	logic_remap_t * remap;
	bool_t jednou = FALSE;
	static systime_t time;
	time = chTimeNow();

	//logic_marshallMute();

	//chThdSleepMilliseconds(30);

	for (i = 0; i < but->buttonCallCount; i++)
	{
		/*vyhrabat to prvni z ramky kvuli přemapování*/
		logic_buttonCall_t * tmp = *(but->ramCalls);
		if (tmp == NULL)
			call = &but->calls[i];
		else
			call = &tmp[i];

		if (call->callType == callType_channel)
		{
			//odmapuje na defaultni hodnotu minuly tlačitko s kanálem
			if (lastBut != but)
			{
				if (lastBut != NULL)
					*lastBut->ramCalls = NULL;
				lastBut = but;
			}

			while (remap_count)
			{
				remap_count--;
				logic_remap(bank, lastRemap[remap_count], TRUE);
			}
			channel = (logic_channel_t *) call->call;
			active.activeChannel = channel->index;
			active.activeChannelName = channel->name;
			logic_channel(channel, but);
			retreat = FALSE;
			last_func = NULL;

		}
		else if (call->callType == callType_function)
		{
			func = (logic_function_t *) call->call;
			if (func->channelCondition == prevChannel || func->channelCondition
					== 0)
			{
				if (retreat == TRUE && last_func == func)
				{
					//return back
					if (func->prevChannel != NULL)
					{
						active.activeChannel = channel->index;
						active.activeChannelName = channel->name;
						logic_channel(func->prevChannel, NULL);
					}
					last_func = 0;
					retreat = FALSE;
				}
				else
				{

					logic_function(func, but);
					if (func->retreat == TRUE)
					{
						retreat = TRUE;
						last_func = func;
					}
				}
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
				if (remap->newCall.callType != callType_channel)
					lastRemap[remap_count++] = remap;
				logic_channelLeds(0, COL_NONE);
			}
		}
	}
	time = chTimeNow() - time;

}

/**
 * @brief vykrade funkce ktery sou namapovany na tlačitka a rozbliká podle toho jejich ledky
 * @ingroup LOGIC_HL
 *
 */
static void logic_blinkingThread(void * data)
{
	(void) data;
	chRegSetThreadName("logic blink");
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
		while (active.bank == NULL)
		{
			chThdSleepMilliseconds(1000);
			if (chThdShouldTerminate())
				return;
		}

		if (chThdShouldTerminate())
			return;

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
					if (calls == NULL)
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
							if (logic_functionLeds(_func, num) && _func->blikat
									== TRUE)
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

		chThdSleepMilliseconds(200);
	}
}

static void logic_specificThread(void *data)
{
	(void) data;
	chRegSetThreadName("specific");
	logic_specific_t * arg = &_specific;

	while (TRUE)
	{
		chEvtWaitAny(1 << 24);

		delay_time(arg->delay.time);
		delay_volume(arg->delay.volume);

		harm_mode(arg->harmonist.mode);
		harm_key(arg->harmonist.key);
		harm_volume(arg->harmonist.volume);
		harm_harmony(arg->harmonist.harmony);
	}
}
