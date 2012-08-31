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
#include "touch.h"
#include "i2c_user.h"
#include "usb_user.h"
#include "switch_master.h"
#include "logic_use.h"
#include "logic_types.h"
#include "gui_master.h"

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

extern logic_buttonCall_t * logic_pointery[60] ;

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


	/*
	 * kruciální řádek protože pokud se to zapne bez debugru tak je tam
	 * jenom bordel a když se to pak srovnává s NULL tak se to cely
	 * vymrdá a skoči do unhandled exception
	 */
	uint8_t i;
	for (i = 0 ; i< 60 ; i++)
		logic_pointery[i] = NULL;

	//test_logic_fill();

	chThdSleepMilliseconds(100);

	/*
	 * Activates the USB driver and then the USB bus pull-up on D+.
	 */
	usb_user_init();

	/*
	 * Shell manager initialization.
	 */
	shellInit();

	/*
	 * Init tft display ssd1289 and gui
	 */
	gui_init();


	/**
	 * @brief start the whole bad thing
	 */
	logic_init();

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

	gui_thread();

	while(TRUE);
}
