/**
 * @file gui_banks.c
 * @author kubanec
 * @date 28.8.2012
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "ch.h"
#include "hal.h"
#include "framework_button.h"
#include "logic_types.h"
#include "ssd1289_port.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static framework_button_t * button_first;
extern framework_button_t button_toBanks;
extern const logic_base_t * base;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void gui_putBankScreen(void)
{
	/*
	 * aktivovat a vykreslit tlačitka, deaktivovat to prvni
	 */
	button_toBanks.bit.active = FALSE;
	framework_button_t * but = button_first;

	do
	{
		but->bit.active = TRUE;
		framework_drawButton(but);
	} while ((but = but->p_next) != NULL );
}

void gui_bankScreenInit(touch_callback cb)
{
	/*
	 * alokovat paměť pro tlačitka s bankou, vytvořit tlačitka a registrovat
	 */
	uint8_t i;
	uint16_t y = 30;
	framework_button_t * temp;
	for (i = 0; i < base->bankCount; i++)
	{
		temp = (framework_button_t *) chCoreAlloc(sizeof(framework_button_t));
		framework_RegisterButton(temp);

		temp->backgroundColor = LCD_GREY;
		temp->bit.active = FALSE;
		temp->cb = cb;
		temp->text = base->banks[i].name;
		temp->textColor = LCD_WHITE;
		temp->textSize = 16;
		temp->x1 = 20;
		temp->x2 = 80;
		temp->y1 = y;
		y += 24;
		temp->y2 = y;
		y += 20;

		if (i == 0)
			button_first = temp;
	}
}
