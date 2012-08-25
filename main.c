/*
 ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
 2011,2012 Giovanni Di Sirio.

 This file is part of ChibiOS/RT.

 ChibiOS/RT is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 ChibiOS/RT is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>

#include "ch.h"
#include "hal.h"
#include "shell.h"

#include "ssd1289/ssd1289_lld.h"
#include "ssd1289/print.h"
#include "touch.h"

#include "i2c_user.h"
#include "usb_user.h"
#include "rs232.h"
#include "switch_master.h"
#include "logic_use_test.h"

/*
 * external interrupt system config
 */
static const EXTConfig extcfg =
{
{
{ EXT_CH_MODE_DISABLED, NULL },
{ EXT_CH_MODE_DISABLED, NULL },
{ EXT_CH_MODE_DISABLED, NULL },
{ EXT_CH_MODE_DISABLED, NULL },
{ EXT_CH_MODE_DISABLED, NULL },
{ EXT_CH_MODE_DISABLED, NULL },
{ EXT_CH_MODE_DISABLED, NULL },
{ EXT_CH_MODE_DISABLED, NULL },
{ EXT_CH_MODE_DISABLED, NULL },
{ EXT_CH_MODE_DISABLED, NULL },
{ EXT_CH_MODE_DISABLED, NULL },
{ TOUCH_EXTI },
{ EXT_CH_MODE_DISABLED, NULL },
{ EXT_CH_MODE_DISABLED, NULL },
{ EXT_CH_MODE_DISABLED, NULL },
{ EXTERNAL_INTERRUPT_PCA },
{ EXT_CH_MODE_DISABLED, NULL },
{ EXT_CH_MODE_DISABLED, NULL },
{ EXT_CH_MODE_DISABLED, NULL },
{ EXT_CH_MODE_DISABLED, NULL },
{ EXT_CH_MODE_DISABLED, NULL },
{ EXT_CH_MODE_DISABLED, NULL },
{ EXT_CH_MODE_DISABLED, NULL } } };

/*===========================================================================*/
/* Generic code.                                                             */
/*===========================================================================*/
int main(void)
{
	/*
	 * System initializations.
	 * - HAL initialization, this also initializes the configured device drivers
	 *   and performs the board-specific initializations.
	 * - Kernel initialization, the main() function becomes a thread and the
	 *   RTOS is active.
	 */

	halInit();
	chSysInit();

	test_logic_fill();

	/*
	 * start shell for RS232 - communication with marshall
	 */
	serial_init();

	/*
	 * Activates the USB driver and then the USB bus pull-up on D+.
	 */
	usb_user_init();

	/*
	 * Shell manager initialization.
	 */
	shellInit();

	/*
	 * Init tft display ssd1289
	 */
	tft_InitLCD();
	touch_init();
	tft_ClearScreen(LCD_BLUE);
	palSetPadMode(GPIOD, 12, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPad(GPIOD, 12);

	/**
	 * @brief init I2C1, make i2c1 thread
	 */
#ifdef I2C_MODULE
	i2c1_init();
#endif

	/**
	 * @brief init gpio for relay switching and optocouplers watching
	 */
	switch_masterGpioInit();

	/*
	 * start external interrupt system
	 */
	extStart(&EXTD1, &extcfg);

	/*
	 * Normal main() thread activity, in this demo it does nothing except
	 * sleeping in a loop and check the button state.
	 */

	EventListener el;
	chEvtRegister(&event_touch, &el, TOUCH_PUSH);

#ifdef I2C_TEST
	i2c_test();
#endif

	while (TRUE)
	{

		chThdSleepMilliseconds(3);
		if (touch_getState() == PUSHED)
		{
			palSetPad(GPIOD, 12);
			tft_DrawPixel(touch_coor.x, touch_coor.y, LCD_BLACK);
		}
		else
		{
			palClearPad(GPIOD, 12);
			chEvtWaitAny(TOUCH_PUSH);
		}
	}

}
