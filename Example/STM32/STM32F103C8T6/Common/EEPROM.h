#ifndef _EEPROM_H_INCLUDED_
#define _EEPROM_H_INCLUDED_

#include "IridiumTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

// Инициализация энергонезависимой памяти
void EEPROM_Init(size_t in_stStart, size_t in_stEnd);
   
// Чтение/запись энергонезависимой памяти
u8 EEPROM_ReadU8(size_t in_stIndex);
void EEPROM_WriteU8(size_t in_stIndex, u8 in_u8Value);
u16 EEPROM_ReadU16(size_t in_stIndex);
void EEPROM_WriteU16(size_t in_stIndex, u16 in_u16Value);
u32 EEPROM_ReadU32(size_t in_stIndex);
void EEPROM_WriteU32(size_t in_stIndex, u32 in_u32Value);

// Буферизация EEPROM
void EEPROM_SetBuffer(u8* in_pBuffer, size_t in_stSize);
void EEPROM_NeedSaveBuffer(void);
bool EEPROM_ForceSaveBuffer(void);
void EEPROM_WorkBuffer(void);
   
#ifdef __cplusplus
}
#endif
#endif   // _EEPROM_H_INCLUDED_
