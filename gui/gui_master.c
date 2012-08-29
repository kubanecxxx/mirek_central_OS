/**
 * @file gui_master.c
 * @author kubanec
 * @date 28.8.2012
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "ch.h"
#include "hal.h"
#include "logic_types.h"
#include "framework_button.h"
#include "gui_banks.h"
#include "gui_MainScreen.h"
#include "string.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

extern logic_active_t active;
extern const logic_base_t * base;

static void tobanks_callback(framework_button_t * but)
{
	(void) but;

	/*
	 * put bank screen
	 */
	gui_putMainScreen();
}

static void bank_callback(framework_button_t * but)
{
	/*
	 * zvolit banku a zpátky do mainscreenu
	 */

	uint8_t i ;
	for (i = 0 ; i < base->bankCount; i++)
	if (!strcmp(but->text,base->banks[i].name))
	{
		/*
		 * make active bank
		 */
		active.bank = &base->banks[i];
	}

	gui_putBankScreen();
}

void gui_init(void)
{
	gui_mainScreenInit(tobanks_callback);
	gui_bankScreenInit(bank_callback);

	gui_putMainScreen();
}
