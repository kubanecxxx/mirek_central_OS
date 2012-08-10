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
#define DAC_NAME "MCP4728" //12-bit DAC
#define PCA 0b1000001 //PCA 7-bit address
#define _BV(x) (1<<x)
/* Exported macro ------------------------------------------------------------*/
/**
 * @brief output voltage to bits
 */
#define Vref 3.3
#define DAC_VOLTAGE(Vout) ((uint16_t)(Vout*4096/Vref))

/* Exported functions --------------------------------------------------------*/
void i2c1_init(void);
void set_harmonist(uint8_t channel, uint8_t index);

#ifdef __cplusplus
}
#endif

#endif /* I2C_USER_H_ */
