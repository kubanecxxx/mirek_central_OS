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
	chprintf(chp, "    addr    stack prio refs     state time\r\n");
	tp = chRegFirstThread();
	do
	{
		chprintf(chp, "%.8lx %.8lx %4lu %4lu %9s %lu\r\n", (uint32_t) tp,
				(uint32_t) tp->p_ctx.r13, (uint32_t) tp->p_prio,
				(uint32_t) (tp->p_refs - 1), states[tp->p_state],
				(uint32_t) tp->p_time);
		tp = chRegNextThread(tp);
	} while (tp != NULL);
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
	if (tp == NULL)
	{
		chprintf(chp, "out of memory\r\n");
		return;
	}
	chThdWait(tp);
}

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

const ShellCommand commands[] =
{
{ "mem", cmd_mem },
{ "threads", cmd_threads },
{ "test", cmd_test },
{ "dezo", cmd_dezo },
{ "clear", cmd_clear },
{ NULL, NULL } };


