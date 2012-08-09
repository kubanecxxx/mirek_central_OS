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
#include "test.h"

#include "usb_cdc.h"
#include "shell.h"
#include "chprintf.h"

#include "ssd1289/ssd1289_lld.h"
#include "ssd1289/print.h"

#include "i2c_user.h"

/*===========================================================================*/
/* Command line related.                                                     */
/*===========================================================================*/


/*===========================================================================*/
/* Generic code.                                                             */
/*===========================================================================*/

/*u
 * Red LED blinker thread, times are in milliseconds.
 */
static WORKING_AREA(waThread1, 128);
static msg_t Thread1(void *arg)
{

	(void) arg;
	chRegSetThreadName("blinker");
	while (TRUE)
	{
		palClearPad(GPIOD, GPIOD_LED4);
		chThdSleepMilliseconds(500);
		palSetPad(GPIOD, GPIOD_LED4);
		chThdSleepMilliseconds(500);
	}
	return 0;
}

/*
 * Application entry point.
 */
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

	/*
	 * Activates the USB driver and then the USB bus pull-up on D+.
	 */
	usb_user_init();

	/*
	 * Shell manager initialization.
	 */
	shellInit();

	/*
	 * Creates the blinker thread.
	 */
	chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL );

	/*
	 * Init tft display ssd1289
	 */
	tft_InitLCD();
	tft_ClearScreen(LCD_BLUE);

	/**
	 * @brief init I2C1, make i2c1 thread
	 */
	i2c1_init();

	/*
	 * Normal main() thread activity, in this demo it does nothing except
	 * sleeping in a loop and check the button state.
	 */
	while (TRUE)
	{
		chThdSleepMilliseconds(1000);
	}
}
