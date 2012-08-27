/*
 * framework_button.c
 *
 *  Created on: 27.8.2012
 *      Author: kubanec
 */

#include "framework_button.h"
#include "ssd1289_port.h"
#include "touch.h"
#include "ch.h"
#include "hal.h"

static Thread * buttonThread = NULL;
static WORKING_AREA(wa_button,256);
extern EventSource event_touch;
static void framework_buttonThread(void * data);

typedef (*touch_callback)(void);

typedef struct
{
	touch_callback cb;
	uint16_t x1;
	uint16_t x2;
	uint16_t y1;
	uint16_t y2;
	const char * text;
	uint16_t textColor;
	uint16_t backgroundColor;

} framework_button_t;

void framework_RegisterButton(const framework_button_t * button)
{
	if (buttonThread == NULL )
		buttonThread = chThdCreateStatic(&wa_button, sizeof(wa_button),
				NORMALPRIO - 1, (tfunc_t) framework_buttonThread, NULL );
}

void framework_unregisterButton(const framework_button_t * button)
{

}

static void framework_buttonThread(void * data)
{
	EventListener el;
	chEvtRegisterMask(&event_touch, &el, TOUCH_PULL);

	while (TRUE)
	{
		chEvtWaitAny(TOUCH_PULL);

		touch_coor.x;
	}
}

