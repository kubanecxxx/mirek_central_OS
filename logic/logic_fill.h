/**
 * @file logic_fill.h
 * @author kubanec
 * @date 24.8.2012
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LOGIC_FILL_H_
#define LOGIC_FILL_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void cmd_bankAdd(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_openLogic(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_closeLogic(BaseSequentialStream *chp, int argc, char *argv[]);

/*
 * channel set of shell commands
 */
void cmd_channelAdd(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_channel_effs(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_channel_marshall(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_channel_special(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_channel_canOverload(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_channel_color(BaseSequentialStream *chp, int argc, char *argv[]);

/*
 * function set of shell commands
 */
void cmd_functionAdd(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_function_special(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_function_marshall(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_function_effs(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_function_prevCondition(BaseSequentialStream *chp, int argc,
		char *argv[]);
void cmd_function_led(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_function_watch(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_function_blink(BaseSequentialStream *chp, int argc, char * argv[]);
void cmd_function_retreat(BaseSequentialStream *chp, int argc, char * argv[]);
void cmd_function_overloadVolume(BaseSequentialStream *chp, int argc, char * argv[]);
void cmd_function_overloadEff(BaseSequentialStream *chp, int argc, char * argv[]);

/**
 * remap set of shell commands
 */
void cmd_remapAdd(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_remap_led(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_remap_prevCondition(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_remap_setButtonName(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_remap_setNewCallName(BaseSequentialStream *chp, int argc,
		char *argv[]);
void cmd_remap_setOldCallName(BaseSequentialStream *chp, int argc,
		char *argv[]);
void cmd_remap_save(BaseSequentialStream *chp, int argc, char *argv[]);

/**
 * button set of shell commands
 */
void cmd_buttonAdd(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_button_hold(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_button_now(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_button_call(BaseSequentialStream *chp, int argc, char *argv[]);

void cmd_bend(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_time(BaseSequentialStream *chp, int argc, char *argv[]);

void cmd_wahVolume(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_wahGain(BaseSequentialStream *chp, int argc, char *argv[]);

#ifdef __cplusplus
}
#endif

#endif /* LOGIC_FILL_H_ */
