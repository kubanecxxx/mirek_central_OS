/*
 * switch_master.c
 *
 *  Created on: 23.8.2012
 *      Author: kubanec
 */

/********************************************************************
 * Includes
 ********************************************************************/
#include "ch.h"
#include "hal.h"
#include "switch_master.h"

/********************************************************************
 * Private functions
 ********************************************************************/
void switch_masterGpioInit(void)
{
	switch_init();
	opto_init();
}

