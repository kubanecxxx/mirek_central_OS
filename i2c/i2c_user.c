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

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const I2CConfig i2c1_config =
{ .op_mode = OPMODE_I2C, .clock_speed = 40000, .duty_cycle = STD_DUTY_CYCLE };
extern I2CDriver I2CD1;

/* Private function prototypes -----------------------------------------------*/
#ifdef I2C_TEST
static WORKING_AREA(wa_i2c_test, 256);
static void i2c_test_thread(void * data) __attribute__ ((noreturn));
#endif
/* Private functions ---------------------------------------------------------*/
/**
 * @defgroup I2C
 */

/**
 * @addtogroup I2C
 * @{
 */

/**
 * @brief init i2C1 and init I2C devices
 */
void i2c1_init(void)
{
	//pins are already initialized in board.h
	i2cStart(&I2CD1, &i2c1_config);

	/*
	 * fill DAC with default values and PCA setup
	 */
	harm_init();
	/*
	 * setup footswitch PCAs
	 */
	foot_init();
	/*
	 * DD3 doesn't need any init
	 */
}

#ifdef I2C_TEST
void i2c_test(void)
{
	chThdCreateStatic(&wa_i2c_test, sizeof(wa_i2c_test), NORMALPRIO,
			(tfunc_t) i2c_test_thread, NULL );
}

static void i2c_test_thread(void * data)
{
#ifdef I2C_FOOTSWITCH
	(void) data;

	uint8_t i = 1;
	uint8_t j = 0;

	chRegSetThreadName("i2c footswitch test");

	/*
	 * test PCA footswitch leds
	 */
	i = 1;
	chThdSleepMilliseconds(100);
	foot_SetLedsBoth(0, 0);
	for (j = 0; j < 8; j++)
	{
		chThdSleepMilliseconds(100);
		foot_SetLedsYellow(i);
		i <<= 1;
	}
	chThdSleepMilliseconds(100);
	foot_SetLedsBoth(0, 0);
	i = 1;
	for (j = 0; j < 8; j++)
	{
		chThdSleepMilliseconds(100);
		foot_SetLedsGreen(i);
		i <<= 1;
	}
	chThdSleepMilliseconds(100);
	foot_SetLedsBoth(0, 0);
	i = 1;
	for (j = 0; j < 8; j++)
	{
		chThdSleepMilliseconds(100);
		foot_SetLedsBoth(i, i);
		i <<= 1;
	}

	EventListener el;
	chEvtRegisterMask(&event_i2c_buttons, &el, BUTTON_EVENT_ID);

	uint8_t errors = 0;

	while (TRUE)
	{
		chEvtWaitOne(BUTTON_EVENT_ID);
		switch (foot_switch.count)
		{
			case 0:
			foot_SetLedsBoth(0, 0);
			break;
			case 1:
			foot_SetLedsBoth(0, foot_switch.pin);
			break;
			case 2:
			foot_SetLedsBoth(foot_switch.pin, 0);
			break;
			case 3:
			foot_SetLedsBoth(foot_switch.pin, foot_switch.pin);
			break;
			default:
			errors++;
			break;
		}
	}
#endif //I2C footswitch
}
#endif

/**
 * @}
 */
