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
#include "string.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#define TEST_WA_SIZE    THD_WA_SIZE(256)
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
static void cmd_harmonist(BaseSequentialStream *chp, int argc, char *argv[])
{
	uint8_t txbuf[3];
	uint8_t msg;
	//cvak dolu
	if (argc == 0)
	{
		txbuf[0] = 1;
		txbuf[1] = 0;

		i2cAcquireBus(&I2CD1);
		msg = i2cMasterTransmitTimeout(&I2CD1, HARM_PCA, txbuf, 2, NULL, 0,
				TIME_INFINITE );
		i2cReleaseBus(&I2CD1);

		chThdSleepMilliseconds(20);

		txbuf[1] |= _BV(1);
		i2cAcquireBus(&I2CD1);
		msg = i2cMasterTransmitTimeout(&I2CD1, HARM_PCA, txbuf, 2, NULL, 0,
				TIME_INFINITE );
		i2cReleaseBus(&I2CD1);
	}
	else if (argc == 2)
	{
		set_harmonist(atoi(argv[0]), atoi(argv[1]));
	}
	else
	{
		chprintf(chp, "picu!!!\n\r");
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
			delay_volume(temp);
		}
		else if (!strcmp(argv[0], "time"))
		{
			delay_time(temp);
		}
		else
		{
			chprintf(chp, "picu\n");
		}
	}
}
#endif

#ifdef RFM_SHELL
static void cmd_rfm(BaseSequentialStream *chp, int argc, char *argv[])
{
	chprintf(chp, "nirq\n");
	rf_prepare();

	rf_send(2);

	//dummy
	rf_send(0xAA);
	rf_send(0xAA);
	chprintf(chp, "jiz\n");
}
#endif

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
		{	"harm", cmd_harmonist},
#endif
#ifdef DELAY_SHELL
		{	"delay", cmd_delay},
#endif
#ifdef RFM_SHELL
		{ "rfm", cmd_rfm },
#endif
		{ NULL, NULL } };

