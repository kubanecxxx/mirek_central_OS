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

#undef LOGIC_USE_TEST
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

	char * dskl[] =
	{ "remap1", "" };
	char * dsklsd[] =
	{ "but1", "" };
	char * dsklsda[] =
	{ "funkce1", "" };
	char * dsklsdb[] =
	{ "funkce2", "" };
	cmd_remapAdd(NULL, 1, dskl);
	cmd_remap_setButtonName(NULL, 1, dsklsd);
	cmd_remap_setOldCallName(NULL, 1, dsklsda);
	cmd_remap_setNewCallName(NULL, 1, dsklsdb);

	char * lsdf[] =
	{ "but1", "3", "1" };
	char * asdf[] =
	{ "funkce1", "" };
	char * asdff[] =
	{ "kanal1", "" };
	char * rem[] =
	{ "remap1", "" };
	cmd_buttonAdd(NULL, 3, lsdf);
	cmd_button_call(NULL, 1, asdf);
	cmd_button_call(NULL, 1, asdff);
	cmd_button_call(NULL, 1, rem);

	char * lsdfa[] =
	{ "but2", "1", "2" };
	char * asdfa[] =
	{ "funkce1", "" };
	char * asdffa[] =
	{ "kanal2", "" };
	cmd_buttonAdd(NULL, 3, lsdfa);
	cmd_button_call(NULL, 1, asdfa);
	cmd_button_call(NULL, 1, asdffa);

	char * dsk2l[] =
	{ "remap2", "" };
	cmd_remapAdd(NULL, 1, dsk2l);

	cmd_closeLogic(NULL, 0, NULL );
}

