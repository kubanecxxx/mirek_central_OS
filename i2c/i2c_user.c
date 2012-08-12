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

static WORKING_AREA(wa_i2c1,256);
/**
 * @brief I2C1 thread
 * @note
 */
static void i2c1_thread(void * data)
{
	(void) data;
	/*
	 * fill DAC with default values and PCA setup
	 */
	chRegSetThreadName("i2c");
	harm_init();
	/*
	 * setup footswitch PCAs
	 */
	foot_init();

	while (TRUE)
	{
		chThdSleepMilliseconds(200);
	}
}

/**
 * @brief init i2C1 and start thread
 */
void i2c1_init(void)
{
	//pins are already initialized in board.h
	i2cStart(&I2CD1, &i2c1_config);

	chThdCreateStatic(wa_i2c1, sizeof(wa_i2c1), NORMALPRIO,
			(tfunc_t) i2c1_thread, NULL );
}

/**
 * @}
 */
