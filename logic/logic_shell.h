/**
 * @file logic_shell.h
 * @author kubanec
 * @date 24.8.2012
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LOGIC_SHELL_H_
#define LOGIC_SHELL_H_

{"open" , cmd_openLogic},
{ "close" , cmd_closeLogic},
{"banka" , cmd_bankAdd},
{"kanal" , cmd_channelAdd},
{"kan_marshall" , cmd_channel_marshall},
{"kan_efekty" , cmd_channel_effs},
{"kan_special" , cmd_channel_special},
{"funkce", cmd_functionAdd},
{"fun_marshall" , cmd_function_marshall},
{"fun_efekty" , cmd_function_effs},
{"fun_special" , cmd_function_special},
{"fun_podminka" , cmd_function_prevCondition},

#endif /* LOGIC_SHELL_H_ */
