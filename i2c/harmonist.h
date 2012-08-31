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

/**
 * @defgroup HARMONIST
 * @brief API pro ovládání harmonizéru po I2C
 * @ingroup I2C
 */

/**
 * @addtogroup HARMONIST
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "i2c_user.h"
/* Exported types ------------------------------------------------------------*/
/**
 * @brief počet poloh MODE
 */
#define HARM_MODE_COUNT 5

///@brief Počet poloh KEY
#define HARM_KEY_COUNT 12
///@brief Počet poloh SHIFT
#define HARM_SHIFT_COUNT 11

/**
 * @brief MODE konstanty pro D/A převodnik
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

/**
 * @brief KEY konstanty pro D/A převodnik
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

/**
 * @brief HARMONY konstanty pro D/A převodnik
 */
typedef union
{
	uint16_t a[HARM_SHIFT_COUNT];
}harmonizer_harmony_t;

/**
 * @brief sada všech konstant
 */
typedef struct harmonizer_t
{
	harmonizer_mode_t MODE;
	harmonizer_key_t KEY;
	harmonizer_harmony_t HARMONY;
}harmonizer_t;

/* Exported constants --------------------------------------------------------*/
/**
 * @brief DAC channels
 */
typedef enum
{
	CHAN_MODE = 0, CHAN_VOLUME = 1, CHAN_KEY = 2, CHAN_HARM = 3
}DAC_channel;

/**
 * @brief HARMONIST PCA PINS
 */
typedef enum
{
	HARM_LDAC = 0, HARM_EFF = 1, HARM_BUT = 2, HARM_LED = 3
}HARM_pins;

/**
 * @brief voltage constants set for Harmonizer analog inputs
 */
extern const harmonizer_t HARMONIZER;
/**
 * @brief interní stav harmonizéru (on/off)
 * @details
 * extern jenom kvuli makrum na ovládání
 * @notapi
 */
extern bool_t _harm_enabled;

/**
 *  @brief I2C address set for harmonizer - PCA9536
 */
#define HARM_PCA 0b1000001 //Harmnoist PCA 7-bit address
/**
 * @brief  I2C address for DAC MCP4728
 */
#define DACAN 0b1100000 //DAC 7-bit address MCP4728
/**
 * @brief referenčí napětí DAC
 */
#define Vref 3.3
/**
 * @brief output voltage to bits DAC MCP4728
 * @param[in] chtěné výstupní napětí ve floatu
 */
#define DAC_VOLTAGE(Vout) ((uint16_t)(Vout*4096/Vref))

/* Exported macro ------------------------------------------------------------*/
/*
 * DAC channels
 */
///@brief nastavení hlasitosi harmonizéru
#define harm_volume(vol)  _dac_write(CHAN_VOLUME,vol)
///@brief výběr módu harmonizéru
#define harm_mode(x)  _dac_write(CHAN_MODE,HARMONIZER.MODE.a[x])
///@brief výběr key
#define harm_key(x)   _dac_write(CHAN_KEY,HARMONIZER.KEY.a[x])
///@brief výběr harmony
#define harm_harmony(x) _dac_write(CHAN_HARM,HARMONIZER.HARMONY.a[x])

///@brief výběr hlasitosti + odpověď když se něco pojebe
#define harm_volumeR(vol,response) vol < 4096 ? vol : response
///@brief výběr módu + odpověď když se něco pojebe
#define harm_modeR(x,response) x < HARM_MODE_COUNT ? x : response
///@brief výběr key + odpověď když se něco pojebe
#define harm_keyR(x,response)  x < HARM_KEY_COUNT ? x :response
///@brief výběr harmony + odpověď když se něco pojebe
#define harm_harmonyR(x,response) x < HARM_SHIFT_COUNT ? x :response

///@brief PCA input LDAC
#define harm_getInput_LDAC(inputs) ((inputs >> HARM_LDAC) & 1)
///@brief PCA input EFF
#define harm_getInput_EFF(inputs) ((inputs >> HARM_EFF) & 1)
///@brief PCA input BUT
#define harm_getInput_BUT(inputs) (!((inputs >> HARM_BUT) & 1))
///@brief PCA input LED
#define harm_getInput_LED(inputs) ((inputs >> HARM_LED) & 1)

/*
 * PCA outputs
 */
///@brief PCA release footswitch
///@note parametr je kvůli tomu aby se nemuselo číst víckrát posobě
#define harm_releaseButton(inputs) _harm_SetOutputs(inputs | _BV(HARM_EFF))
///@brief PCA push footswitch
///@note parametr je kvůli tomu aby se nemuselo číst víckrát posobě
#define harm_pushButton(inputs) _harm_SetOutputs(inputs & (~_BV(HARM_EFF)))
///@brief PCA set LDAC
///@note parametr je kvůli tomu aby se nemuselo číst víckrát posobě
#define harm_setLDAC(inputs) _harm_SetOutputs(inputs | _BV(HARM_LDAC))

///@brief enable harmonizer via internal variable
#define harm_enable() _harm_enabled = TRUE
///@brief disable harmonizer via internal variable
#define harm_disable() _harm_enabled = FALSE
///@brief toggle harmonizer via internal variable
#define harm_toggle() _harm_enabled = !_harm_enabled

/* Exported functions --------------------------------------------------------*/

/**
 * @}
 */

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
