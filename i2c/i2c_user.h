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
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define _BV(x) (1<<x)
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void i2c1_init(void);


#ifdef __cplusplus
}
#endif

#endif /* I2C_USER_H_ */
