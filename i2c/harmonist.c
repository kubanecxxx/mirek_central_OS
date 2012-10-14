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
#include "rs232.h"
#include "footswitch.h"
#include "logic_types.h"

/**
 * @addtogroup HARMONIST
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#ifdef I2C_HARMONIST
static WORKING_AREA(wa_harmonizer,256);
#endif
bool_t _harm_enabled;
extern logic_active_t active;

///@brief sada konstant pro DAC
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
		DAC_VOLTAGE(2.141), DAC_VOLTAGE(2.464), DAC_VOLTAGE(2.815),
		DAC_VOLTAGE(3.104), DAC_VOLTAGE(3.288) },
/*
 * HARMONY
 */
{ DAC_VOLTAGE(0), DAC_VOLTAGE(0.205), DAC_VOLTAGE(0.558), DAC_VOLTAGE(0.867),
		DAC_VOLTAGE(1.254), DAC_VOLTAGE(1.661), DAC_VOLTAGE(2.041),
		DAC_VOLTAGE(2.436), DAC_VOLTAGE(2.815), DAC_VOLTAGE(3.132),
		DAC_VOLTAGE(3.228) } };
/* Private function prototypes -----------------------------------------------*/
static void harm_thread(void * data);
/* Private functions ---------------------------------------------------------*/

#ifdef I2C_HARMONIST
/**
 * @brief thread for watching harmozier status
 * @notapi
 */
static void harm_thread(void * data)
{

	(void) data;
	uint8_t inputs;
	chRegSetThreadName("harmonist");
	eff_loop_t prev;
	uint32_t time1,time2;

	while (TRUE)
	{
		inputs = harm_getInputs();

		//if odněkud jesli má byt zapnuto nebo vypnuto + co řiká ledka + mód
		//todo broadcast pokud se zmačklo tlačitko
		/*
		 * pokud se zmačkne tlačitko tak se nastavi nějak
		 * a zmačkne efekt, až se pusti
		 * tak se aji efekt pusti
		 */
		if (harm_getInput_BUT(inputs))
		{
			time1 = chTimeNow();
			/*
			 * bend
			 */
			harm_volume(active.bank->bend.volume);
			harm_mode(4);
			harm_key(active.bank->bend.key);
			harm_harmony(active.bank->bend.harmony);

			prev = serial_getLoopState();

			if (prev == eff_loop_bypass)
			{
				serial_loopOn()
				;
			}

			while (harm_getInput_BUT(inputs))
			{

				inputs = harm_getInputs();
				harm_pushButton(inputs);
				time2 = chTimeNow() - time1;
				chThdSleepMilliseconds(50);
			}
			harm_releaseButton(inputs);
			if (prev == eff_loop_bypass)
			{
				chThdSleepMilliseconds(1000);
				serial_loopBypass()
				;
			}
		}
		else if (_harm_enabled != harm_getInput_LED(inputs))
		{
			harm_pushButton(inputs);
			chThdSleepMilliseconds(20);
			harm_releaseButton(inputs);
		}
		chThdSleepMilliseconds(50);
	}
}

/**
 * @brief set PCA outputs
 * @param[in] bitmask
 * @notapi
 */
void _harm_SetOutputs(uint8_t data)
{
	uint8_t txbuf[2];

	txbuf[0] = PCA_ODR;
	txbuf[1] = data;

	i2cAcquireBus(&I2CD1);
	i2cMasterTransmit(&I2CD1, HARM_PCA, txbuf, 2, NULL, 0);
	i2cReleaseBus(&I2CD1);
}

/**
 * @brief write harmonist DAC value
 * @notapi
 */
void _dac_write(DAC_channel channel, uint16_t voltage)
{
	uint8_t txbuf[3];
	uint8_t err;
	uint8_t inputs = harm_getInputs();
	_harm_SetOutputs(inputs & ~_BV(HARM_LDAC));

	chThdSleepMilliseconds(20);

	txbuf[0] = 0b01011000 | ((channel & 0b11) << 1);
	txbuf[1] = (voltage >> 8) & 0xF;
	txbuf[2] = voltage & 0xFF;

	i2cAcquireBus(&I2CD1);
	err = i2cMasterTransmitTimeout(&I2CD1, DACAN, txbuf, 3, NULL, 0,
			TIME_INFINITE );
	i2cReleaseBus(&I2CD1);

	chThdSleepMilliseconds(20);

	harm_setLDAC(inputs);
}

/**
 * @brief get PCA expander inputs
 * @return IDR of PCA in harmonizer
 */
uint8_t harm_getInputs(void)
{
	uint8_t txbuf[2];
	uint8_t rxbuf[2];

	txbuf[0] = PCA_IDR;

	i2cAcquireBus(&I2CD1);
	i2cMasterTransmit(&I2CD1, HARM_PCA, txbuf, 1, rxbuf, 2);
	i2cReleaseBus(&I2CD1);

	return (rxbuf[0] & 0x0F);
}

/**
 * @brief setup harmonist default values and PCA setup
 */
void harm_init(void)
{
	uint8_t txbuf[5];
	uint8_t err;

	/*
	 * DAC has own eeprom - memorize last state itself
	 harm_harmony(HARMONIZER.HARMONY.a[0]);
	 harm_key(HARMONIZER.KEY.s.A);
	 harm_mode(HARMONIZER.MODE.s.MAJOR);
	 harm_volume(1000);
	 */

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

	chThdCreateStatic(&wa_harmonizer, sizeof(wa_harmonizer), NORMALPRIO,
			(tfunc_t) harm_thread, NULL );
}
/**
 * @}
 */
#endif
