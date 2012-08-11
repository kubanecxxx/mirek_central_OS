/**
 * @file harmonist.c
 * @author kubanec
 * @date 10.8.2012
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "ch.h"
#include "hal.h"
#include "harmonist.h"
#include "i2c_user.h"
#include "footswitch.h"

//@todo přihodit vlákno na kontrolu zapnutí nebo vypnutí podle nastavení někde v paměti
//@todo přihodit vlákno na vyčitání zmačknuti harmonizéru
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const harmonizer_t HARMONIZER =
{
/*
 * MODEs
 */
{ DAC_VOLTAGE(0), DAC_VOLTAGE(0.151), DAC_VOLTAGE(0.494), DAC_VOLTAGE(0.839),
		DAC_VOLTAGE(1.201) },
/*
 * KEY
 */
{ DAC_VOLTAGE(0), DAC_VOLTAGE(0.181), DAC_VOLTAGE(0.493), DAC_VOLTAGE(0.810),
		DAC_VOLTAGE(1.129), DAC_VOLTAGE(1.462), DAC_VOLTAGE(1.810),
		DAC_VOLTAGE(2.141), DAC_VOLTAGE(2.464), DAC_VOLTAGE(2.786),
		DAC_VOLTAGE(3.104), DAC_VOLTAGE(3.288) },
/*
 * HARMONY
 */
{ DAC_VOLTAGE(0), DAC_VOLTAGE(0.205), DAC_VOLTAGE(0.558), DAC_VOLTAGE(0.867),
		DAC_VOLTAGE(1.254), DAC_VOLTAGE(1.661), DAC_VOLTAGE(2.041),
		DAC_VOLTAGE(2.436), DAC_VOLTAGE(2.765), DAC_VOLTAGE(3.132),
		DAC_VOLTAGE(3.228) } };
/* Private function prototypes -----------------------------------------------*/
static void dac_write(DAC_channel channel, uint16_t voltage);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief write harmonist DAC value
 */
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
	err = i2cMasterTransmitTimeout(&I2CD1, HARM_PCA, txbuf, 2, NULL, 0,
			TIME_INFINITE );
	i2cReleaseBus(&I2CD1);
}

/**
 * @brief setup harmonist default values and PCA setup
 */
void harm_init(void)
{
	uint8_t txbuf[5];
	uint8_t err;

	dac_write(CHAN_HARM, HARMONIZER.HARMONY.a[0]);
	dac_write(CHAN_KEY, HARMONIZER.KEY.s.A);
	dac_write(CHAN_MODE, HARMONIZER.MODE.s.MAJOR);

	//config PCA Harmonist direction
	txbuf[0] = PCA_DDR;
	txbuf[1] = 0;
	//inputs
	txbuf[1] = _BV(HARM_BUT) | _BV(HARM_LED);
	//outputs
	//txbuf[1] = _BV(HARM_EFF) | _BV(HARM_LDAC);
	i2cAcquireBus(&I2CD1);
	err = i2cMasterTransmitTimeout(&I2CD1, HARM_PCA, txbuf, 2, NULL, 0,
			TIME_INFINITE );
	//set LDAC, EFF
	txbuf[0] = PCA_ODR;
	txbuf[1] = _BV(HARM_LDAC) | _BV(HARM_EFF);
	err = i2cMasterTransmitTimeout(&I2CD1, HARM_PCA, txbuf, 2, NULL, 0,
			TIME_INFINITE );
	i2cReleaseBus(&I2CD1);
}

/**
 * @brief test function from shell, set voltage accords channel
 */
void set_harmonist(uint8_t channel, uint8_t index)
{
	switch (channel)
	{
	//MODE
	case 0:
		if (index < HARM_MODE_COUNT)
		{
			dac_write(CHAN_MODE, HARMONIZER.MODE.a[index]);
		}
		break;

		//KEY
	case 1:
		if (index < HARM_KEY_COUNT)
		{
			dac_write(CHAN_KEY, HARMONIZER.KEY.a[index]);
		}
		break;

		//HARMONY
	case 2:
		if (index < HARM_SHIFT_COUNT)
		{
			dac_write(CHAN_HARM, HARMONIZER.HARMONY.a[index]);
		}
		break;

	default:
		break;
	}
}
