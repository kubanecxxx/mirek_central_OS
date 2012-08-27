/**
 * @file logic_fill.c
 * @author kubanec
 * @date 24.8.2012
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "ch.h"
#include "hal.h"
#include "shell.h"
#include "logic_types.h"
#include "chprintf.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "logic_flash.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
	uint8_t bankCounter;
	logic_bank_t bank[10];

	uint8_t channelCounter;
	logic_channel_t channel[10];

	uint8_t functionCounter;
	logic_function_t function[10];

	uint8_t remapCounter;
	logic_remap_t remap[10];

	uint8_t buttonCounter;
	logic_button_t button[10];

	uint8_t specialCounter;
	logic_specific_t special[10];

	uint8_t callCounter;
	logic_buttonCall_t call[30];
} fill_temp_static_t;

typedef struct
{
	logic_base_t base;
	logic_bank_t * bank;
	logic_channel_t * channel;
	logic_function_t * function;
	logic_remap_t * remap;
	logic_button_t * button;
} fill_temp_t;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define fill_ifOn(string) !strcmp(string,"on")
#define fill_ifOff(string) !strcmp(string,"off")
#define fill_ifToggle(string) !strcmp(string,"toggle")
#define fill_ifNone(string) !strcmp(string,"none")

/*
 * Heaps macro set
 */
/*
 #define OFFSET 10000
 #define HEAP_END (__heap_base__ + OFFSET)
 #define heap_channel_init() chHeapInit(&heap_channel,HEAP_END,OFFSET)
 #define heap_function_init() chHeapInit(&heap_function,HEAP_END + OFFSET,OFFSET)
 #define heap_bank_init() chHeapInit(&heap_bank, HEAP_END + 2*OFFSET, OFFSET)
 #define heap_special_init() chHeapInit(&heap_special, HEAP_END + 3*OFFSET, OFFSET)
 #define heap_button_init()  chHeapInit(&heap_button, HEAP_END + 4*OFFSET, OFFSET)
 #define heap_remap_init()  chHeapInit(&heap_remap, HEAP_END + 5*OFFSET, OFFSET)
 #define heap_calls_init()  chHeapInit(&heap_calls, HEAP_END + 6*OFFSET, OFFSET)

 #define alloc_channel() (logic_channel_t *)chHeapAlloc(&heap_channel,sizeof(logic_channel_t))
 #define alloc_bank() (logic_bank_t *)chHeapAlloc(&heap_bank, sizeof(logic_bank_t))
 #define alloc_special() (logic_specific_t *)chHeapAlloc(&heap_bank, sizeof(logic_specific_t))
 #define alloc_button() (logic_button_t *)chHeapAlloc(&heap_bank, sizeof(logic_button_t))
 #define alloc_remap() (logic_remap_t *)chHeapAlloc(&heap_bank, sizeof(logic_remap_t))
 #define alloc_function() (logic_function_t *)chHeapAlloc(&heap_function, sizeof(logic_function_t))
 #define alloc_call() (logic_buttonCall_t *)chHeapAlloc(&heap_calls, sizeof(logic_buttonCall_t))
 */
#define heap_neco_init(type) _fill_static->type##Counter = 0
#define heap_channel_init() heap_neco_init(channel)
#define heap_function_init() heap_neco_init(function)
#define heap_bank_init() heap_neco_init(bank)
#define heap_button_init() heap_neco_init(button)
#define heap_remap_init() heap_neco_init(remap)
#define heap_special_init() heap_neco_init(special)
#define heap_calls_init() heap_neco_init(call)

#define alloc_neco(type) &_fill_static->type[_fill_static->type##Counter++];
#define alloc_channel() alloc_neco(channel)
#define alloc_bank() alloc_neco(bank)
#define alloc_special() alloc_neco(special)
#define alloc_button() alloc_neco(button)
#define alloc_remap() alloc_neco(remap)
#define alloc_function() alloc_neco(function)
#define alloc_call() alloc_neco(call)

/*
 * helping macro for setting effects dibit
 */
#define _set_dibit(word, number, eff_state) \
word &= ~(0b11 << (2 * number)); \
word |= (eff_state << (2*number))
/* Private variables ---------------------------------------------------------*/
static fill_temp_t fill_actives;
static fill_temp_static_t * _fill_static;

/*
 static MemoryHeap heap_bank;
 static MemoryHeap heap_channel;
 static MemoryHeap heap_function;
 static MemoryHeap heap_special;
 static MemoryHeap heap_button;
 static MemoryHeap heap_remap;
 static MemoryHeap heap_calls;
 */

//extern uint8_t __heap_base__[];
/* Private function prototypes -----------------------------------------------*/
static void _cmd_special(BaseSequentialStream *chp, int argc, char *argv[],
		logic_specific_t * spec);
static void _effects_name(uint8_t * num, const char * arg, uint8_t mul);
static void _led(const char * retezec, logic_ledColor_t * led);
/* Private functions ---------------------------------------------------------*/

/*
 banky
 ------kanály
 ------------name
 ------------special
 ------funkce
 ------------name
 ------------special
 ------remapy
 ------------name
 ------------button
 ------tlačitka
 ------------name
 ------------calls
 -----------------call
 */

/**
 * @defgroup logic_shell
 * @ingroup Logika
 * @brief shell commandy pro naplnění všechmožnéch struktur
 */

/**
 * @ingroup logic_shell
 * @defgroup logic_bank
 */

/**
 * @defgroup logic_channel
 * @ingroup logic_bank
 * @defgroup logic_function
 * @ingroup logic_bank
 * @defgroup logic_button
 * @ingroup logic_bank
 * @defgroup logic_remap
 * @ingroup logic_bank
 */

/**
 * @brief nic
 * @ingroup logic_shell
 */

extern Thread * thd_logic_scan;
extern Thread * thd_logic_blinking;

void cmd_openLogic(BaseSequentialStream *chp, int argc, char *argv[])
{
	(void) chp;
	(void) argc;
	(void) argv;

	//zastavit potřebny vlákna
	if (thd_logic_blinking != NULL )
	{
		chThdTerminate(thd_logic_blinking);
		while (!chThdTerminated(thd_logic_blinking))
			;
	}
	/*if (thd_logic_scan)
	 {
	 chThdTerminate(thd_logic_scan);
	 while (!chThdTerminated(thd_logic_scan))
	 ;
	 }*/

	uint32_t temp;
//smazat flašku a připravit pro zápis
	logic_flashErase(FLASH_ADDRESS_START, FLASH_ADDRESS_STOP );

	temp = chCoreStatus();
	_fill_static = chCoreAlloc(sizeof(fill_temp_static_t));
	temp = chCoreStatus();

//vymazat cache
	fill_actives.base.bankCount = 0;
	fill_actives.base.banks = NULL;
	fill_actives.bank = NULL;
	fill_actives.button = NULL;
	fill_actives.channel = NULL;
	fill_actives.function = NULL;
	fill_actives.remap = NULL;

	heap_bank_init();
}

/**
 * @ingroup logic_shell
 */
void cmd_closeLogic(BaseSequentialStream *chp, int argc, char *argv[])
{
	(void) argc;
	(void) argv;

//uložit posledni banku
	logic_flashWriteBank(fill_actives.bank);

//uložit sadu bank na předem známou adresu
	logic_flashWriteBase(&fill_actives.base);

//todo uvolnit paměť

	if (chp != NULL )
		chprintf(chp, "Okej");

}

/**
 * @ingroup logic_bank
 */
void cmd_bankAdd(BaseSequentialStream *chp, int argc, char *argv[])
{
	logic_bank_t * temp;
	if (argc == 1)
	{
		//pokud je to další banka tak uložit do flašky a uvolnit heapy

		if (fill_actives.base.banks != NULL )
		{
			//tady uložit do flaš aktivni banku + celej bordel k ni
			logic_flashWriteBank(fill_actives.bank);
		}

		temp = alloc_bank();
		fill_actives.base.bankCount++;

		fill_actives.bank = temp;

		temp->buttonCount = 0;
		temp->buttons = NULL;
		temp->channelCount = 0;
		temp->channels = NULL;
		temp->functionCount = 0;
		temp->functions = NULL;
		temp->name = NULL;
		temp->remapCount = 0;
		temp->remaps = NULL;

		heap_channel_init();
		heap_function_init();
		heap_special_init();
		heap_button_init();
		heap_remap_init();
		heap_calls_init();

		if (fill_actives.base.banks == NULL )
		{
			fill_actives.base.banks = temp;
		}

		//uložit jméno do flašky
		char * name = logic_flashWriteName(argv[0]);
		fill_actives.bank->name = name;

	}
	else
	{
		chprintf(chp, "banka nic");
	}
}

/*--------------------------------------------------------------------*
 * channel commands
 *--------------------------------------------------------------------*/

/**
 * @ingroup logic_channel
 */
void cmd_channelAdd(BaseSequentialStream *chp, int argc, char *argv[])
{
	logic_channel_t * temp;

	if (argc == 1)
	{
		//inicializovat všechno na nuly a tak
		temp = alloc_channel();

		fill_actives.channel = temp;
		temp->index = ++fill_actives.bank->channelCount;

		temp->effects.w = 0;
		temp->marshall.effLoop = EFF_ENABLE;
		temp->marshall.gain = 1;
		temp->marshall.volume = 1;
		temp->marshall.mute = EFF_DISABLE;
		temp->marshall.high = EFF_ENABLE;
		temp->name = NULL;
		temp->special = NULL;

		if (fill_actives.bank->channels == NULL )
		fill_actives.bank->channels = fill_actives.channel;

		/*uložit jméno do flašky*/
		fill_actives.channel->name = logic_flashWriteName(argv[0]);

	}
	else
	{
		chprintf(chp, "kanal nic");
	}
}

		/**
		 * @ingroup logic_channel
		 */
void cmd_channel_marshall(BaseSequentialStream *chp, int argc, char *argv[])
{
	uint8_t num = atoi(argv[1]);
	logic_marshall_t * marsh = &fill_actives.channel->marshall;
	if (argc == 2 && num < 5)
	{
		if (!strcmp(argv[0], "preamp"))
		{
			marsh->gain = num;
		}
		else if (!strcmp(argv[0], "volume"))
		{
			marsh->volume = num;
		}
		else if (!strcmp(argv[0], "mute"))
		{
			marsh->mute = fill_ifOn(argv[1]) ? EFF_ENABLE : EFF_DISABLE;
		}
		else if (!strcmp(argv[0], "sens"))
		{
			marsh->mute = !strcmp("high", argv[1]) ? EFF_ENABLE : EFF_DISABLE;
		}
		else if (!strcmp(argv[0], "smycka"))
		{
			marsh->effLoop = fill_ifOn(argv[1]) ? EFF_ENABLE : EFF_DISABLE;
		}
		else
		{
			chprintf(chp, "marshal kanal nic");
		}
	}
	else
	{
		chprintf(chp, "marshal kanal nic");
	}
}

/**
 * @ingroup logic_channel
 */
void cmd_channel_effs(BaseSequentialStream *chp, int argc, char *argv[])
{
	//přidat pak pomocny jména přimo efektů
	if (argc == 2)
	{
		uint8_t num = atoi(argv[0]);
		_effects_name(&num, argv[0], 1);

		if (fill_ifOn(argv[1]))
			fill_actives.channel->effects.w |= 1 << num;
		else
			fill_actives.channel->effects.w &= ~(1 << num);
	}
	else
	{
		chprintf(chp, "kanal efekty nic");
	}
}

/**
 * @ingroup logic_channel
 */
static void _effects_name(uint8_t * num, const char * arg, uint8_t mul)
{
	if (!strcmp(arg, "superdrive_true"))
		*num = 0;
	else if (!strcmp(arg, "wah"))
		*num = 1 * mul;
	else if (!strcmp(arg, "compresor"))
		*num = 2 * mul;
	else if (!strcmp(arg, "phaser"))
		*num = 3 * mul;
	else if (!strcmp(arg, "overdrive"))
		*num = 4 * mul;
	else if (!strcmp(arg, "chorus"))
		*num = 5 * mul;
	else if (!strcmp(arg, "detox"))
		*num = 6 * mul;

	else if (!strcmp(arg, "superdrive_norm"))
		*num = 30 * mul;
	else if (!strcmp(arg, "tuner"))
		*num = 31 * mul;
	else if (!strcmp(arg, "dd3"))
		*num = 29 * mul;
	else if (!strcmp(arg, "harmonist"))
		*num = 28 * mul;
}

/**
 * @ingroup logic_channel
 */
void cmd_channel_special(BaseSequentialStream *chp, int argc, char *argv[])
{
	_cmd_special(chp, argc, argv, fill_actives.channel->special);
}

static void _cmd_special(BaseSequentialStream *chp, int argc, char *argv[],
		logic_specific_t * spec)
{
	logic_specific_t * temp = NULL;
	if (argc == 3)
	{
		if (spec == NULL )
		{
			temp = alloc_special();
			spec = temp;

			//default setup
					temp->delay.time = -1;
					temp->delay.volume = -1;

					temp->harmonist.harmony = -1;
					temp->harmonist.key = -1;
					temp->harmonist.mode = -1;
					temp->harmonist.volume = -1;
				}

				uint16_t num = atoi(argv[2]);
				if (!strcmp(argv[0], "delay"))
				{
					if (!strcmp(argv[1], "volume"))
					temp->delay.volume = num;
					else if (!strcmp(argv[1], "time"))
					temp->delay.time = num;
				}
				else if (!strcmp(argv[0], "harmonist"))
				{
					if (!strcmp(argv[1], "mode"))
					temp->harmonist.mode = num;
					else if (!strcmp(argv[1], "key"))
					temp->harmonist.key = num;
					else if (!strcmp(argv[1], "harmony"))
					temp->harmonist.harmony = num;
					else if (!strcmp(argv[1], "volume"))
					temp->harmonist.volume = num;
				}
				else
				{
					chprintf(chp, "kanal special nic");
				}
			}
			else
			{
				chprintf(chp, "kanal special nic");
			}
		}
			/*--------------------------------------------------------------------*
			 * function commands
			 *--------------------------------------------------------------------*/

			/**
			 * @ingroup logic_function
			 */
void cmd_functionAdd(BaseSequentialStream *chp, int argc, char *argv[])
{
	logic_function_t * temp;
	if (argc == 1)
	{
		temp = alloc_function();
		//inicializovat všechno na nuly a tak
				fill_actives.function = temp;
				fill_actives.bank->functionCount++;

				temp->effects.w = EFF_NOTHING;
				temp->marshall.effLoop = EFF_NOTHING;
				temp->marshall.gain = 0;
				temp->marshall.volume = 0;
				temp->marshall.mute = EFF_NOTHING;
				temp->marshall.high = EFF_NOTHING;
				temp->name = NULL;
				temp->special = NULL;
				///nula znamená žádná podmínka afunguje dycky
				temp->channelCondition = 0;
				temp->led = 0;
				temp->watchEffect = 0;

				if (fill_actives.bank->functions == NULL )
				fill_actives.bank->functions = temp;

				//uložit jméno do flašky
				fill_actives.function->name = logic_flashWriteName(argv[0]);

			}
			else
			{
				chprintf(chp, "kanal nic");
			}
		}

		/**
		 * @ingroup logic_function
		 */
void cmd_function_effs(BaseSequentialStream *chp, int argc, char *argv[])
{
//přidat pak pomocny jména přimo efektů
	if (argc == 2)
	{
		uint8_t num = atoi(argv[0]);
		_effects_name(&num, argv[0], 1);

		if (fill_ifOn(argv[1]))
		{
			_set_dibit(fill_actives.function->effects.w, num, EFF_ENABLE);
		}
		else if (fill_ifOff(argv[1]))
		{
			_set_dibit(fill_actives.function->effects.w, num, EFF_DISABLE);
		}
		else if (fill_ifToggle(argv[1]))
		{
			_set_dibit(fill_actives.function->effects.w, num, EFF_TOGGLE);
		}
		else
		{
			_set_dibit(fill_actives.function->effects.w, num, EFF_NOTHING);
		}
	}
	else
	{
		chprintf(chp, "funkce efekty nic");
	}
}

/**
 * @ingroup logic_function
 */
void cmd_function_led(BaseSequentialStream *chp, int argc, char *argv[])
{
	if (argc == 1)
	{
		_led(argv[0], &fill_actives.function->led);
	}
	else
	{

	}
}
/**
 * @ingroup logic_function
 */
void cmd_function_watch(BaseSequentialStream *chp, int argc, char *argv[])
{
	if (argc == 1)
	{
		uint8_t num = atoi(argv[0]);
		_effects_name(&num, argv[0], 1);

		fill_actives.function->watchEffect = num;
	}
	else
	{

	}
}
/**
 * @ingroup logic_function
 */
void cmd_function_marshall(BaseSequentialStream *chp, int argc, char *argv[])
{

	uint8_t num = atoi(argv[1]);
	logic_marshall_t * marsh = &fill_actives.channel->marshall;
	if (argc == 2 && num < 5)
	{
		//gain a volume nula pokud se nemá nic dít - nemusim to řešit
		if (!strcmp(argv[0], "preamp"))
		{
			marsh->gain = num;
		}
		else if (!strcmp(argv[0], "volume"))
		{
			marsh->volume = num;
		}
		else if (!strcmp(argv[0], "mute"))
		{
			if (fill_ifOn(argv[1]))
				marsh->mute = EFF_ENABLE;
			else if (fill_ifOff(argv[1]))
				marsh->mute = EFF_DISABLE;
			else if (fill_ifToggle(argv[1]))
				marsh->mute = EFF_TOGGLE;
			else
				marsh->mute = EFF_NOTHING;
		}
		else if (!strcmp(argv[0], "sens"))
		{
			if (fill_ifOn(argv[1]))
				marsh->high = EFF_ENABLE;
			else if (fill_ifOff(argv[1]))
				marsh->high = EFF_DISABLE;
			else if (fill_ifToggle(argv[1]))
				marsh->high = EFF_TOGGLE;
			else
				marsh->high = EFF_NOTHING;
		}
		else if (!strcmp(argv[0], "smycka"))
		{
			if (fill_ifOn(argv[1]))
				marsh->effLoop = EFF_ENABLE;
			else if (fill_ifOff(argv[1]))
				marsh->effLoop = EFF_DISABLE;
			else if (fill_ifToggle(argv[1]))
				marsh->effLoop = EFF_TOGGLE;
			else
				marsh->effLoop = EFF_NOTHING;
		}
		else
		{
			chprintf(chp, "marshal funkce nic");
		}
	}
	else
	{
		chprintf(chp, "marshal funkce nic");
	}
}

/**
 * @ingroup logic_function
 */
void cmd_function_special(BaseSequentialStream *chp, int argc, char *argv[])
{
	_cmd_special(chp, argc, argv, fill_actives.function->special);
}

/**
 * @ingroup logic_function
 */
void cmd_function_prevCondition(BaseSequentialStream *chp, int argc,
		char *argv[])
{
	uint8_t temp;
	if (argc == 1)
	{
		temp = atoi(argv[0]);

		fill_actives.function->channelCondition = temp;
	}
	else
	{
		chprintf(chp, "funkce podminka nic");
	}
}

/*--------------------------------------------------------------------*
 * remap commands
 *--------------------------------------------------------------------*/
/**
 * @ingroup logic_remap
 */
void cmd_remapAdd(BaseSequentialStream *chp, int argc, char *argv[])
{
	logic_remap_t * temp;

	if (argc == 1)
	{
		temp = alloc_remap();
		fill_actives.remap = temp;
		fill_actives.bank->remapCount++;

		if (fill_actives.bank->remaps == NULL )
		fill_actives.bank->remaps = temp;

		//default values
				temp->channelCondition = 0;
				temp->ledBlinkColor = COL_NONE;
				temp->name = NULL;
				temp->ButtonName = NULL;

				temp->newCall.CallName = NULL;
				temp->newCall.call = NULL;
				temp->oldCall.call = NULL;
				temp->oldCall.CallName = NULL;
				temp->save = FALSE;

				//tyhle věci je potřeba vypočitat ze jmen před uloženim do flaš
				temp->button.count = 0;
				temp->button.pin = 0;

				fill_actives.remap->name = logic_flashWriteName(argv[0]);

			}
			else
			{
				chprintf(chp, "remap nic");
			}
		}
		/**
		 * @ingroup logic_remap
		 */
void cmd_remap_led(BaseSequentialStream *chp, int argc, char *argv[])
{
	if (argc == 1)
	{
		_led(argv[0], &fill_actives.remap->ledBlinkColor);
	}
	else
	{
		chprintf(chp, "remap led nic");
	}
}

static void _led(const char * retezec, logic_ledColor_t * led)
{
	if (!strcmp(retezec, "zelena"))
	{
		*led = COL_GREEN;
	}
	else if (!strcmp(retezec, "zluta"))
	{
		*led = COL_YELLOW;
	}
	else if (!strcmp(retezec, "obe"))
	{
		*led = COL_BOTH;
	}
}
/**
 * @ingroup logic_remap
 */
void cmd_remap_prevCondition(BaseSequentialStream *chp, int argc, char *argv[])
{
	uint8_t temp;
	if (argc == 1)
	{
		temp = atoi(argv[0]);

		fill_actives.remap->channelCondition = temp;
	}
	else
	{
		chprintf(chp, "remap podminka nic");
	}
}
/**
 * @ingroup logic_remap
 */
void cmd_remap_setButtonName(BaseSequentialStream *chp, int argc, char *argv[])
{
	if (argc == 1)
	{
		(void) argv;
		fill_actives.remap->ButtonName = logic_flashWriteName(argv[0]);
	}
	else
	{
		chprintf(chp, "remap jmeno tlacitka nic");
	}
}
/**
 * @ingroup logic_remap
 */
void cmd_remap_setNewCallName(BaseSequentialStream *chp, int argc, char *argv[])
{
	if (argc == 1)
	{
		fill_actives.remap->newCall.CallName = logic_flashWriteName(argv[0]);
	}
	else
	{
		chprintf(chp, "remap jmeno nove fce nic");
	}
}
/**
 * @ingroup logic_remap
 */
void cmd_remap_setOldCallName(BaseSequentialStream *chp, int argc, char *argv[])
{
	if (argc == 1)
	{
		fill_actives.remap->oldCall.CallName = logic_flashWriteName(argv[0]);
	}
	else
	{
		chprintf(chp, "remap jmeno stare fce nic");
	}
}

void cmd_remap_save(BaseSequentialStream *chp, int argc, char *argv[])
{
	fill_actives.remap->save = TRUE;
}

/*--------------------------------------------------------------------*
 * button commands
 *--------------------------------------------------------------------*/
/**
 * @ingroup logic_button
 */
void cmd_buttonAdd(BaseSequentialStream *chp, int argc, char *argv[])
{
	logic_button_t * temp;
	uint8_t pushcount;
	uint8_t buttonNumber;

	if (argc == 3)
	{
		pushcount = atoi(argv[2]);
		buttonNumber = atoi(argv[1]);

		temp = alloc_button();
		fill_actives.button = temp;
		fill_actives.bank->buttonCount++;

		if (fill_actives.bank->buttons == NULL )
			fill_actives.bank->buttons = temp;

		//default values
		temp->bit.hold = FALSE;
		temp->bit.now = TRUE;
		temp->calls = NULL;
		temp->buttonCallCount = 0;

		temp->button.pin = 1 << (buttonNumber - 1);
		temp->button.count = pushcount;

		temp->name = logic_flashWriteName(argv[0]);

	}
	else
	{
		chprintf(chp, "tlacitko nic");
	}
}

/**
 * @ingroup logic_button
 */
void cmd_button_hold(BaseSequentialStream *chp, int argc, char *argv[])
{
	if (argc == 1)
	{
		if (!strcmp(argv[0], "drzet"))
		{
			fill_actives.button->bit.hold = TRUE;
		}
		else if (!strcmp(argv[0], "mackat"))
		{
			fill_actives.button->bit.hold = FALSE;
		}
		else
		{
			chprintf(chp, "tlacitko hold nic");
		}
	}
	else
	{
		chprintf(chp, "tlacitko hold nic");
	}
}

/**
 * @ingroup logic_button
 */
void cmd_button_now(BaseSequentialStream *chp, int argc, char *argv[])
{
	if (argc == 1)
	{
		if (!strcmp(argv[0], "hned"))
		{
			fill_actives.button->bit.now = TRUE;
		}
		else if (!strcmp(argv[0], "potom"))
		{
			fill_actives.button->bit.now = FALSE;
		}
		else
		{
			chprintf(chp, "tlacitko hned nic");
		}
	}
	else
	{
		chprintf(chp, "tlacitko hned nic");
	}
}

/**
 * @ingroup logic_button
 */
void cmd_button_call(BaseSequentialStream *chp, int argc, char *argv[])
{
	logic_buttonCall_t * temp;
	if (argc == 1)
	{
		temp = alloc_call();
		fill_actives.button->buttonCallCount++;

		if (fill_actives.button->calls == NULL )
		{
			fill_actives.button->calls = temp;
		}

		temp->call = NULL;

		temp->CallName = logic_flashWriteName(argv[0]);
		/*
		 * zbytek struktury se dopočte až se bude ukládat do flašky
		 */

	}
	else
	{
		chprintf(chp, "tlacitko call nic");
	}

}

