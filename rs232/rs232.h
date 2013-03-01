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
typedef enum
{
	eff_loop_enabled, eff_loop_bypass
} eff_loop_t;

typedef struct
{
	struct
	{
		bool_t mute :2;
		eff_loop_t loop :2;
		bool_t high_sens :2;
	} s;
	uint8_t gain;
	uint8_t volume;
} serial_marshall_state_t;
/* Exported constants --------------------------------------------------------*/
extern BaseSequentialStream * marshall;
extern eff_loop_t _loop;
extern serial_marshall_state_t _marshall_state;
/* Exported macro ------------------------------------------------------------*/
/**
 * @brief výběr kanálu s gainem
 * @ingroup RS232
 * @param[in] číslo kanálu (1-4)
 */
#define SULIN //chThdSleepMilliseconds(1)
#define serial_gain(channel) \
	if (channel != 0) \
	{\
		chprintf(marshall,"gain %d\n\r\n",channel); \
		_marshall_state.gain = channel; \
	}\
	SULIN
/**
 * @brief výběr kanálu s volume
 * @ingroup RS232
 * @param[in] číslo kanálu (1-4)
 */
#define serial_volume(channel) \
	if (channel != 0) \
{ \
	chprintf(marshall,"volume %d\r\n\r\n", channel); \
	_marshall_state.volume = channel; \
}\
	SULIN

/**
 * @brief zamutuje maršála
 * @ingroup RS232
 */
#define serial_mute() \
	chprintf(marshall,"mute on\r\n\r\n");\
	_marshall_state.s.mute = TRUE; \
	SULIN
/**
 * @brief odmutuje maršála
 * @ingroup RS232
 */
#define serial_unmute() \
	chprintf(marshall,"mute off\r\n\r\n"); \
	_marshall_state.s.mute = FALSE; \
	SULIN
/**
 * @brief nastaví vstup low sens
 * @ingroup RS232
 */
#define serial_channelLow() \
	chprintf(marshall,"channel low\r\n\r\n");\
	_marshall_state.s.high_sens = FALSE; \
	SULIN

#define serial_channelLowSrat() \
		chprintf(marshall,"channel low\r\n\r\n");\
	SULIN
/**
 * @brief nastaví vstup high sens
 * @ingroup RS232
 */
#define serial_channelHigh() \
	chprintf(marshall,"channel high\r\n\r\n");\
	_marshall_state.s.high_sens = TRUE;\
	SULIN
/**
 * @brief Zapne efektovou smyčku
 * @ingroup RS232
 */
#define serial_loopOn() \
	chprintf(marshall,"loop on\r\n\r\n");\
	_marshall_state.s.loop = eff_loop_enabled;\
	SULIN
/**
 * @brief Vypne efektovou smyčku (true bypass)
 * @ingroup RS232
 */
#define serial_loopBypass() \
	chprintf(marshall,"loop off\r\n\r\n"); \
	_marshall_state.s.loop = eff_loop_bypass; \
	SULIN

#define serial_loopBypass_delay() chprintf(marshall,"loop off\r\n\r\n")
#define serial_loopOn_delay() chprintf(marshall,"loop on\r\n\r\n")

#define serial_getLoopState() (_marshall_state.s.loop)
#define serial_getHighSensState() (_marshall_state.s.high_sens)
#define serial_getMuteState() (_marshall_state.s.mute)
#define serial_getVolume() (_marshall_state.volume)
#define serial_getGain() (_marshall_state.gain)
/* Exported functions --------------------------------------------------------*/
void serial_init(void);

#ifdef __cplusplus
}
#endif

#endif /* RS232_H_ */
