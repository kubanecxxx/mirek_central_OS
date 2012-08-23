/**
 * @file logic_use.c
 * @author kubanec
 * @date 23.8.2012
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "ch.h"
#include "hal.h"
#include "logic_types.h"
#include "logic_use.h"
#include "switch_master.h"
#include "i2c_user.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void logic_specific(const logic_specific_t * arg);

/* Private functions ---------------------------------------------------------*/
/**
 * @brief provede co je potřeba pro funkci
 * @note
 * posledních 8 bitů je pro I2C a opto
 * zbytek jsou efekty pro relé
 *
 * @param[in] function setup
 * @ingroup LOGIC_HL
 *
 */
void logic_function(const logic_function_t * arg)
{
	//I2C effects 28 - harmonist ,29 - delay
	uint64_t temp = arg->effects.w;
	logic_dibit_t temp_i2c = arg->effects;
	uint8_t temp_opto = (temp >> 60) & 0b1111;
	temp &= 0x00FFFFFFFFFFFFFF;

	logic_specific(arg->special);

	//relays
	uint8_t i;
	logic_effect_t tmp;
	for (i = 0; i < 28; i++)
	{
		tmp = temp & 0b11;
		temp >>= 2;

		if (tmp == EFF_ENABLE)
			switch_setRelay(i);
		else if (tmp == EFF_DISABLE)
			switch_clearRelay(i);
		else if (tmp == EFF_TOGGLE)
			switch_toggleRelay(i);
	}

	//opto
	for (i = 0; i < 2; i++)
	{
		tmp = temp_opto & 0b11;
		temp_opto >>= 2;

		if (tmp == EFF_ENABLE)
			opto_enableEffect(i);
		else if (tmp == EFF_DISABLE)
			opto_disableEffect(i);
		else if (tmp == EFF_TOGGLE)
			opto_toggleEffect(i);
	}

	//harmonist
	if (temp_i2c.s.eff28 == EFF_ENABLE)
		harm_enable();
	else if (temp_i2c.s.eff28 == EFF_DISABLE)
		harm_disable();
	else if (temp_i2c.s.eff28 == EFF_TOGGLE)
		harm_toggle();

	//delay
	if (temp_i2c.s.eff29 == EFF_ENABLE)
		delay_on();
	else if (temp_i2c.s.eff29 == EFF_DISABLE)
		delay_off();
	else if (temp_i2c.s.eff29 == EFF_TOGGLE)
		delay_toggle();
}

/**
 * @brief provede co je potřeba pro kanál
 * @note
 * poslední 4 bity jsou pro I2C a opto
 * zbytek jsou efekty pro relé
 *
 * @param[in] channel setup
 * @ingroup LOGIC_HL
 */
void logic_channel(const logic_channel_t * arg)
{
	uint32_t temp = arg->effects.w;
	//I2C bits 28 - harmonist ,29 - delay
	logic_bit_t i2c_temp = arg->effects;
	//opto bits 30,31
	uint8_t temp_opto = (temp >> 30) & 0b11;
	temp &= 0x0FFFFFFF;

	logic_specific(arg->special);

	//relays
	switch_setRelays(temp);

	//optocouplers
	opto_setEffects(temp_opto);

	//harmonist
	if (i2c_temp.s.bit28)
		harm_enable();
	else
		harm_disable();

	//delay
	if (i2c_temp.s.bit29)
		delay_on();
	else
		delay_off();
}

void logic_remap(const logic_remap_t * arg)
{

}

static void logic_specific(const logic_specific_t * arg)
{
	if (arg == NULL )
		return;
}

void logic_button(const logic_button_t * arg)
{
	//tohle bude volat funkce above
	//volání bude probihat někde v samostatnym vlákně
}
