/**
 * @file delay.c
 * @author kubanec
 * @date 16.8.2012
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "ch.h"
#include "hal.h"
#include "delay.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#ifdef I2C_DD3
/**
 * @brief turn delay on/off - internal function shouldn't be used by user
 */
void _delay_onOff(uint8_t data)
{
	uint8_t txbuf[2];

	txbuf[0] = PCA_LS0;
	txbuf[1] = data;
	i2cAcquireBus(&I2CD1);
	i2cMasterTransmit(&I2CD1, DELAY_PCA, txbuf, 2, NULL, 0);
	i2cReleaseBus(&I2CD1);
}

bool_t delay_get(void)
{
	uint8_t txbuf[2];
	uint8_t rxbuf[2];

	txbuf[0] = PCA_INPUT_REG;

	i2cAcquireBus(&I2CD1);
	i2cMasterTransmit(&I2CD1, DELAY_PCA, txbuf, 1, rxbuf, 2);
	i2cReleaseBus(&I2CD1);

	if ((rxbuf[0] & 1) == 1)
		return TRUE;
	else
		return FALSE;
}

void _delay_dalas(uint8_t channel, uint8_t value)
{
	uint8_t txbuf[2];
	bool_t prev;

	if (channel == DAL_POTVOLUME)
	{
		prev = delay_get();
		delay_off();
		chThdSleepMilliseconds(100);
	}

	txbuf[0] = channel;
	txbuf[1] = value;
	i2cAcquireBus(&I2CD1);
	i2cMasterTransmit(&I2CD1, DELAY_DAL, txbuf, 2, NULL, 0);
	i2cReleaseBus(&I2CD1);

	if (prev == TRUE && channel == DAL_POTVOLUME)
	{
		chThdSleepMilliseconds(150);
		delay_on();
	}
}

#endif
