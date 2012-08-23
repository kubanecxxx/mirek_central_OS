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

/********************************************************************
 * Includes
 ********************************************************************/

/********************************************************************
 * Exported typedef
 ********************************************************************/
typedef union
{
	struct
	{
		bool_t overdrive :1;
		bool_t tuner :1;
	} s;
	uint8_t b;
} opto_bitStruct_t;
/********************************************************************
 * Exported global variables
 ********************************************************************/
extern opto_bitStruct_t _opto_states;

/********************************************************************
 * Exported macros
 ********************************************************************/
#define opto_setEffects(bitmask)		 _opto_states.b = bitmask
#define opto_getEffects()				(_opto_states.b)
#define opto_getEffect(number)			((opto_getEffects() >> number) & 1)
#define opto_enableEffect(number)		opto_setEffects(opto_getEffects() | (1<<number))
#define opto_disableEffect(number)		opto_setEffects(opto_getEffects() & (~(1<<number)))
#define opto_setEffect(number, state)	state == TRUE ? opto_enableEffect(number) : opto_disableEffect(number)

#define opto_disableOverdrive()			_opto_states.s.overdrive = FALSE;
#define opto_enableOverdrive() 			_opto_states.s.overdrive = TRUE;
#define opto_toggleOverdrive() 			_opto_states.s.overdrive = !_opto_states.s.overdrive;
#define opto_getOverdrive() 			(_opto_states.s.overdrive)

#define opto_disableTuner()				_opto_states.s.tuner = FALSE;
#define opto_enableTuner() 				_opto_states.s.tuner = TRUE;
#define opto_toggleTuner() 				_opto_states.s.tuner = !_opto_states.s.tuner;
#define opto_getTuner() 				(_opto_states.s.tuner)

/********************************************************************
 * Exported functions
 ********************************************************************/
void opto_init(void);

#ifdef __cplusplus
}
#endif

#endif /* OPTOCOUPLERS_H_ */
