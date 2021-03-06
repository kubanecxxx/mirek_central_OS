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
		bool_t bit20 :1;
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
	EFF_ENABLE = 3, EFF_DISABLE = 2, EFF_TOGGLE = 1, EFF_NOTHING = 0
} logic_effect_t;

/**
 * @brief barva ledky která bude blikat podle mapování na tlačítka
 * @ingroup LOGIC
 */
typedef enum
{
	COL_NONE = 0, COL_YELLOW = 1, COL_GREEN = 2, COL_BOTH = 3
} logic_ledColor_t;

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
		logic_effect_t eff15 :2;
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
	uint8_t VolumeOverloadEnabled;
	logic_ledColor_t led;

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
	const char * ChannelConditionName;
	///user data - harm setup, delay setup, led setup
	logic_dibit_t effects;

	///podminka předchozího kanálu
	uint8_t channelCondition;
	logic_specific_t * special;
	logic_marshall_t marshall;
	logic_ledColor_t led;
	uint8_t watchEffect;
	bool_t blikat;
	bool_t retreat;
	uint16_t overloadVolume;
	uint16_t overloadEff;
	logic_channel_t * prevChannel;
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
	logic_buttonCall_t ** ramCalls;
	struct
	{
		///jesli se má reagovat na mačkání nebo jenom po dobu držení
		bool_t hold :2;
		///jesli má reagovat hned nebo až po limitu
		bool_t now :2;
	} bit;
} logic_button_t;

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
	//todo přidat podporu uložení při změně v kanále
	bool_t save;
} logic_remap_t;

/**
 * @brief nastavení bendu v bance
 * @ingroup LOGIC_HL
 */
typedef struct
{
	uint16_t volume;
	uint8_t key;
	uint8_t harmony;
} logic_bend_t;

/**
 * @brief nastavení volume a gainu když se zapne wah
 * @ingroup LOGIC_HL
 */
typedef struct
{
	uint8_t volume;
	uint8_t gain;
} logic_wah_t;

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
	logic_bend_t bend;
	logic_wah_t wah;
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
	uint16_t time;
} logic_base_t;

typedef struct
{
	const logic_bank_t * bank;
	uint8_t activeChannel;
	const char * activeChannelName;
} logic_active_t;

#define SHELL_ERROR(string)	chprintf(chp,string); \
	fill_actives.zmrseno++

#define SHELL_ERROR_USER(string)	chprintf(chp,string)

/*
 * sada chybovéch hlášení v šelu - normálni přikazy
 */
#define SHELL_HARMONIST "Pouziti: bud jeden argument on/off, nebo prvni argument volume(0-4095); key(0-11); harmony(0-10); mode(0-4) a jeho hodnota\n"
#define SHELL_DELAY "Pouziti: bud jeden argument on/off, nebo prvni argument volume; time a jeho hodnota 0-255\n"
#define SHELL_RELAY "Pouziti: dva argumenty, prvni cislo rele a druhej 0/1 (vypnout/zapnout); cisla rele: \n 0 - zlute overdrive \n 1 - wah \n 2 - compresor \n 3 - phaser \n 4 - smrcka overdrive \n 5 - chorus \n 6 - detox \n (jenom ovladani pres rele)\n"
#define SHELL_OPTO "Pouziti: dva argumetny, prvni je cislo efektu (0 - overdrive, 1 - tuner) a druhej je 0/1 (jenom ovladani pres optocleny)\n"
#define SHELL_MARSHALL "Pouziti: dva argumenty prvni volume; gain; loop; mute; high; a druhej cislo 2-vypnout; 3-zapnout, gain a volume 1-4\n"

/*
 * sada chybovech hlaseni pro plneni
 */

#define SHELL_FILL_BANK "Vytvori novou banku. Pouziti: jeden argument - originalni jmeno banky (bude i na tlacitku)\n"

#define SHELL_FILL_CHANNEL_ADD "Vytvori novej kanal, defaultne vypne vsechny efekty marshala nastavi na preamp 1 a volume 1 , zapne smycku a nastavi high sens Pouziti: jeden argument - originalni jmeno kanalu (bude i na displeju)\n"
#define SHELL_FILL_CHANNEL_MARS "Do vytvorenyho kanalu nastavi marshala. Pouziti: vzdy dva argumenty, prvni muze byt preamp; volume; mute; sens; smycky \n preamp a volume potom 1-4; mute on/off; sens high/low; smycka on/off\n"
#define SHELL_FILL_CHANNEL_EFFS "Do vytvorenyho kanalu nastavi ktery efekty zapnout a ktery vypnout. Pouziti: vzdy dva argumenty prvni jmeno efektu (superdrive_true, wah, compresor phaser, overdrive, chorus, detox, superdrive_norm, tuner, dd3, harmonist) nebo cislo efektu 0 - 15	(0-6 jsou uz pojmenovany a pouzity, 7 a vejs je rezerva k dalsimu pouziti v budoucnu)\n"
#define SHELL_FILL_CHANNEL_SPECIAL "Pouziti: nastaveni delaye a harmonistu, vzdy tri argumenty, \n 	prvni je delay nebo harmonist \n delay ma potom dalsi nastaveni volume a time + jejich hodnota 0-255 \n harmonist ma nastaveni mode(0-4), key(0-11), harmony(0-10), volume(0-4095)\n"
#define SHELL_FILL_CHANNEL_OVERLOAD "povoli pretizeni hlasitosti kanalu on/off \n"
#define SHELL_FILL_CHANNEL_COLOR "vybere barvu ledku v kanalu zelena/zluta"

#define SHELL_FILL_FUNC_ADD "Vytvori novou funkci. Pouziti: jeden argument - originalni jmeno funkce, jeji defaultni nastaveni je ze nic nemeni\n"
#define SHELL_FILL_FUNC_MARS "Nastaveni marshala. Pouziti: dva argumenty preamp/volume/mute/sens/smycka, volume a preamp 1-4, 0 neudela nic; zbytek on/off/toggle\n"
#define SHELL_FILL_FUNC_EFFS "Nastaveni efektu ve funkci. Pouziti: dva argumenty, prvni je jmeno efektu jako v kanalu (nebudu to sem prepisovat jak trouba znova) a druhej argument je on/off/toggle (cokoliv jinyho neudela nic)\n"
#define SHELL_FILL_FUNC_LED "Nastaveni barvy ledky ktera ma sledovat funkci. Pouziti: jeden argument barva (zelena/zluta/obe) cokoliv jinyho neudela nic\n"
#define SHELL_FILL_FUNC_WATCH "Nastaveni kterej efekt ma funkce ledka sledovat jesli je zapnutej nebo ne. Pouziti: jeden argument jmeno efektu tak jako v kanalu a funkci (nebudu to zase cely prepisovat)\n"
#define SHELL_FILL_FUNC_CONDITION "nastaveni kanalu v kterym ma funkce fungovat. Pouziti: jenom jeden argument jmeno kanalu, pokud se prikaz vubec nenapise tak zadna podminka neni a pude to vzdycky\n"
#define SHELL_FILL_FUNC_BLINK "nastaveni jesli to ma blikat, jeden argument on/off\n"
#define SHELL_FILL_FUNC_RETREAT "nastaveni jesli se ma vratit do kanalu kterej ma jako podminku pokud se stoupne 2x , parametr on/off"
#define SHELL_FILL_FUNC_OVERLOAD "nastaveni jesli ma prebijet nastaveni kanalu parametr on/off"

#define SHELL_FILL_REMAP_ADD "Vytvori novy premapovani ktery muze mezi sebou prohodit na tlacitku dve funkce. Pouziti jenom jeden argument jmeno remapu\n"
#define SHELL_FILL_REMAP_CONDITION "Nastavi cislo kanalu v kterym ma remap fungovat jeden argument cislo kanalu\n"
#define SHELL_FILL_REMAP_BUTTON "Nastavi na kterym tlacitku ma premapovavat, jeden argument jmeno tlacitka\n"
#define SHELL_FILL_REMAP_OLD "Nastavi ktera funkce se ma prohodit, argument je meno funkce\n"

#define SHELL_FILL_BUTTON_ADD "Vytvori novy tlacitko, 3 argumenty - originalni jmeno tlacitka, cislo tlacitka na liste 1-8, a pocet zmacknuti\n"
#define SHELL_FILL_BUTTON_NOW "Nastavi ve vytvorenym tlacitku jesli bude reagovat hned, nebo az po casovym limitu, pouziti jeden argument hned/potom\n"
#define SHELL_FILL_BUTTON_CALL "Prida do tlacitka co ma volat, jeden argument - nekde uz pouzity originalni jmeno funkce/remapu/kanalu, radek se muze napsat vickrat pod sebe s ruznema volanima\n"

#define SHELL_FILL_BEND "Nastaveni bendu v bance. Pouziti 3 parametry, volume(0-4095), key (0-11), harmony(0-10)\n"
#define SHELL_FILL_TIME "Jeden argument - kolik ms ma cekat\n"

#define SHELL_FILL_WAH_VOLUME "nastaveni kterej volume kanal se vybere kdyz si zapnes wah; 0-4 (0 neudela nic)"
#define SHELL_FILL_WAH_GAIN "nastaveni kterej preamp kanal se vybere kdyz si zapnes wah; 0-4 (0 neudela nic)"

#endif /* LOGIC_TYPES_H_ */
