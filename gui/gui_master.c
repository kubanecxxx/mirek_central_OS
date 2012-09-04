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
#include "ssd1289_port.h"
#include "touch.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

extern logic_active_t active;
extern const logic_base_t * base;
Thread * threadGui;

static void tobanks_callback(framework_button_t * but)
{
	(void) but;

	/*
	 * put bank screen
	 */
	chEvtSignalFlags(threadGui, 0b100);
}

static void bank_callback(framework_button_t * but)
{
	/*
	 * zvolit banku a zpátky do mainscreenu
	 */

	uint8_t i;
	for (i = 0; i < base->bankCount; i++)
		if (!strcmp(but->text, base->banks[i].name))
		{
			/*
			 * make active bank
			 */
			active.bank = &base->banks[i];
		}

	chEvtSignalFlags(threadGui, 0b1000000);
}

void gui_init(void)
{
	bool_t state;

	tft_InitLCD();
	touch_init();
	tft_ClearScreen(LCD_BLUE);
	palSetPadMode(GPIOD, 12, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPad(GPIOD, 12);

	gui_mainScreenInit(tobanks_callback);
	state = gui_bankScreenInit(bank_callback);

	if (state == TRUE)
		gui_putMainScreen();
	else
		disp_PutsStringBackground("Neni konfigurace", 10, 10, LCD_WHITE,
				LCD_BLACK, 16);
}

void gui_thread(void)
{
	threadGui = chThdSelf();
	eventmask_t mask;
	while (TRUE)
	{
		mask = chEvtWaitAny(0b1111100);

		if (mask & 0b100)
		{
			gui_putBankScreen();
		}
		if (mask & 0b1000)
		{
			gui_refreshSpecial();
		}
		if (mask & 0b10000)
		{
			gui_refreshChannel();
		}
		if (mask & 0b100000)
		{
			gui_refreshMarshall();
		}
		if (mask & 0b1000000)
		{
			gui_putMainScreen();
		}
		gui_refreshDelay();
	}
}
