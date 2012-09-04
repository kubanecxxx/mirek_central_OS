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

/**
 * @defgroup RS232
 *
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/**
 * @brief stream pro posilání dat do maršála
 * @notapi
 * @ingroup RS232
 */

BaseSequentialStream * marshall = (BaseSequentialStream*) &SD2;
eff_loop_t _loop;
serial_marshall_state_t _marshall_state;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
 * @brief nastavení UARTU
 * @notapi
 * @ingroup RS232
 */
static const SerialConfig cfg =
{ 9600, 0, 0, 0 };

/**
 * @brief inicializace UART2
 * @ingroup RS232
 */
void serial_init(void)
{
	sdStart(&SD2, &cfg);
	palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
	palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));
}
