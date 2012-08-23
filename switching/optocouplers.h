/*
 * optocouplers.h
 *
 *  Created on: 23.8.2012
 *      Author: kubanec
 */

#ifndef OPTOCOUPLERS_H_
#define OPTOCOUPLERS_H_

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup Optočleny
 * @ingroup Switching
 * @brief API pro ovládání efektů s optočlenama
 */

/********************************************************************
 * Includes
 ********************************************************************/

/********************************************************************
 * Exported typedef
 ********************************************************************/
/**
 * @brief není využito, používám jenom .b
 * @ingroup Optočleny
 */
typedef union
{
	uint8_t b;
} opto_bitStruct_t;
/********************************************************************
 * Exported global variables
 ********************************************************************/
/**
 * @brief interní proměnná pro nastavení stavu efektů
 * @note externí jenom kvůli makrům
 * @ingroup Optočleny
 * @notapi
 */
extern opto_bitStruct_t _opto_states;

/********************************************************************
 * Exported macros
 ********************************************************************/
/**
 * @brief Nastavení efektů (portů) podle bitové masky
 * @ingroup Optočleny
 * @param[in] bitmask bitová maska
 */
#define opto_setEffects(bitmask)		 _opto_states.b = bitmask
/**
 * @brief Čtení stavu efektů z pinů
 * @ingroup Optočleny
 */
#define opto_getEffects()				(_opto_states.b)
/**
 * @brief Přečte jeden efekt podle čísla
 * @ingroup Optočleny
 * @param[in] number číslo efektu s optočlenama
 */
#define opto_getEffect(number)			((opto_getEffects() >> number) & 1)
/**
 * @brief Zapne jeden efekt
 * @ingroup Optočleny
 * @param[in] number číslo efektu
 */
#define opto_enableEffect(number)		opto_setEffects(opto_getEffects() | (1<<number))
/**
 * @brief Vypne jeden efekt
 * @ingroup Optočleny
 * @param[in] number číslo efektu
 */
#define opto_disableEffect(number)		opto_setEffects(opto_getEffects() & (~(1<<number)))
/**
 * @brief Přepne jeden efekt
 * @ingroup Optočleny
 * @param[in] number číslo efektu
 */
#define opto_toggleEffect(number)		opto_setEffects(opto_getEffects() ^ (1<<number))

/********************************************************************
 * Exported functions
 ********************************************************************/
void opto_init(void);

#ifdef __cplusplus
}
#endif

#endif /* OPTOCOUPLERS_H_ */
