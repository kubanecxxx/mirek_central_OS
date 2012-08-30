/*
 * framework_button.c
 *
 *  Created on: 27.8.2012
 *      Author: kubanec
 */

#include "ssd1289_port.h"
#include "touch.h"
#include "ch.h"
#include "hal.h"
#include "framework_button.h"
#include "string.h"

static Thread * buttonThread = NULL;
static WORKING_AREA(wa_button,256);

extern EventSource event_touch;
static void framework_buttonThread(void * data);

static framework_button_t * first_button = NULL;
static uint8_t button_count = 0;

void framework_RegisterButton(framework_button_t * button)
{
	if (button == NULL )
		return;

	if (buttonThread == NULL )
		buttonThread = chThdCreateStatic(&wa_button, sizeof(wa_button),
				NORMALPRIO - 1, (tfunc_t) framework_buttonThread,
				&first_button);

	button->p_next = NULL;

	if (first_button == NULL )
	{
		first_button = button;
		button->p_prev = NULL;
	}
	else
	{
		first_button[button_count - 1].p_next = button;
		button->p_prev = &first_button[button_count - 1];
	}
	button_count++;
}

void framework_drawButton(const framework_button_t * button)
{
	tft_DrawRectangle(button->x1, button->y1, button->x2, button->y2,
			button->backgroundColor);

	uint16_t y = button->y1 - button->textSize / 2
			+ ((button->y2 - button->y1) / 2);

	uint16_t delta = (button->x2 - button->x1)
			- (strlen(button->text) * button->textSize);
	uint16_t x = button->x1 + delta / 2;
	disp_PutsStringBackground(button->text, x, y, button->textColor,
			button->backgroundColor, button->textSize);

	tft_DrawPixel(button->x1, button->y1, button->textColor);
	tft_DrawPixel(button->x2 - 1, button->y1, button->textColor);
	tft_DrawPixel(button->x1, button->y2 - 1, button->textColor);
	tft_DrawPixel(button->x2 - 1, button->y2 - 1, button->textColor);
}

void framework_unregisterButton(framework_button_t * button)
{
	(void) button;
}

static void framework_buttonThread(void * data)
{
	EventListener el;
	chEvtRegisterMask(&event_touch, &el, TOUCH_PULL);
	chRegSetThreadName("button framework");

	(void) data;

	//framework

	uint16_t x, y;

	while (TRUE)
	{
		chEvtWaitAny(TOUCH_PULL);

		x = touch_coor.x;
		y = touch_coor.y;

		framework_button_t * but = first_button;

		do
		{
			if (framework_getButtonActive(but))
			{
				if (but->x1 < x && but->x2 > x && but->y1 < y && but->y2 > y)
				{
					but->cb(but);
					break;
				}
			}
			but = but->p_next;
		} while (but != NULL );
	}
}

