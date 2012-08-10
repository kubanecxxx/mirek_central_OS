/**
 * @file harmonist.h
 * @author kubanec
 * @date 10.8.2012
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef HARMONIST_H_
#define HARMONIST_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "i2c_user.h"

/* Exported types ------------------------------------------------------------*/

/*
 * MODE
 */
typedef union
{
	struct
	{
		uint16_t MAJOR;
		uint16_t MINOR;
		uint16_t PITCH_SHIFT;
		uint16_t DETUNE;
		uint16_t SBEND;
	}s;
	uint16_t a[5];
}harmonizer_mode_t;

/*
 * KEY
 */

typedef union
{
	struct
	{
		uint16_t C;
		uint16_t Cx;
		uint16_t D;
		uint16_t Eb;
		uint16_t E;
		uint16_t F;
		uint16_t Fx;
		uint16_t G;
		uint16_t Gx;
		uint16_t A;
		uint16_t Ax;
		uint16_t H;
	}s;
	uint16_t a[12];
}harmonizer_key_t;

/*
 * HARMONY
 */
typedef union
{
	uint16_t a[11];
}harmonizer_harmony_t;

typedef struct harmonizer_t
{
	harmonizer_mode_t MODE;
	harmonizer_key_t KEY;
	harmonizer_harmony_t HARMONY;
}harmonizer_t;

/* Exported constants --------------------------------------------------------*/
/*
 * DAC channels
 */
typedef enum
{
	CHAN_MODE = 0, CHAN_KEY=2, CHAN_HARM=3
}DAC_channel;

/*
 * HARMONIST PCA PINS
 */
typedef enum
{
	HARM_LDAC = 0, HARM_EFF=1, HARM_BUT, HARM_LED
}HARM_pins;

const harmonizer_t HARMONIZER =
{
	/*
	 * MODEs
	 */
	{
		DAC_VOLTAGE(0),
		DAC_VOLTAGE(0.151),
		DAC_VOLTAGE(0.494),
		DAC_VOLTAGE(0.839),
		DAC_VOLTAGE(1.201)
	},
	/*
	 * KEY
	 */
	{
		DAC_VOLTAGE(0),
		DAC_VOLTAGE(0.181),
		DAC_VOLTAGE(0.493),
		DAC_VOLTAGE(0.810),
		DAC_VOLTAGE(1.129),
		DAC_VOLTAGE(1.462),
		DAC_VOLTAGE(1.810),
		DAC_VOLTAGE(2.141),
		DAC_VOLTAGE(2.464),
		DAC_VOLTAGE(2.786),
		DAC_VOLTAGE(3.104),
		DAC_VOLTAGE(3.288)
	},
	/*
	 * HARMONY
	 */
	{
		DAC_VOLTAGE(0),
		DAC_VOLTAGE(0.205),
		DAC_VOLTAGE(0.558),
		DAC_VOLTAGE(0.867),
		DAC_VOLTAGE(1.254),
		DAC_VOLTAGE(1.661),
		DAC_VOLTAGE(2.041),
		DAC_VOLTAGE(2.436),
		DAC_VOLTAGE(2.765),
		DAC_VOLTAGE(3.132),
		DAC_VOLTAGE(3.228)
	}
};
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* HARMONIST_H_ */
