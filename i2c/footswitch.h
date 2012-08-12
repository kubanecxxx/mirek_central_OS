/**
 * @file footswitch.h
 * @author kubanec
 * @date 10.8.2012
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FOOTSWITCH_H_
#define FOOTSWITCH_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "ch.h"
#include "hal.h"
/* Exported types ------------------------------------------------------------*/
typedef struct
{
	uint16_t count;
	uint16_t pin;
}foot_t;
/* Exported constants --------------------------------------------------------*/
/*
 * External interrupt pin from PCA buttons
 */
#define PCA_INTERRUPT 			15
/*
 * footswitch PCA adressess
 */
#define PCA_LED_1_ADDRESS 		0b0100000
#define PCA_LED_2_ADDRESS 		0b0100001
#define PCA_BUTTONS_ADDRESS 	0b0100010

/**
 * PCA register set
 */
#define PCA_IDR 	0
#define PCA_ODR		1
#define PCA_POL		2
#define PCA_DDR		3

/**
 * PCA port direction
 */
#define PCA_OUTPUT  0
#define PCA_INPUT	1

#define STEP_TIMEOUT 500

/*
 * buttons event ID
 */
#define BUTTON_EVENT_ID 1
/* Exported macro ------------------------------------------------------------*/
extern EventSource event_i2c_buttons;
extern volatile foot_t footswitch;

#define foot_SetLedsYellow(data) _foot_SetLeds(PCA_LED_1_ADDRESS,data)
#define foot_SetLedsGreen(data) _foot_SetLeds(PCA_LED_2_ADDRESS,data)
#define foot_SetLedsBoth(yellow,green) foot_SetLedsYellow(yellow); foot_SetLedsGreen(green)

#define EXTERNAL_INTERRUPT_PCA  EXT_CH_MODE_FALLING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOC, foot_buttons_interrupt
/* Exported functions --------------------------------------------------------*/
void foot_init(void);
void _foot_SetLeds(uint8_t address, uint8_t data);
void foot_buttons_interrupt(EXTDriver *extp, expchannel_t channel);

#ifdef __cplusplus
}
#endif

#endif /* FOOTSWITCH_H_ */