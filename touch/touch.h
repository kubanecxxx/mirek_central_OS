/**
 * @file touch.h
 * @author kubanec
 * @date 12.8.2012
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef TOUCH_H_
#define TOUCH_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "ssd1289/ssd1289_lld.h"
/* Exported types ------------------------------------------------------------*/
typedef struct
{
	uint16_t x;
	uint16_t y;
}touch_t;

typedef enum
{
	TOUCH_PUSH = 1, TOUCH_PULL = 2
}touch_event_t;

typedef enum
{
	PUSHED, FREE = !PUSHED
}touch_state_t;

extern EventSource event_touch;
extern volatile touch_t touch_coor;
/* Exported constants --------------------------------------------------------*/
#define CHX 0x90
#define CHY 0xD0
/* Exported macro ------------------------------------------------------------*/
#define TOUCH_EXTI EXT_CH_MODE_FALLING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOB, touch_interrupt
/* Exported functions --------------------------------------------------------*/
void touch_init(void);
uint16_t _touch_get(uint8_t reg);
void touch_interrupt(EXTDriver *extp, expchannel_t channel);
touch_state_t touch_getState(void);

#define touch_getX() (((0x7FF - _touch_get(CHX))*10)/64)
#define touch_getY() ((_touch_get(CHY)*10)/85)

#ifdef __cplusplus
}
#endif

#endif /* TOUCH_H_ */
