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

/*
 * function set of shell commands
 */
void cmd_functionAdd(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_function_special(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_function_marshall(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_function_effs(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_function_prevCondition(BaseSequentialStream *chp, int argc,
		char *argv[]);

#ifdef __cplusplus
}
#endif

#endif /* LOGIC_FILL_H_ */
