/**
 * @file delay.h
 * @author kubanec
 * @date 16.8.2012
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DELAY_H_
#define DELAY_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/*
 * I2C address set for delay DD3 - PCA9530 + DS18030
 */
#define DELAY_PCA 0b1100000 //Harmnoist PCA 7-bit address
#define DELAY_DAL 0b0101000 //DAC 7-bit address MCP4728
/*
 * PCA registers
 */
#define PCA_INPUT_REG 0
#define PCA_PSC0 1
#define PCA_PWM0 2
#define PCA_PSC1 3
#define PCA_PWM1 4
#define PCA_LS0  5

#define PCA_INCREMENT 0b00010000

/*
 * DALAS registers
 */
#define DAL_POT0 0b10101001
#define DAL_POT1 0b10101010
#define DAL_POT_BOTH 0b10101111

/* Exported macro ------------------------------------------------------------*/
#define delay_on() _delay_onOff(0)
#define delay_off() _delay_onOff(1)

#define delay_volume(x) _delay_dalas(DAL_POT0,x)
#define delay_time(x) _delay_dalas(DAL_POT1,x)

/* Exported functions --------------------------------------------------------*/
#ifdef I2C_DD3
void _delay_onOff (uint8_t data);
void _delay_dalas(uint8_t channel, uint8_t value);
#else
#define _delay_onOff(d) NULL
#define _delay_dalas(d,value) NULL
#endif

#ifdef __cplusplus
}
#endif

#endif /* DELAY_H_ */
