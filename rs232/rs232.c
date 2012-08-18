/**
 * @file rs232.c
 * @author kubanec
 * @date 18.8.2012
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "ch.h"
#include "hal.h"
#include "shell.h"

#include "rs232.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static WORKING_AREA(wa_shell, 2048);
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

static void chatra_cmd(BaseSequentialStream *chp, int argc, char *argv[])
{
	chprintf(chp, "vole\n");
}

static const ShellCommand comms[] =
{
{ "chatra", chatra_cmd },
{ NULL, NULL } };

static const ShellConfig shell_cfg =
{ (BaseSequentialStream *) &SD2, comms };

static const SerialConfig cfg =
{ 9600, 0, 0, 0 };

void serial_init(void)
{
	sdStart(&SD2, &cfg);
	palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
	palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));

	shellCreateStatic(&shell_cfg, &wa_shell, sizeof(wa_shell), NORMALPRIO);
}
