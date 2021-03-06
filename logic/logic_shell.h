/**
 * @file logic_shell.h
 * @author kubanec
 * @date 24.8.2012
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LOGIC_SHELL_H_
#define LOGIC_SHELL_H_

{	"open" , cmd_openLogic},
{	"close" , cmd_closeLogic},
{	"banka" , cmd_bankAdd},
{	"kanal" , cmd_channelAdd},
{	"kan_marshall" , cmd_channel_marshall},
{	"kan_efekty" , cmd_channel_effs},
{	"kan_special" , cmd_channel_special},
{	"kan_overload" , cmd_channel_canOverload},
{	"kan_led" , cmd_channel_color},
{	"funkce", cmd_functionAdd},
{	"fun_marshall" , cmd_function_marshall},
{	"fun_efekty" , cmd_function_effs},
{	"fun_special" , cmd_function_special},
{	"fun_podminka" , cmd_function_prevCondition},
{	"fun_led" , cmd_function_led},
{	"fun_watch" , cmd_function_watch},
{	"fun_blink", cmd_function_blink},
{	"fun_retreat", cmd_function_retreat},
{	"fun_overloadVolume" , cmd_function_overloadVolume},
{	"fun_overloadEff" , cmd_function_overloadEff},
{	"remap", cmd_remapAdd},
//{	"rem_led" , cmd_remap_led},
{	"rem_podminka", cmd_remap_prevCondition},
{	"rem_tlacitko" ,cmd_remap_setButtonName},
{	"rem_jedna" , cmd_remap_setNewCallName},
{	"rem_dva", cmd_remap_setOldCallName},
//{	"rem_save", cmd_remap_save},
{	"tlacitko", cmd_buttonAdd},
{	"tlac_nast" , cmd_button_call},
{	"tlac_hned" , cmd_button_now},
//{	"tlac_drzet", cmd_button_hold},
{	"bend", cmd_bend},
{	"time", cmd_time},
{	"wah_volume" , cmd_wahVolume},
{	"wah_gain" , cmd_wahGain},

#endif /* LOGIC_SHELL_H_ */
