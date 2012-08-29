/*
 * framework_button.h
 *
 *  Created on: 27.8.2012
 *      Author: kubanec
 */

#ifndef FRAMEWORK_BUTTON_H_
#define FRAMEWORK_BUTTON_H_

typedef struct framework_button_t framework_button_t;

typedef void(*touch_callback)(framework_button_t * but);

struct framework_button_t
{
	touch_callback cb;
	uint16_t x1;
	uint16_t x2;
	uint16_t y1;
	uint16_t y2;
	const char * text;
	uint16_t textColor;
	uint16_t backgroundColor;
	uint8_t textSize;
	struct
	{
		bool_t active :1;
	} bit;

//double linkded list
	framework_button_t * p_next;
	framework_button_t * p_prev;
};

void framework_RegisterButton(framework_button_t * button);
void framework_drawButton(const framework_button_t * button);

#define framework_setButtonActive(button,bolean) button->bit.active = bolean
#define framework_getButtonActive(button) (button->bit.active)

#endif /* FRAMEWORK_BUTTON_H_ */
