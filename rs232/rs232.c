/**
 * @file rs232.c
 * @author kubanec
 * @date 18.8.2012
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "ch.h"
#include "hal.h"
#include "rs232.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
BaseSequentialStream * marshall = (BaseSequentialStream*) &SD2;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

static const SerialConfig cfg =
{ 9600, 0, 0, 0 };

void serial_init(void)
{
	sdStart(&SD2, &cfg);
	palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
	palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));
}
