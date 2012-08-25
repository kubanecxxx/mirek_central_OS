/**
 * @file logic_use_test.c
 * @author kubanec
 * @date 24.8.2012
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "mcuconf.h"
#include "logic_types.h"
#include "logic_fill.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#ifdef LOGIC_USE_TEST

const logic_bank_t test_banka =
{

};

#endif

void test_logic_fill(void)
{
	cmd_openLogic(NULL, 0, NULL );
	char * temp1[] =
	{ "banka1", "m" };
	cmd_bankAdd(NULL, 1, temp1);

	char * temp2[] =
	{ "kanal1", "m" };
	cmd_channelAdd(NULL, 1, temp2);
	char * temp3[] =
	{ "kanal2", "m" };
	cmd_channelAdd(NULL, 1, temp3);

	char * temp4[] =
	{ "funkce1", "m" };
	cmd_functionAdd(NULL, 1, temp4);
	char * temp5[] =
	{ "funkce2", "m" };
	cmd_functionAdd(NULL, 1, temp5);

	char * tefds[] =
	{ "banka2", "m" };
	cmd_bankAdd(NULL, 1, tefds);
}

