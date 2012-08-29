/**
 * @file chsprintf.h
 * @author kubanec
 * @date 29.8.2012
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CHSPRINTF_H_
#define CHSPRINTF_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void chsprintf(char * str, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* CHSPRINTF_H_ */
