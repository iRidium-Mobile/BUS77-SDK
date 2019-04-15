#ifndef _FLASH_H_INCLUDED_
#define _FLASH_H_INCLUDED_

#include "IridiumTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

// Работа с Flash памятью
void FLASH_Clear(size_t in_stStart, size_t in_stEnd);
void FLASH_Write(u8* in_pBuffer, size_t in_stFlash, size_t in_stSize);
void FLASH_Read(u8* in_pBuffer, size_t in_stFlash, size_t in_stSize);
bool FLASH_Test(u8* in_pBuffer, size_t in_stFlash, size_t in_stSize);
   
#ifdef __cplusplus
}
#endif
#endif   // _FLASH_H_INCLUDED_
