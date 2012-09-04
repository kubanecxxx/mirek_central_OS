/*
 * gui_MainScreen.h
 *
 *  Created on: 27.8.2012
 *      Author: kubanec
 */

#ifndef GUI_MAINSCREEN_H_
#define GUI_MAINSCREEN_H_

#include "framework_button.h"

void gui_mainScreenInit(touch_callback cb);
void gui_putMainScreen(void);
void gui_putSpecial(const logic_specific_t * special);
void gui_putChannel(const logic_channel_t * channel);
void gui_putMarshall(const logic_marshall_t * marshall);

void gui_refreshChannel(void);
void gui_refreshMarshall(void);
void gui_refreshSpecial(void);
void gui_refreshDelay(void);

#endif /* GUI_MAINSCREEN_H_ */
