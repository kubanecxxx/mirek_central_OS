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
#include "framework_button.h"
#include "string.h"

framework_button_t button_toBanks;

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

extern framework_button_t * first;
extern const logic_base_t * base;

void gui_putMainScreen(void)
{
	putSpecial(data.special);
	putMarshall(data.marshall);
	putChannel(data.channel);

	uint8_t i;
	for (i = 0; i < base->bankCount; i++)
	{
		first[i].bit.active = FALSE;
	}

	button_toBanks.bit.active = TRUE;
	framework_drawButton(&button_toBanks);
	//tlačitko vykreslit, aktivovat, deaktivovat všecky ostatni

	//commit
}

void gui_mainScreenInit(touch_callback cb)
{
	button_toBanks.backgroundColor = LCD_BLACK;
	button_toBanks.bit.active = FALSE;
	button_toBanks.cb = cb;
	button_toBanks.text = "Banky";
	button_toBanks.textColor = LCD_GREY;
	button_toBanks.textSize = 16;
	button_toBanks.x1 = 280;
	button_toBanks.x2 = 319;
	button_toBanks.y1 = 20;
	button_toBanks.y2 = 50;

	framework_RegisterButton(&button_toBanks);
}

/*
 * v bank screenu vykreslit a registrovat tlačitka a deaktivovat to z mainscreenu
 */
/*
 * udělat podporu v shellu pro nastavování harmonistu a delayje až si to bude mirek nastavovat
 */
