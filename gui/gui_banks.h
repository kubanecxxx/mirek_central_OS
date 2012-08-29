/**
 * @file gui_banks.h
 * @author kubanec
 * @date 28.8.2012
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef GUI_BANKS_H_
#define GUI_BANKS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void gui_bankScreenInit(touch_callback cb);
void gui_putBankScreen(void);

#ifdef __cplusplus
}
#endif

#endif /* GUI_BANKS_H_ */
