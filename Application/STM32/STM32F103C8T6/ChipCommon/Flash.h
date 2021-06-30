#ifndef _FLASH_H_INCLUDED_
#define _FLASH_H_INCLUDED_

#include "IridiumTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

// Работа с Flash памятью
bool FLASH_Clear(size_t in_stStart, size_t in_stEnd);
bool FLASH_Write(u8* in_pBuffer, size_t in_stFlash, size_t in_stSize);

#ifdef __cplusplus
}
#endif
#endif   // _FLASH_H_INCLUDED_
