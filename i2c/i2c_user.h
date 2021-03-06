/**
 * @file i2c_user.h
 * @author kubanec
 * @date 9.8.2012
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef I2C_USER_H_
#define I2C_USER_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "footswitch.h"
#include "harmonist.h"
#include "delay.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define _BV(x) (1<<x)
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void i2c1_init(void);
#ifdef I2C_TEST
void i2c_test(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* I2C_USER_H_ */
