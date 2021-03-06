/**
 * @file shell_user.c
 * @author kubanec
 * @date 9.8.2012
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "ch.h"
#include "hal.h"
#include "test.h"

#include "shell.h"
#include "chprintf.h"
#include "ssd1289/ssd1289_lld.h"
#include "ssd1289/print.h"
#include "stdlib.h"
#include "i2c_user.h"
#include "harmonist.h"
#include "delay.h"
#include "switch_master.h"
#include "string.h"
#include "logic_fill.h"
#include "logic_types.h"
#include "logic_use.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#define TEST_WA_SIZE    THD_WA_SIZE(1024)
static void cmd_mem(BaseSequentialStream *chp, int argc, char *argv[])
{
	size_t n, size;

	(void) argv;
	if (argc > 0)
	{
		chprintf(chp, "Usage: mem\r\n");
		return;
	}
	n = chHeapStatus(NULL, &size);
	chprintf(chp, "core free memory : %u bytes\r\n", chCoreStatus());
	chprintf(chp, "heap fragments   : %u\r\n", n);
	chprintf(chp, "heap free total  : %u bytes\r\n", size);
}

static void cmd_threads(BaseSequentialStream *chp, int argc, char *argv[])
{
	static const char *states[] =
	{ THD_STATE_NAMES };
	Thread *tp;

	(void) argv;
	if (argc > 0)
	{
		chprintf(chp, "Usage: threads\r\n");
		return;
	}
	chprintf(chp, "   name    stack prio refs     state time  \r\n");
	tp = chRegFirstThread();
	do
	{
		chprintf(chp, "%15s  %.8lx  %4lu  %4lu  %9s  %lu \r\n", tp->p_name,
				(uint32_t) tp->p_ctx.r13, (uint32_t) tp->p_prio,
				(uint32_t) (tp->p_refs - 1), states[tp->p_state],
				(uint32_t) tp->p_time);
		tp = chRegNextThread(tp);
	} while (tp != NULL );
}

static void cmd_test(BaseSequentialStream *chp, int argc, char *argv[])
{
	Thread *tp;

	(void) argv;
	if (argc > 0)
	{
		chprintf(chp, "Usage: test\r\n");
		return;
	}
	tp = chThdCreateFromHeap(NULL, TEST_WA_SIZE, chThdGetPriority(), TestThread,
			chp);
	if (tp == NULL )
	{
		chprintf(chp, "out of memory\r\n");
		return;
	}
	chThdWait(tp);
}

#ifdef TFT_SHELL
static void cmd_dezo(BaseSequentialStream *chp, int argc, char *argv[])
{
	(void) chp;
	(void) argc;

	disp_PutsStringBackground(argv[0], 10, 10, LCD_BLACK, LCD_RED, 16);
}

static void cmd_clear(BaseSequentialStream *chp, int argc, char *argv[])
{
	(void) chp;
	(void) argc;
	(void) argv;

	tft_ClearScreen(LCD_BLUE);
}
#endif

/*
 * harm commmand
 */
#ifdef I2C_HARMONIST_SHELL
static logic_specific_t special;


void * _sbrk(size_t size)
{
	return chCoreAlloc(size);
}


static void cmd_harmonistUplne(BaseSequentialStream *chp, int argc, char *argv[])
{
	if (argc == 1)
	{
		if (!strcmp(argv[0], "on"))
			harm_enable();
		else if (!strcmp(argv[0], "off"))
			harm_disable();
	}
	else if (argc == 2)
	{
		float temp2 = atoff(argv[1]);
		uint16_t temp = DAC_VOLTAGE(temp2);
		if (!strcmp(argv[0], "volume"))
		{
			_dac_write(CHAN_VOLUME,temp);
		}

		else if (!strcmp(argv[0], "key"))
		{
			_dac_write(CHAN_KEY,temp);
		}

		else if (!strcmp(argv[0], "harmony"))
		{
			_dac_write(CHAN_HARM,temp);
		}

		else if (!strcmp(argv[0], "mode"))
		{
			_dac_write(CHAN_MODE,temp);
		}
		else
			SHELL_ERROR_USER(SHELL_HARMONIST);

		//logic_specific(&special);
	}
	else
	{
		SHELL_ERROR_USER(SHELL_HARMONIST);
	}

}

static void cmd_harmonist(BaseSequentialStream *chp, int argc, char *argv[])
{
	if (argc == 1)
	{
		if (!strcmp(argv[0], "on"))
			harm_enable();
		else if (!strcmp(argv[0], "off"))
			harm_disable();
	}
	else if (argc == 2)
	{
		uint16_t temp = atoi(argv[1]);
		if (!strcmp(argv[0], "volume"))
			special.harmonist.volume = harm_volumeR(temp, 1);

		else if (!strcmp(argv[0], "key"))
			special.harmonist.key = harm_keyR(temp, 1);

		else if (!strcmp(argv[0], "harmony"))
			special.harmonist.harmony = harm_harmonyR(temp, 1);

		else if (!strcmp(argv[0], "mode"))
			special.harmonist.mode = harm_modeR(temp, 1);
		else
			SHELL_ERROR_USER(SHELL_HARMONIST);
		logic_specific(&special);
	}
	else
	{
		SHELL_ERROR_USER(SHELL_HARMONIST);
	}

}
#endif

#ifdef DELAY_SHELL
static void cmd_delay(BaseSequentialStream *chp, int argc, char *argv[])
{
	uint8_t temp;

	if (argc == 1)
	{
		if (!strcmp("off", argv[0]))
		{
			delay_off();
		}
		else if (!strcmp("on", argv[0]))
		{
			delay_on();
		}
		else
		{
			chprintf(chp, "picu\n");
		}
	}
	else if (argc == 2)
	{
		temp = atoi(argv[1]);
		if (!strcmp(argv[0], "volume"))
		{
			special.delay.volume = temp;
		}
		else if (!strcmp(argv[0], "time"))
		{
			special.delay.time = temp;
		}
		else
		{
			SHELL_ERROR_USER(SHELL_DELAY);
		}
		logic_specific(&special);
	}
	else
		SHELL_ERROR_USER(SHELL_DELAY);
}
#endif

#ifdef RELAY_SHELL
static void cmd_relay(BaseSequentialStream *chp, int argc, char *argv[])
{
	if (argc != 2)
	{
		SHELL_ERROR_USER(SHELL_RELAY);
		return;
	}

	uint8_t eff = atoi(argv[0]);
	bool_t state = atoi(argv[1]);

	if (state == TRUE)
		switch_setRelay(eff);
	else
		switch_clearRelay(eff);
}

static void cmd_opto(BaseSequentialStream *chp, int argc, char *argv[])
{
	if (argc != 2)
	{
		SHELL_ERROR_USER(SHELL_OPTO);
		return;
	}

	uint8_t eff = atoi(argv[0]);
	bool_t state = atoi(argv[1]);

	if (state == TRUE)
		opto_enableEffect(eff);
	else
		opto_disableEffect(eff);
}
#endif

static void cmd_marshall(BaseSequentialStream *chp, int argc, char *argv[])
{
	static logic_marshall_t marsh;

	if (argc != 2)
	{
		SHELL_ERROR_USER(SHELL_MARSHALL);
		return;
	}

	uint8_t temp = atoi(argv[1]);

	if (!strcmp(argv[0], "volume"))
	{
		marsh.volume = temp;
	}
	else if (!strcmp(argv[0], "gain"))
	{
		marsh.gain = temp;
	}
	else if (!strcmp(argv[0], "loop"))
	{
		marsh.effLoop = temp;
	}
	else if (!strcmp(argv[0], "mute"))
	{
		marsh.mute = temp;
	}
	else if (!strcmp(argv[0], "high"))
	{
		marsh.high = temp;
	}
	else
	{
		SHELL_ERROR_USER(SHELL_MARSHALL);
		return;
	}
	logic_marshallSetup(&marsh);
}

const ShellCommand commands[] =
{
{ "mem", cmd_mem },
{ "threads", cmd_threads },
{ "test", cmd_test },
#ifdef TFT_SHELL
		{	"dezo", cmd_dezo},
		{	"clear", cmd_clear},
#endif
#ifdef I2C_HARMONIST_SHELL
		{ "harm", cmd_harmonist },
		{ "harm_uplne", cmd_harmonistUplne },
#endif
#ifdef DELAY_SHELL
		{ "delay", cmd_delay },
#endif
#ifdef RELAY_SHELL
		{ "relay", cmd_relay },
		{ "opto", cmd_opto },
#endif
		{ "marshall", cmd_marshall },
		//structs filling via usb
#include "logic_shell.h"

		{ NULL, NULL } };

