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
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
extern BaseSequentialStream * marshall;
/* Exported macro ------------------------------------------------------------*/
/**
 * gain and volume, channels are 1,2,3,4
 */
#define serial_gain(channel) chprintf(marshall,"gain %d",channel)
#define serial_volume(channel) chprintf(marshall,"volume %d", channel)
//mute
#define serial_mute() chprintf(marshall,"mute on")
#define serial_unmute() chprintf(marshall,"mute off")
//channel
#define serial_channelLow() chprintf(marshall,"channel low")
#define serial_channelHigh() chprintf(marshall,"channel high")
//effect loop
#define serial_loopOn() chrpintf(marshall,"loop on")
#define serial_loopBypass() chprintf(marshall,"loop off")
/* Exported functions --------------------------------------------------------*/
void serial_init(void);

#ifdef __cplusplus
}
#endif

#endif /* RS232_H_ */
