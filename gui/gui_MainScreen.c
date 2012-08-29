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
#include "chsprintf.h"

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
	if (threadGui != NULL )
		chEvtSignalFlags(threadGui, 0b1000);
}

void gui_putChannel(const logic_channel_t * channel)
{
	data.channel = channel;
	if (threadGui != NULL )
		chEvtSignalFlags(threadGui, 0b10000);
}

void gui_putMarshall(const logic_marshall_t * marshall)
{
	data.marshall = marshall;
	if (threadGui != NULL )
		chEvtSignalFlags(threadGui, 0b100000);
}

#define SPEC(string, x,y) disp_PutsStringBackground(string,x,y,LCD_WHITE,LCD_BLACK,16)

void putSpecial(const logic_specific_t * special)
{
	char buffer[30];
	if (special == NULL )
		return;

	SPEC("Delay: ", 5, 10);
	chsprintf(buffer, "Vol.:%3d time: %3d", special->delay.volume,
			special->delay.time);
	SPEC(buffer, 5, 40);

	SPEC("Harmonist: ", 5, 70);
	chsprintf(buffer, "Vol.:%3d mode: %3d", special->harmonist.volume,
			special->harmonist.mode);
	SPEC(buffer, 5, 100);
	chsprintf(buffer, "Key.:%3d harm: %3d", special->harmonist.key,
			special->harmonist.harmony);
	SPEC(buffer, 5, 130);

}

static void putMarshall(const logic_marshall_t * marshall)
{
	if (marshall == NULL || (marshall->gain == 0 || marshall->volume == 0))
		return;

	char buffer[30];

	chsprintf(buffer, "Pre.:%3d Master:%3d", marshall->gain, marshall->volume);
	SPEC(buffer, 5, 160);
}

static void putChannel(const logic_channel_t * channel)
{
	if (channel == NULL )
		return;

	SPEC(channel->name, 5, 190);
}

/*
 * tlačitka registrovat všechny hned někde v initu - hlavně vykreslovat grafiku jenom z
 * jednoho vlákna
 */

extern framework_button_t * button_first;
extern const logic_base_t * base;

void gui_putMainScreen(void)
{
	tft_ClearScreen(LCD_BLACK);

	putSpecial(data.special);
	putMarshall(data.marshall);
	putChannel(data.channel);

	uint8_t i;
	for (i = 0; i < base->bankCount; i++)
	{
		button_first[i].bit.active = FALSE;
	}

	button_toBanks.bit.active = TRUE;
	framework_drawButton(&button_toBanks);
	//tlačitko vykreslit, aktivovat, deaktivovat všecky ostatni

	//commit
}

void gui_refreshSpecial(void)
{
	putSpecial(data.special);
}

void gui_refreshMarshall(void)
{
	putMarshall(data.marshall);
}

void gui_refreshChannel(void)
{
	putChannel(data.channel);
}

void gui_mainScreenInit(touch_callback cb)
{
	button_toBanks.backgroundColor = LCD_GREY;
	button_toBanks.bit.active = FALSE;
	button_toBanks.cb = cb;
	button_toBanks.text = "Banky";
	button_toBanks.textColor = LCD_BLACK;
	button_toBanks.textSize = 16;
	button_toBanks.x1 = 180;
	button_toBanks.x2 = 300;
	button_toBanks.y1 = 190;
	button_toBanks.y2 = 220;

	framework_RegisterButton(&button_toBanks);
}
