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
} foot_t;
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
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void foot_init(void);

#ifdef __cplusplus
}
#endif

#endif /* FOOTSWITCH_H_ */
