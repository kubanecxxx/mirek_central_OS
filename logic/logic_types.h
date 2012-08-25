/*
 * logic_types.h
 *
 *  Created on: 22.8.2012
 *      Author: kubanec
 */

#ifndef LOGIC_TYPES_H_
#define LOGIC_TYPES_H_

/********************************************************************
 * Includes
 ********************************************************************/
#include "ch.h"
#include "footswitch.h"

/********************************************************************
 * Exported types
 ********************************************************************/

/*
 banky
 ------kanály
 ------------name
 ------------special
 ------funkce
 ------------name
 ------------special
 ------remapy
 ------------name
 ------------button
 ------tlačitka
 ------------name
 ------------calls
 -----------------call
 */

/**
 * @defgroup Logika
 * @brief všechny datové typy použité při ukládání/rozhodování chování boardu
 */

/**
 * @defgroup LOGIC_HL
 * @ingroup Logika
 * @brief high level datové typy v chování boardu
 * @details Z těchto 4 struktur se poskládá jakykoliv nastavení
 */

/**
 * @defgroup LOGIC
 * @brief low level datové typy v chování boardu
 * @ingroup LOGIC_HL
 */

/**
 * @brief efekty přes relé, jednotlivy bity/cely slovo; zapnout/vypnout
 * @ingroup LOGIC
 */
typedef union
{
	struct
	{
		bool_t bit0 :1;
		bool_t bit1 :1;
		bool_t bit2 :1;
		bool_t bit3 :1;
		bool_t bit4 :1;
		bool_t bit5 :1;
		bool_t bit6 :1;
		bool_t bit7 :1;
		bool_t bit8 :1;
		bool_t bit9 :1;
		bool_t bit10 :1;
		bool_t bit11 :1;
		bool_t bit12 :1;
		bool_t bit13 :1;
		bool_t bit14 :1;
		bool_t bit15 :1;
		bool_t bit16 :1;
		bool_t bit17 :1;
		bool_t bit18 :1;
		bool_t bit19 :1;
		bool_t bit21 :1;
		bool_t bit22 :1;
		bool_t bit23 :1;
		bool_t bit24 :1;
		bool_t bit25 :1;
		bool_t bit26 :1;
		bool_t bit27 :1;
		bool_t bit28 :1;
		bool_t bit29 :1;
		bool_t bit30 :1;
		bool_t bit31 :1;
	} s;
	uint32_t w;
} logic_bit_t;

/**
 * @brief nastavení efektů přes relé ve funkci - výčet stavů
 * @ingroup LOGIC
 */
typedef enum
{
	EFF_ENABLE, EFF_DISABLE, EFF_TOGGLE, EFF_NOTHING
} logic_effect_t;

/**
 * @brief nastavení efektů ve funkcích po dvojbitech/cely slovo
 * @details stavy se použivaji z: @ref logic_effect_t
 * @ingroup LOGIC
 */
typedef union
{
	struct
	{
		logic_effect_t eff0 :2;
		logic_effect_t eff1 :2;
		logic_effect_t eff2 :2;
		logic_effect_t eff3 :2;
		logic_effect_t eff4 :2;
		logic_effect_t eff5 :2;
		logic_effect_t eff6 :2;
		logic_effect_t eff7 :2;
		logic_effect_t eff8 :2;
		logic_effect_t eff9 :2;
		logic_effect_t eff10 :2;
		logic_effect_t eff11 :2;
		logic_effect_t eff12 :2;
		logic_effect_t eff13 :2;
		logic_effect_t eff14 :2;
		logic_effect_t eff16 :2;
		logic_effect_t eff17 :2;
		logic_effect_t eff18 :2;
		logic_effect_t eff19 :2;
		logic_effect_t eff20 :2;
		logic_effect_t eff21 :2;
		logic_effect_t eff22 :2;
		logic_effect_t eff23 :2;
		logic_effect_t eff24 :2;
		logic_effect_t eff25 :2;
		logic_effect_t eff26 :2;
		logic_effect_t eff27 :2;
		logic_effect_t eff28 :2;
		logic_effect_t eff29 :2;
		logic_effect_t eff30 :2;
		logic_effect_t eff31 :2;
	} s;
	uint64_t w;
} logic_dibit_t;

/**
 * @brief struktura pro nastavení delaye
 * @ingroup LOGIC
 */
typedef struct
{
	uint16_t volume;
	uint16_t time;
} logic_delay_t;

/**
 * @brief struktura pro nastavení harmonizéru
 * @ingroup LOGIC
 */
typedef struct
{
	uint8_t mode;
	uint8_t key;
	uint8_t harmony;
	uint16_t volume;
} logic_harmonist_t;

/**
 * @brief specifické nastavení v kanálu/fci - delay,harmonist,leds
 * @details používá: @ref logic_delay_t @ref logic_harmonist_t @ref logic_leds_t
 * @ingroup LOGIC
 */
typedef struct
{
	//delay setting
	logic_delay_t delay;
	//harmonizer setting
	logic_harmonist_t harmonist;
} logic_specific_t;

/**
 * @brief nastavení maršála
 * @ingroup LOGIC
 */
typedef struct
{
	uint8_t gain :4;
	uint8_t volume :4;

	logic_effect_t mute :2;
	logic_effect_t high :2;
	logic_effect_t effLoop :2;

} logic_marshall_t;

/**
 * @brief nastavení kanálu
 * @ingroup LOGIC_HL
 * @details kanál bude natvrdo nastavovat stavy efektů (nebude umět jenom přidat nebo jenom ubrat)
 *
 * používá: @ref logic_bit_t @ref logic_specific_t
 */
typedef struct
{
	const char * name;
	logic_bit_t effects;
	uint8_t index;

	logic_specific_t * special;
	logic_marshall_t marshall;
} logic_channel_t;

/**
 * @brief nastavení funkce
 * @details funkce bude umět to co kanál +
 * efekty přidávat a ubirat, proto je potřeba použit @ref logic_dibit_t +
 * obsahuje podmínku v kterym se nachází kanálu
 *
 * používá: @ref logic_dibit_t @ref logic_specific_t
 * @ingroup LOGIC_HL
 *
 */
typedef struct
{
	///jméno
	const char * name;
	///user data - harm setup, delay setup, led setup
	logic_dibit_t effects;

	///podminka předchozího kanálu
	uint8_t channelCondition;
	logic_specific_t * special;
	logic_marshall_t marshall;

} logic_function_t;

/**
 * @brief sada typů volání funkcí - použito v argumentu volání
 * @ingroup LOGIC
 */
typedef enum
{
	callType_channel, callType_function, callType_remap
} logic_callType_t;

/**
 * @brief argument volání
 * @ingroup LOGIC
 *
 * @details potřeba zadávat typ @ref logic_callType_t,
 * aby věděl které funkci má argument předat
 */
typedef struct
{
	///typ volání (funkce kanál efekt repam0)
	logic_callType_t callType;
	///pointer na argument
	const void * call;
	///jméno argumetu pro usera
	const char * CallName;

} logic_buttonCall_t;

/**
 * @brief nastavení tlačítka
 * @details tato struktura se bude volat při zmáčknutí potřebného
 * tlačítka několikrát (taky určeno ve struktuře)
 * a podle ní se zavolají potřebné funkce s parametrama
 * kanál,funkce,remap
 *
 * @ingroup LOGIC_HL
 */
typedef struct
{
	///jméno
	const char * name;
	foot_t button;
	///počet argumentů volání
	uint8_t buttonCallCount;
	///sada pointrů
	logic_buttonCall_t * calls;
	struct
	{
		///jesli se má reagovat na mačkání nebo jenom po dobu držení
		bool_t hold :1;
		///jesli má reagovat hned nebo až po limitu
		bool_t now :1;
	} bit;
} logic_button_t;

/**
 * @brief barva ledky která bude blikat podle mapování na tlačítka
 * @ingroup LOGIC
 */
typedef enum
{
	COL_NONE, COL_YELLOW, COL_GREEN, COL_BOTH
} logic_ledColor_t;

/**
 * @brief struktura pro blikání ledkou podle mapování ve vlákně
 * @ingroup LOGIC
 */
typedef struct
{
	logic_ledColor_t ledColor;
	uint8_t ledNumber;
} logic_blinking;

/**
 * @brief přemapování volání tlačítek na jiné
 * @ingroup LOGIC_HL
 *
 * @details
 * určuje z kteryho tlačitka ktery volání se přemapuje
 * a na ktery se přemapuje, proto je potřeba @ref logic_buttonCall_t
 * +taky podmínka předchoziho kanálu
 */
typedef struct
{
	const char * name;
	///na tohle tlačítko bude namapovávat - zbytek si budu muset vypočitat než zapišu do flaš
	const char * ButtonName;
	///podmínka předchozího kanálu
	uint8_t channelCondition;
	///barva ledky která bude blikat pokud bude namapovany na jaky tlačitko
	logic_ledColor_t ledBlinkColor;
	///interně si vypočte před zápisem do flašky přesně tlačitko k přemapování
	foot_t button;
	///interně si vypočte před záspisem do flašky přesně argument a jeho typ
	logic_buttonCall_t newCall;
	logic_buttonCall_t oldCall;
} logic_remap_t;

/**
 * @brief celá banka
 * @ingroup LOGIC_HL
 *
 * @details obsahuje sadu kanálů, funkcí, tlačitek a přemapování
 * uživatel si muže nastavit na každou banku uplně něco jinyho
 */
typedef struct
{
	///jméno
	const char * name;

	uint16_t channelCount;
	uint16_t functionCount;
	uint16_t remapCount;
	uint16_t buttonCount;

	//pointer to first item in list, limited by xxxCount
	logic_channel_t * channels;
	logic_function_t * functions;
	logic_remap_t * remaps;
	logic_button_t * buttons;
} logic_bank_t;

/**
 * @brief sada bank
 * @ingroup LOGIC_HL
 *
 * @details
 * zarhnuje všechny banky + jejich počet
 */
typedef struct
{
	///počet bank
	uint16_t bankCount;
	///sada pointrů
	logic_bank_t * banks;
} logic_base_t;

#endif /* LOGIC_TYPES_H_ */
