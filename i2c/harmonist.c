/**
 * @file harmonist.c
 * @author kubanec
 * @date 10.8.2012
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "ch.h"
#include "harmonist.h"
#include "i2c_user.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const harmonizer_t HARMONIZER =
{
	/*
	 * MODEs
	 */
	{
		DAC_VOLTAGE(0),
		DAC_VOLTAGE(0.151),
		DAC_VOLTAGE(0.494),
		DAC_VOLTAGE(0.839),
		DAC_VOLTAGE(1.201)
	},
	/*
	 * KEY
	 */
	{
		DAC_VOLTAGE(0),
		DAC_VOLTAGE(0.181),
		DAC_VOLTAGE(0.493),
		DAC_VOLTAGE(0.810),
		DAC_VOLTAGE(1.129),
		DAC_VOLTAGE(1.462),
		DAC_VOLTAGE(1.810),
		DAC_VOLTAGE(2.141),
		DAC_VOLTAGE(2.464),
		DAC_VOLTAGE(2.786),
		DAC_VOLTAGE(3.104),
		DAC_VOLTAGE(3.288)
	},
	/*
	 * HARMONY
	 */
	{
		DAC_VOLTAGE(0),
		DAC_VOLTAGE(0.205),
		DAC_VOLTAGE(0.558),
		DAC_VOLTAGE(0.867),
		DAC_VOLTAGE(1.254),
		DAC_VOLTAGE(1.661),
		DAC_VOLTAGE(2.041),
		DAC_VOLTAGE(2.436),
		DAC_VOLTAGE(2.765),
		DAC_VOLTAGE(3.132),
		DAC_VOLTAGE(3.228)
	}
};
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


