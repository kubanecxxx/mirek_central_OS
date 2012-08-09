/**
 * @file i2c_user.c
 * @author kubanec
 * @date 9.8.2012
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "ch.h"
#include "hal.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define PCA 0b0100000 //PCA 7-bit address

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const I2CConfig i2c1_config =
{ .op_mode = OPMODE_I2C, .clock_speed = 40000, .duty_cycle = STD_DUTY_CYCLE };
extern I2CDriver I2CD1;

/* Private function prototypes -----------------------------------------------*/
static void i2c1_thread(void * data) __attribute__ ((noreturn));
;

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
static WORKING_AREA(wa_i2c1,128);
static void i2c1_thread(void * data)
{
	(void) data;

	uint8_t err;
	uint8_t txbuf[5];
	uint8_t rxbuf[5];

	//config PCA direction
	txbuf[0] = 0x3;
	txbuf[1] = 0xFE;
	i2cAcquireBus(&I2CD1);
	err = i2cMasterTransmitTimeout(&I2CD1, PCA, txbuf, 2, NULL, 0, TIME_INFINITE );
	i2cReleaseBus(&I2CD1);

	txbuf[0] = 1;
	while (TRUE)
	{
		txbuf[1] ^= 1;
		chThdSleepMilliseconds(200);
		i2cAcquireBus(&I2CD1);
		err = i2cMasterTransmitTimeout(&I2CD1, PCA, txbuf, 2, NULL, 0,
				TIME_INFINITE );
		i2cReleaseBus(&I2CD1);
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

/**
 * @}
 */
