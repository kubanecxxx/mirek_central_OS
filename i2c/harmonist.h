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
	} s;
	uint16_t a[HARM_MODE_COUNT];
} harmonizer_mode_t;

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
	} s;
	uint16_t a[HARM_KEY_COUNT];
} harmonizer_key_t;

/*
 * HARMONY
 */
typedef union
{
	uint16_t a[HARM_SHIFT_COUNT];
} harmonizer_harmony_t;

typedef struct harmonizer_t
{
	harmonizer_mode_t MODE;
	harmonizer_key_t KEY;
	harmonizer_harmony_t HARMONY;
} harmonizer_t;

/* Exported constants --------------------------------------------------------*/
/*
 * DAC channels
 */
typedef enum
{
	CHAN_MODE = 0, CHAN_KEY = 2, CHAN_HARM = 3
} DAC_channel;

/*
 * HARMONIST PCA PINS
 */
typedef enum
{
	HARM_LDAC = 0, HARM_EFF = 1, HARM_BUT = 2, HARM_LED = 3
} HARM_pins;

/*
 * voltage constants set for Harmonizer analog inputs
 */
extern const harmonizer_t HARMONIZER;

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
/* Exported functions --------------------------------------------------------*/
void harm_init(void);
void set_harmonist(uint8_t channel, uint8_t index);

#ifdef __cplusplus
}
#endif

#endif /* HARMONIST_H_ */
