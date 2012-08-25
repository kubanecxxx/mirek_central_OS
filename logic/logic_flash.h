/**
 * @file logic_flash.h
 * @author kubanec
 * @date 25.8.2012
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LOGIC_FLASH_H_
#define LOGIC_FLASH_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Base address of the Flash sectors */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */

#define FLASH_ADDRESS_START ADDR_FLASH_SECTOR_8
#define FLASH_ADDRESS_STOP ADDR_FLASH_SECTOR_9

#define FLASH_MACRO(offset) 	(FLASH_ADDRESS_START | offset)
#define FLASH_NAME_ADDRESS		FLASH_MACRO(0x0000)
#define FLASH_BASE_ADDRESS 		FLASH_MACRO(0x1000)
#define FLASH_BANK_ADDRESS		FLASH_MACRO(0x2000)
#define FLASH_CHANNEL_ADDRESS 	FLASH_MACRO(0x3000)
#define FLASH_FUNCTION_ADDRESS 	FLASH_MACRO(0x4000)
#define FLASH_SPECIAL_ADDRESS 	FLASH_MACRO(0x5000)

#define FLASH_BUTTON_ADDRESS 	FLASH_MACRO(0x6000)
#define FLASH_REMAP_ADDRESS 	FLASH_MACRO(0x7000)
#define FLASH_CALLS_ADDRESS 	FLASH_MACRO(0x8000)

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void * logic_flashWrite(uint32_t * Address, void * datas, uint32_t size);
void logic_flashErase(uint32_t start, uint32_t stop);
void * logic_flashWriteS(uint32_t * Address, void * datas, uint32_t size);
void logic_flashEraseS(uint32_t start, uint32_t stop);
uint8_t logic_flashGetSector(uint32_t Address);

char * logic_flashWriteName(char * name);
logic_remap_t * logic_flashWriteRemap(logic_remap_t * rem);
logic_function_t * logic_flashWriteFunction(logic_function_t * func);
logic_channel_t * logic_flashWriteChannel(logic_channel_t * chan);
logic_specific_t * logic_flashWriteSpecial(logic_specific_t * spec);
logic_bank_t * logic_flashWriteBank(logic_bank_t * bank);

logic_function_t * logic_flashWriteAllFunctions(logic_bank_t * bank);
logic_channel_t * logic_flashWriteAllChannels(logic_bank_t * bank);
logic_base_t * logic_flashWriteBase(logic_base_t * base);

#ifdef __cplusplus
}
#endif

#endif /* LOGIC_FLASH_H_ */
