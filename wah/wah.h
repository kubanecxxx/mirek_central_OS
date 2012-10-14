/**
 * @file wah.h
 * @author kubanec
 * @date 13.10.2012
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef WAH_H_
#define WAH_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void wah_init(void);
inline bool_t wah_isEnabled(void);
inline void wah_event(void);

#ifdef __cplusplus
}
#endif

#endif /* WAH_H_ */
