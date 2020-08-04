#include "stm32f1xx_hal.h"
#include "EEPROM.h"
#include "Flash.h"
#include "IridiumBytes.h"

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
