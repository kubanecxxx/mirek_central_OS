/**
 * @file logic_flash.c
 * @author kubanec
 * @date 25.8.2012
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "ch.h"
#include "hal.h"
#include "logic_types.h"
#include "logic_flash.h"
#include "stm32f4xx_flash.h"
#include "string.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint32_t LastAddress;
static uint32_t EndAddress;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @defgroup logic_flash
 * @ingroup logic_shell
 */

/**
 * @defgroup low_level_flash
 * @ingroup logic_flash
 */

/**
 * @defgroup logic_flash_bank
 * @ingroup logic_flash
 */

/**
 * @defgroup logic_channels
 * @ingroup logic_flash_bank
 * @defgroup logic_functions
 * @ingroup logic_flash_bank
 * @defgroup logic_remaps
 * @ingroup logic_flash_bank
 * @defgroup logic_buttons
 * @ingroup logic_flash_bank
 * @defgroup logic_common
 * @ingroup logic_flash_bank
 */

/**
 * @ingroup logic_common
 *
 */
char * logic_flashWriteName(char * name)
{
	uint8_t len = strlen(name);
	char buffer[20];

	strcpy (buffer,name);
	return logic_flashWrite(LastAddress, buffer, len + 1);
}

/**
 * @ingroup logic_common
 */
logic_specific_t * logic_flashWriteSpecial(logic_specific_t * spec)
{
	if (spec != NULL )
		return logic_flashWrite(LastAddress, spec, sizeof(logic_specific_t));
	else
		return NULL ;
}

/**
 * @ingroup logic_channels
 */
logic_channel_t * logic_flashWriteChannel(logic_channel_t * chan)
{
	return logic_flashWrite(LastAddress, chan, sizeof(logic_channel_t));
}

/**
 * @ingroup logic_functions
 */
logic_function_t * logic_flashWriteFunction(logic_function_t * func)
{
	return logic_flashWrite(LastAddress, func, sizeof(logic_function_t));
}

/**
 * @ingroup logic_remaps
 */
logic_remap_t * logic_flashWriteRemap(logic_remap_t * rem)
{
	return logic_flashWrite(LastAddress, rem, sizeof(logic_remap_t));
}

/**
 * @ingroup logic_channels
 * @returns pointer of first channel in flash
 */
logic_channel_t * logic_flashWriteAllChannels(logic_bank_t * bank)
{
	uint8_t cunt = bank->channelCount;
	uint8_t i;
	logic_channel_t * chan, *first;

	for (i = 0; i < cunt; i++)
	{
		chan = &bank->channels[i];

		chan->special = logic_flashWriteSpecial(chan->special);
		chan = logic_flashWriteChannel(chan);

		if (i == 0)
			first = chan;
	}

	bank->channels = first;
	return bank->channels;
}

/**
 * @ingroup logic_functions
 * @returns pointer of the first functoin in flash
 */
logic_function_t * logic_flashWriteAllFunctions(logic_bank_t * bank)
{
	uint8_t cunt = bank->functionCount;
	uint8_t i;
	logic_function_t * func, *first;

	for (i = 0; i < cunt; i++)
	{
		func = &bank->functions[i];

		func->special = logic_flashWriteSpecial(func->special);
		func = logic_flashWriteFunction(func);

		if (i == 0)
			first = func;
	}

	bank->functions = first;
	return bank->functions;
}

/**
 * @ingroup logic_remaps
 */
logic_remap_t * logic_flashWriteAllRemaps(logic_bank_t * bank)
{

}

/**
 * @ingroup logic_flash_bank
 */
logic_bank_t * logic_flashWriteBank(logic_bank_t * bank)
{
	logic_flashWriteAllChannels(bank);
	logic_flashWriteAllFunctions(bank);
	logic_flashWriteAllRemaps(bank);
}

/**
 * @addtogroup low_level_flash
 * @{
 */

void * logic_flashWrite(uint32_t Address, void * datas, uint32_t size)
{
	void * temp;
	chSysLock()
	;
	temp = logic_flashWriteS(Address, datas, size);
	chSysUnlock()
	;

	return temp;
}

void logic_flashErase(uint32_t start, uint32_t stop)
{
	chSysLock()
	;
	logic_flashEraseS(start, stop);
	chSysUnlock()
	;
}

void * logic_flashWriteS(uint32_t Address, void * datas, uint32_t size)
{
	uint32_t * data = (uint32_t *) datas;
	uint32_t temp = Address;

	FLASH_Unlock();
	while (size % 4 != 0)
		size++;

	size /= 4;
	while (size--)
	{
		if (FLASH_ProgramWord(Address, *(data++)) == FLASH_COMPLETE)
		{
			Address += 4;
		}
		else
		{
			/* Error occurred while writing data in Flash memory.
			 User can add here some code to deal with this error */
			while (1)
			{
			}
		}
	}
	LastAddress = Address;
	FLASH_Lock();

	return (void *) temp;
}

void logic_flashEraseS(uint32_t start, uint32_t stop)
{
	FLASH_Unlock();
	FLASH_ClearFlag(
			FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR
					| FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR
					| FLASH_FLAG_PGSERR );

	/* Get the number of the start and end sectors */
	uint32_t StartSector = logic_flashGetSector(start);
	uint32_t EndSector = logic_flashGetSector(stop);
	uint32_t SectorCounter;

	for (SectorCounter = StartSector; SectorCounter < EndSector;
			SectorCounter += 8)
	{
		/* Device voltage range supposed to be [2.7V to 3.6V], the operation will
		 be done by word */
		if (FLASH_EraseSector(SectorCounter, VoltageRange_3 ) != FLASH_COMPLETE)
		{
			/* Error occurred while sector erase.
			 User can add here some code to deal with this error  */
			while (1)
			{
			}
		}
	}
	LastAddress = start;
	EndAddress = stop;
	FLASH_Lock();
}

uint8_t logic_flashGetSector(uint32_t Address)
{
	uint32_t sector = 0;

	if ((Address < ADDR_FLASH_SECTOR_1 )&& (Address >= ADDR_FLASH_SECTOR_0)){
	sector = FLASH_Sector_0;
}
else if ((Address < ADDR_FLASH_SECTOR_2)
		&& (Address >= ADDR_FLASH_SECTOR_1))
{
	sector = FLASH_Sector_1;
}
else if ((Address < ADDR_FLASH_SECTOR_3)
		&& (Address >= ADDR_FLASH_SECTOR_2))
{
	sector = FLASH_Sector_2;
}
else if ((Address < ADDR_FLASH_SECTOR_4)
		&& (Address >= ADDR_FLASH_SECTOR_3))
{
	sector = FLASH_Sector_3;
}
else if ((Address < ADDR_FLASH_SECTOR_5)
		&& (Address >= ADDR_FLASH_SECTOR_4))
{
	sector = FLASH_Sector_4;
}
else if ((Address < ADDR_FLASH_SECTOR_6)
		&& (Address >= ADDR_FLASH_SECTOR_5))
{
	sector = FLASH_Sector_5;
}
else if ((Address < ADDR_FLASH_SECTOR_7)
		&& (Address >= ADDR_FLASH_SECTOR_6))
{
	sector = FLASH_Sector_6;
}
else if ((Address < ADDR_FLASH_SECTOR_8)
		&& (Address >= ADDR_FLASH_SECTOR_7))
{
	sector = FLASH_Sector_7;
}
else if ((Address < ADDR_FLASH_SECTOR_9)
		&& (Address >= ADDR_FLASH_SECTOR_8))
{
	sector = FLASH_Sector_8;
}
else if ((Address < ADDR_FLASH_SECTOR_10)
		&& (Address >= ADDR_FLASH_SECTOR_9))
{
	sector = FLASH_Sector_9;
}
else if ((Address < ADDR_FLASH_SECTOR_11)
		&& (Address >= ADDR_FLASH_SECTOR_10))
{
	sector = FLASH_Sector_10;
}
else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
{
	sector = FLASH_Sector_11;
}

	return sector;
}

/**
 * @}
 */
