/*
 * rfm_port.c
 *
 *  Created on: 5.6.2012
 *      Author: kubanec
 */

#include "rfm_port.h"
#include "stm32f4xx_gpio.h"

#ifdef SPI_HARD

const SPIConfig spi1conf =
{	NULL, GPIOA, 4, SPI_CR1_BR_2 | SPI_CR1_BR_1};

void low_level_spi_init(void)
{
	palSetPadMode(GPIOA, 7, PAL_MODE_ALTERNATE(GPIO_AF_SPI1));
	palSetPadMode(GPIOA, 6, PAL_MODE_ALTERNATE(GPIO_AF_SPI1));
	palSetPadMode(GPIOA, 5, PAL_MODE_ALTERNATE(GPIO_AF_SPI1));
	//CS
	palSetPadMode(GPIOA, 4, PAL_MODE_OUTPUT_PUSHPULL);
	//FFIT
	palSetPadMode(GPIOB, 7, PAL_MODE_INPUT);
	//NIRQ
	palSetPadMode(GPIOB, 8, PAL_MODE_INPUT);

	spiAcquireBus(&SPI_RFM);
	spiStart(&SPI_RFM, &spi1conf);
	spiUnselect(&SPI_RFM);
	spiReleaseBus(&SPI_RFM);
}

uint16_t low_level_spi_in_out(uint16_t data)
{
	uint16_t ret;
	spiAcquireBus(&SPI_RFM);
	spiSelect(&SPI_RFM);
	spiExchange(&SPI_RFM, 2, &data, &ret);
	spiUnselect(&SPI_RFM);
	spiReleaseBus(&SPI_RFM);

	return ret;
}
#endif

#ifdef SPI_SOFT
void low_level_spi_init(void)
{
	palSetPadMode(GPIOA, MOSI, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOA, MISO, PAL_MODE_INPUT);
	palSetPadMode(GPIOA, NSS, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOA, SCK, PAL_MODE_OUTPUT_PUSHPULL);

	palSetPad(GPIOA,MOSI);
	palSetPad(GPIOA,NSS);
	palSetPad(GPIOA,SCK);
}

#define spiClearPad(pad) palClearPad(SPI_GPIO,pad)
#define spiSetPad(pad) palSetPad(SPI_GPIO,pad)
#define spiReadPad(pad) palReadPad(SPI_GPIO,pad)
#define SULIN chThdSleepMicroseconds(100)

uint16_t low_level_spi_in_out(uint16_t cmd)
{
	unsigned char i;
	unsigned int recv = 0;

	spiClearPad(SCK);
	spiClearPad(NSS);

	for (i = 0; i < 16; i++)
	{
		SULIN;
		if (cmd & 0x8000)
			spiSetPad(MOSI);
		else
			spiClearPad(MOSI);
		SULIN;
		spiSetPad(SCK);

		SULIN;
		recv <<= 1;
		recv += spiReadPad(MISO);

		SULIN;
		spiClearPad(SCK);
		cmd <<= 1;
	}
	SULIN;
	spiSetPad(NSS);
	return recv;
}
#endif

#define rfmReadPad(pad) palReadPad(GPIOB,pad)
/**
 * @todo časem by se to mohlo zkusit trochu inteligentně přes přerušeni a pak DMA
 */
void low_level_wait_ffit_high(void)
{
	while (rfmReadPad(FFIT) == 0)
		continue;
}

void low_level_wait_nirq_low(void)
{
	while (rfmReadPad(NIRQ) == 1)
		continue;
}

