/**
 * @file ssd1289_port.h
 * @author kubanec
 * @date 18.7.2012
 *
 */

#ifndef SSD1289_PORT_H_
#define SSD1289_PORT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "ch.h"
#include "hal.h"
#include "ssd1289/ssd1289_lld.h"

//Public prototypes
void ssd1289_low_level_init(void);
inline void ssd1289_low_level_output_address(uint16_t address);
inline void ssd1289_low_level_output_data(uint16_t data);
void Delay_ms(uint32_t nTime);

/* Type Definitions **********************************************************/

typedef struct
{
	volatile uint16_t Register; /* LCD Index Write            Address offset 0x00 */
	volatile uint16_t Data; /* LCD Data Write             Address offset 0x02 */
} LCD_TypeDef;

/* Definitions ***************************************************************/

/*
 * FSMC Memory Bank 1: 0x60000000 to 6FFFFFFF
 * NAND bank selections:
 *   SECTION NAME      HADDR[17:16]  ADDRESS RANGE
 *   Address section   1X            0x020000-0x03FFFF
 *   Command section   01            0x010000-0x01FFFF
 *   Data section      00            0x000000-0x0FFFF
 */

#define LCD_BASE            ((uint32_t) (0x60000000 | 0x0001FFFE))
#define LCD                 ((LCD_TypeDef *) LCD_BASE)

//GPIO reset pin macros
#define CLR_RES()		palClearPad(GPIOC,6)
#define SET_RES()		palSetPad(GPIOC,6)

#ifdef __cplusplus
}
#endif

#endif /* SSD1289_PORT_H_ */
