/**
 * @file switch_lld.h
 * @author kubanec
 * @date 22.8.2012
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SWITCH_LLD_H_
#define SWITCH_LLD_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * REL 1-14 outputs
 * REL 1-7 used relays
 * REL 15 16 18 20 - optocouplers
 */

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/**
 * @brief Počet relé
 * @ingroup Relays
 */
#define RELAY_COUNT 11
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
bool_t switch_getRelay(uint8_t relay);
void switch_clearRelay(uint8_t relay);
void switch_setRelay(uint8_t relay);
void switch_setRelays(uint32_t relays);
void switch_toggleRelay(uint8_t relay);
uint32_t switch_getRelays(void);
void switch_init(void);

#ifdef __cplusplus
}
#endif

#endif /* SWITCH_LLD_H_ */
