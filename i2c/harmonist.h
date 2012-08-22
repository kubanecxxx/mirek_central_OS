/**
 * @file harmonist.h
 * @author kubanec
 * @date 10.8.2012
 *
 * @brief harmonist voltage constants
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
#define HARM_MODE_COUNT 5
#define HARM_KEY_COUNT 12
#define HARM_SHIFT_COUNT 11
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
	uint16_t a[HARM_MODE_COUNT];
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
	uint16_t a[HARM_KEY_COUNT];
}harmonizer_key_t;

/*
 * HARMONY
 */
typedef union
{
	uint16_t a[HARM_SHIFT_COUNT];
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
	CHAN_MODE = 0, CHAN_VOLUME = 1, CHAN_KEY = 2, CHAN_HARM = 3
}DAC_channel;

/*
 * HARMONIST PCA PINS
 */
typedef enum
{
	HARM_LDAC = 0, HARM_EFF = 1, HARM_BUT = 2, HARM_LED = 3
}HARM_pins;

/*
 * voltage constants set for Harmonizer analog inputs
 */
extern const harmonizer_t HARMONIZER;
extern bool_t _harm_enabled;

/*
 * I2C address set for harmonizer - PCA9536 + MCP4728
 */
#define HARM_PCA 0b1000001 //Harmnoist PCA 7-bit address
#define DACAN 0b1100000 //DAC 7-bit address MCP4728
/**
 * @brief output voltage to bits DAC MCP4728
 */
#define Vref 3.3
#define DAC_VOLTAGE(Vout) ((uint16_t)(Vout*4096/Vref))

/* Exported macro ------------------------------------------------------------*/
/*
 * DAC channels
 */
#define harm_volume(vol) vol < 4096 ? _dac_write(CHAN_VOLUME,vol) : NULL
#define harm_mode(x) x < HARM_MODE_COUNT ? _dac_write(CHAN_MODE,x) : NULL
#define harm_key(x)  x < HARM_KEY_COUNT ? _dac_write(CHAN_KEY,x) :NULL
#define harm_harmony(x) x < HARM_SHIFT_COUNT ? _dac_write(CHAN_HARM,x) :NULL

#define harm_volumeR(vol,response) vol < 4096 ? _dac_write(CHAN_VOLUME,vol) : response
#define harm_modeR(x,response) x < HARM_MODE_COUNT ? _dac_write(CHAN_MODE,x) : response
#define harm_keyR(x,response)  x < HARM_KEY_COUNT ? _dac_write(CHAN_KEY,x) :response
#define harm_harmonyR(x,response) x < HARM_SHIFT_COUNT ? _dac_write(CHAN_HARM,x) :response

/*
 * PCA inputs
 * @param inputs read by harm_getInputs
 */
#define harm_getInput_LDAC(inputs) ((inputs >> HARM_LDAC) & 1)
#define harm_getInput_EFF(inputs) ((inputs >> HARM_EFF) & 1)
#define harm_getInput_BUT(inputs) ((inputs >> HARM_BUT) & 1)
#define harm_getInput_LED(inputs) ((inputs >> HARM_LED) & 1)

/*
 * PCA outputs
 */
#define harm_releaseButton(inputs) _harm_SetOutputs(inputs | _BV(HARM_EFF))
#define harm_pushButton(inputs) _harm_SetOutputs(inputs & (~_BV(HARM_EFF)))
#define harm_setLDAC(inputs) _harm_SetOutputs(inputs | _BV(HARM_LDAC))

/*
 * enable/disable harmonizer
 */
#define harm_enable() _harm_enabled = TRUE
#define harm_disable() _harm_enabled = FALSE
#define harm_toggle() _harm_enabled = !_harm_enabled

/* Exported functions --------------------------------------------------------*/
#ifdef I2C_HARMONIST
uint8_t harm_getInputs(void);
void harm_init(void);
//void harm_set(uint8_t channel, uint16_t index);
void _dac_write(DAC_channel channel, uint16_t voltage);
void _harm_SetOutputs(uint8_t data);
#else
#define harm_init() NULL
#define harm_getInputs() 0
//#define set_harmonist(x,y) NULL
#define _dac_write(x,y) NULL
#define _harm_SetOutputs(x) NULL
#endif

#ifdef __cplusplus
}
#endif

#endif /* HARMONIST_H_ */
