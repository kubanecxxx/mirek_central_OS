/*
 * gui_MainScreen.c
 *
 *  Created on: 27.8.2012
 *      Author: kubanec
 */

#include "logic_types.h"
#include "ch.h"
#include "hal.h"
#include "stdlib.h"
#include "stdio.h"
#include "ssd1289/print.h"
#include "ssd1289/ssd1289_lld.h"

typedef struct
{
	const logic_specific_t * special;
	const logic_channel_t * channel;
	const logic_marshall_t * marshall;
} gui_mainScreen;

static gui_mainScreen data =
{ NULL, NULL, NULL };

extern Thread * threadGui;

void gui_putSpecial(const logic_specific_t * special)
{
	data.special = special;
	chEvtSignalFlags(threadGui, 0);
}

void gui_putChannel(const logic_channel_t * channel)
{
	data.channel = channel;
	chEvtSignalFlags(threadGui, 0);
}

void gui_putMarshall(const logic_marshall_t * marshall)
{
	data.marshall = marshall;
	chEvtSignalFlags(threadGui, 0);
}

static void putSpecial(const logic_specific_t * special)
{
	char buffer[30];
	if (special != NULL )
	{
		sprintf(buffer, "Delay volume: %d, time: %d", special->delay.volume,
				special->delay.time);
		disp_PutsStringBackground(buffer, 10, 10, LCD_WHITE, LCD_BLACK, 16);
		sprintf(buffer, "Harmonist volume: %d", special->harmonist.volume);
		disp_PutsStringBackground(buffer, 10, 30, LCD_WHITE, LCD_BLACK, 16);
	}
	else
	{
		tft_DrawRectangle(10, 10, 200, 30, LCD_BLACK);
	}
}

static void putMarshall(const logic_marshall_t * marshall)
{
	if (marshall != NULL )
	{

	}
	else
	{

	}
}

static void putChannel(const logic_channel_t * channel)
{
	if (channel != NULL )
	{

	}
	else
	{

	}
}

/*
 * tlačitka registrovat všechny hned někde v initu - hlavně vykreslovat grafiku jenom z
 * jednoho vlákna
 */

void gui_putMainScreen(void)
{
	putSpecial(data.special);
	putMarshall(data.marshall);
	putChannel(data.channel);

	//tlačitko vykreslit, aktivovat, deaktivovat všecky ostatni
	//commit
}

/*
 * v bank screenu vykreslit a registrovat tlačitka a deaktivovat to z mainscreenu
 */
