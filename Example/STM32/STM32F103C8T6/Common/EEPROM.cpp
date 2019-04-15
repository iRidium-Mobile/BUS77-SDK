#include "stm32f1xx_hal.h"
#include "IridiumTypes.h"
#include "EEPROM.h"
#include "Flash.h"
#include "Bytes.h"

#define EEPROM_SAVE_TIME   100

//////////////////////////////////////////////////////////////////////////
// Работа с flash
//////////////////////////////////////////////////////////////////////////
bool     g_bNeedSave = false;
u8*      g_pEEPROMBuffer = NULL;                   // Указатель на промежуточный буфер
size_t   g_stEEPROMSize  = 0;                      // Размер промежуточного буфера
u32      g_u32LastTimeSave = 0;                    // Последнее время сохранения данных из промежуточного буфера во flash
size_t   g_stEEPROMStart = 0;                      // Начало EEPROM памяти
size_t   g_stEEPROMEnd = 0;                        // Конец EEPROM памяти

/**
   Инициализация EEPROM памяти
   на входе    :  in_stStart  - начало EEPROM памяти
                  in_stEnd    - конец EEPROM памяти
   на выходе   :  *
*/
void EEPROM_Init(size_t in_stStart, size_t in_stEnd)
{
   g_stEEPROMStart = in_stStart;
   g_stEEPROMEnd = in_stEnd;
}

/**
   Чтение 8 битного беззнакового значения из указанной EEPROM памяти
   на входе  :  in_stIndex   - индекс в EEPROM памяти
   на выходе :  полученное значение
*/
u8 EEPROM_ReadU8(size_t in_stIndex)
{
   u8 l_u8Value = 0;
   ReadU8(g_pEEPROMBuffer + in_stIndex, l_u8Value);
   return l_u8Value;
}

/**
   Запись 8 битного беззнакового значения в указанную EEPROM память
   на входе  :  in_stIndex - индекс в EEPROM памяти
                in_u8Value - значение для записи
   на выходе :  *
*/
void EEPROM_WriteU8(size_t in_stIndex, u8 in_u8Value)
{
   WriteU8(g_pEEPROMBuffer + in_stIndex, in_u8Value);
}

/**
   Чтение 16 битного беззнакового значения из указанной EEPROM памяти
   на входе  :  in_stIndex - индекс в EEPROM памяти
   на выходе :  полученное значение
*/
u16 EEPROM_ReadU16(size_t in_stIndex)
{
   u16 l_u16Value = 0;
   ReadU16LE(g_pEEPROMBuffer + in_stIndex, l_u16Value);
   return l_u16Value;
}

/**
   Запись 16 битного беззнакового значения в указанную EEPROM память
   на входе  :  in_stIndex    - индекс в EEPROM памяти
                in_u16Value   - значение для записи
   на выходе :  *
*/
void EEPROM_WriteU16(size_t in_stIndex, u16 in_u16Value)
{
   WriteU16LE(g_pEEPROMBuffer + in_stIndex, in_u16Value);
}

/**
   Чтение 32 битного беззнакового значения из указанной EEPROM памяти
   на входе  :  in_stIndex - индекс в EEPROM памяти
   на выходе :  полученное значение
*/
u32 EEPROM_ReadU32(size_t in_stIndex)
{
   u32 l_u32Value = 0;
   ReadU32LE(g_pEEPROMBuffer + in_stIndex, l_u32Value);
   return l_u32Value;
}

/**
   Запись 32 битного беззнакового значения в указанную EEPROM память
   на входе  :  in_stIndex    - индекс в EEPROM памяти
                in_u16Value   - значение для записи
   на выходе :  *
*/
void EEPROM_WriteU32(size_t in_stIndex, u32 in_u32Value)
{
   WriteU32LE(g_pEEPROMBuffer + in_stIndex, in_u32Value);
}

/**
   Инициализация EEPROM
   на входе    :  in_pBuffer  - указатель на буфер с данными 
                  in_stSize   - размер промежуточного буфера
   на выходе   :  *
*/
void EEPROM_SetBuffer(u8* in_pBuffer, size_t in_stSize)
{
   // Проверка на выход за границы
   if(in_stSize > (g_stEEPROMEnd - g_stEEPROMStart))
      in_stSize = g_stEEPROMEnd - g_stEEPROMStart;
   
   // Запомним указатель
   g_pEEPROMBuffer   = in_pBuffer;
   g_stEEPROMSize    = in_stSize;
   
   // Чтение данных в промежуточный буфер
   FLASH_Read(g_pEEPROMBuffer, g_stEEPROMStart, g_stEEPROMSize);
}

/**
   Обработчик запроса сохранения данных
   на входе    :  *
   на выходе	:  *
*/
void EEPROM_NeedSaveBuffer()
{
   g_bNeedSave = true;
   g_u32LastTimeSave = HAL_GetTick();
}

/**
   Обработчик принудительной записи данных
   на входе    :  *
   на выходе	:  *
*/
bool EEPROM_ForceSaveBuffer()
{
   bool l_bResult = false;
   // Разблокирование дуступа к флеш
   HAL_FLASH_Unlock();
   // Очистка страницы
   FLASH_Clear(g_stEEPROMStart, g_stEEPROMEnd);
   // Запись всей EEPROM
   FLASH_Write(g_pEEPROMBuffer, g_stEEPROMStart, g_stEEPROMSize);
   l_bResult = FLASH_Test(g_pEEPROMBuffer, g_stEEPROMStart, g_stEEPROMSize);
   // Блокирование дуступа к флеш
   HAL_FLASH_Lock();
   return l_bResult;
}

/**
   Обработчик сохранения данных
   на входе    :  *
   на выходе	:  *
*/
void EEPROM_WorkBuffer()
{
   if(g_bNeedSave && (g_u32LastTimeSave + EEPROM_SAVE_TIME < HAL_GetTick()))
   {
      // Запись данных
      EEPROM_ForceSaveBuffer();
      g_bNeedSave = false;
   }
}
