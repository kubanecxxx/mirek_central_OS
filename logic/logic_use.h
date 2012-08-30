/**
 * @file logic_use.h
 * @author kubanec
 * @date 23.8.2012
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LOGIC_USE_H_
#define LOGIC_USE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "logic_types.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void logic_init(void);
void logic_specific(const logic_specific_t * arg);
void logic_marshallSetup(const logic_marshall_t * marsh);

#ifdef __cplusplus
}
#endif

#endif /* LOGIC_USE_H_ */
