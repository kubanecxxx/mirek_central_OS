/**
 * @file rs232.h
 * @author kubanec
 * @date 18.8.2012
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef RS232_H_
#define RS232_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "chprintf.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
extern BaseSequentialStream * marshall;
/* Exported macro ------------------------------------------------------------*/
/**
 * @brief výběr kanálu s gainem
 * @ingroup RS232
 * @param[in] číslo kanálu (1-4)
 */
#define SULIN //chThdSleepMilliseconds(10)
#define serial_gain(channel) chprintf(marshall,"gain %d\n\r\n",channel); SULIN
/**
 * @brief výběr kanálu s volume
 * @ingroup RS232
 * @param[in] číslo kanálu (1-4)
 */
#define serial_volume(channel) chprintf(marshall,"volume %d\r\n\r\n", channel); SULIN
/**
 * @brief zamutuje maršála
 * @ingroup RS232
 */
#define serial_mute() chprintf(marshall,"mute on\r\n\r\n"); SULIN
/**
 * @brief odmutuje maršála
 * @ingroup RS232
 */
#define serial_unmute() chprintf(marshall,"mute off\r\n\r\n"); SULIN
/**
 * @brief nastaví vstup low sens
 * @ingroup RS232
 */
#define serial_channelLow() chprintf(marshall,"channel low\r\n\r\n"); SULIN
/**
 * @brief nastaví vstup high sens
 * @ingroup RS232
 */
#define serial_channelHigh() chprintf(marshall,"channel high\r\n\r\n"); SULIN
/**
 * @brief Zapne efektovou smyčku
 * @ingroup RS232
 */
#define serial_loopOn() chprintf(marshall,"loop on\r\n\r\n"); SULIN
/**
 * @brief Vypne efektovou smyčku (true bypass)
 * @ingroup RS232
 */
#define serial_loopBypass() chprintf(marshall,"loop off\r\n\r\n"); SULIN
/* Exported functions --------------------------------------------------------*/
void serial_init(void);

#ifdef __cplusplus
}
#endif

#endif /* RS232_H_ */
