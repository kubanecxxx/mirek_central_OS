/**
 * @file i2c_user.c
 * @author kubanec
 * @date 9.8.2012
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "ch.h"
#include "hal.h"
#include "i2c_user.h"
#include "harmonist.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//#define PCA 0b1000001 //PCA 7-bit address
#define DACAN 0b1100000 //DAC 7-bit address MCP4728
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const I2CConfig i2c1_config =
{ .op_mode = OPMODE_I2C, .clock_speed = 40000, .duty_cycle = STD_DUTY_CYCLE };
extern I2CDriver I2CD1;

/* Private function prototypes -----------------------------------------------*/
static void i2c1_thread(void * data) __attribute__ ((noreturn));
static void dac_write(uint8_t channel, uint16_t voltage);

/* Private functions ---------------------------------------------------------*/
/**
 * @defgroup I2C_user
 */

/**
 * @addtogroup I2C_user
 * @{
 */

/**
 * @brief I2C1 thread
 * @note
 */
static WORKING_AREA(wa_i2c1,256);
static void i2c1_thread(void * data)
{
	(void) data;

	uint8_t err;
	uint8_t txbuf[5];
	uint8_t rxbuf[5];

	//config PCA direction
	txbuf[0] = 0x3;
	txbuf[1] = 0;
	//inputs
	txbuf[1] = _BV(HARM_BUT) | _BV(HARM_LED);
	//outputs
	//txbuf[1] = _BV(HARM_EFF) | _BV(HARM_LDAC);
	i2cAcquireBus(&I2CD1);
	err = i2cMasterTransmitTimeout(&I2CD1, PCA, txbuf, 2, NULL, 0,
			TIME_INFINITE );
	//set LDAC, EFF
	txbuf[0] = 1;
	txbuf[1] = _BV(HARM_LDAC) | _BV(HARM_EFF);
	err = i2cMasterTransmitTimeout(&I2CD1, PCA, txbuf, 2, NULL, 0,
			TIME_INFINITE );
	i2cReleaseBus(&I2CD1);

	dac_write(CHAN_HARM, HARMONIZER.HARMONY.a[0]);
	dac_write(CHAN_KEY, HARMONIZER.KEY.s.A);
	dac_write(CHAN_MODE, HARMONIZER.MODE.s.MAJOR);

	txbuf[0] = 1;
	while (TRUE)
	{
		txbuf[0] = 0;
		chThdSleepMilliseconds(200);
/*
		 i2cAcquireBus(&I2CD1);
		 err = i2cMasterTransmitTimeout(&I2CD1, PCA, txbuf, 1, rxbuf, 0,
		 TIME_INFINITE );
		 i2cReleaseBus(&I2CD1);
*/
		 if (rxbuf[0] & _BV(HARM_BUT))
		 {
			 rxbuf[0] = 3;
		 }

	}
}

/**
 * @brief init i2C1 and start thread
 */
void i2c1_init(void)
{
	//pins are already initialized in board.h
	i2cStart(&I2CD1, &i2c1_config);

	chThdCreateStatic(wa_i2c1, sizeof(wa_i2c1), NORMALPRIO, i2c1_thread, NULL );
}

static void dac_write(DAC_channel channel, uint16_t voltage)
{
	uint8_t txbuf[3];
	uint8_t err;

	txbuf[0] = 0b01011000 | ((channel & 0b11) << 1);
	txbuf[1] = (voltage >> 8) & 0xF;
	txbuf[2] = voltage & 0xFF;

	i2cAcquireBus(&I2CD1);
	err = i2cMasterTransmitTimeout(&I2CD1, DACAN, txbuf, 3, NULL, 0,
			TIME_INFINITE );
	txbuf[0] = 1;
	txbuf[1] = _BV(HARM_EFF) | _BV(HARM_LDAC);
	err = i2cMasterTransmitTimeout(&I2CD1, PCA, txbuf, 2, NULL, 0,
			TIME_INFINITE );
	i2cReleaseBus(&I2CD1);
}

void set_harmonist(uint8_t channel, uint8_t index)
{
	switch (channel)
	{
	//MODE
	case 0:
		if (index < 5)
		{
			dac_write(CHAN_MODE, HARMONIZER.MODE.a[index]);
		}
		break;

		//KEY
	case 1:
		if (index < 12)
		{
			dac_write(CHAN_KEY, HARMONIZER.KEY.a[index]);
		}
		break;

		//HARMONY
	case 2:
		if (index < 11)
		{
			dac_write(CHAN_HARM, HARMONIZER.HARMONY.a[index]);
		}
		break;

	default:
		break;
	}
}

/**
 * @}
 */
