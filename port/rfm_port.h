/*
 * rfm_port.c
 *
 *  Created on: 5.6.2012
 *      Author: kubanec
 */

#ifndef RFM_PORT_C_
#define RFM_PORT_C_

#include "ch.h"
#include "hal.h"

#define SPI_SOFT
//#define RFM_868

#ifndef SPI_SOFT
#define SPI_HARD
#endif

#ifdef SPI_HARD
#define SPI_RFM SPID2
#endif

#define SCK 5
#define MISO 6
#define MOSI 7
#define NSS 4
#define SPI_GPIO GPIOA

#define FFIT	7 //GPIOB
#define NIRQ	8 //GPIOB

void low_level_spi_init(void);
void low_level_wait_ffit_high(void);
void low_level_wait_nirq_low(void);
uint16_t low_level_spi_in_out(uint16_t data);

#if (defined SPI_SOFT && defined SPI_HARD)
	#error oboje SPI rfm_port.h
#endif

#if (!defined SPI_SOFT && !defined SPI_HARD)
	#error ani jedno SPI rfm_port.h
#endif

#endif /* RFM_PORT_C_ */
